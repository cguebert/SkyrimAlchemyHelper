#include "Save.h"
#include "zlib\zlib.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

void Save::parse(const std::string& fileName)
{
	Save save(fileName);
	save.doParse();
}

Save::Save(const std::string& fileName)
{
	ifstream stream(fileName, ios::binary | ios::in);
	if (!stream.is_open())
	{
		cout << "Cannot open " << fileName << endl;
		return;
	}

	in.setStream(std::move(stream));
}

void Save::doParse()
{
	parseHeader();
	parseFormIDArray();
	parseChangeForms();
}

void Save::parseHeader()
{
	std::array<char, 13> magic;
	in >> magic;

	uint32_t headerSize, version, saveNumber, playerLevel;
	in >> headerSize >> version >> saveNumber;

	string playerName = in.readWString();
	in >> playerLevel;
	string playerLocation = in.readWString();

	uint16_t stringSize;
	in >> stringSize; in.jump(stringSize); // gameDate
	in >> stringSize; in.jump(stringSize); // playerRaceEditorId

	in.jump(18); // playerSex + playerCurExp + playerLvlUpExp + filetime
	uint32_t shotWidth, shotHeight;
	in >> shotWidth >> shotHeight;
	in.jump(3 * shotWidth * shotHeight);

	cout << playerName << endl;
	cout << playerLocation << endl;

	uint8_t formVersion;
	in >> formVersion;

	uint32_t pluginInfoSize;
	in >> pluginInfoSize;

	uint8_t nbPlugins;
	in >> nbPlugins;
	for (int i = 0; i < nbPlugins; ++i)
		m_plugins.push_back(in.readWString());

	in >> m_formIDArrayCountOffset >> m_unknownTable3Offset >> m_globalDataTable1Offset
		>> m_globalDataTable2Offset >> m_changeFormsOffset >> globalDataTable3Offset
		>> m_globalDataTable1Count >> m_globalDataTable2Count >> m_globalDataTable3Count
		>> m_changeFormCount;
}

bool isPresent(const vector<unsigned char>& buffer, const vector<unsigned char>& value)
{
	return search(buffer.begin(), buffer.end(), value.begin(), value.end()) != buffer.end();
}

void Save::parseChangeForms()
{
	in.seekg(m_changeFormsOffset);

	ofstream ingredientsRefIDs("data/IngrRefIDs.txt");
	ofstream knownIngredients("data/Known_Ingredients.txt");

	for (uint32_t i = 0; i < m_changeFormCount; ++i)
	{
		RefID refID;
		in >> refID;

		uint32_t changeFlags;
		in >> changeFlags;

		uint8_t type, formType, lengthSize, version;
		in >> type >> version;
		formType = type & 0x3F;
		lengthSize = type >> 6;

		if (version != 74)
		{
			cerr << "Error in parsing change forms: wrong version" << endl;
			return;
		}

		uint32_t length1, length2;
		switch (lengthSize)
		{
		case 0:
		{
			uint8_t l1, l2;
			in >> l1 >> l2;
			length1 = l1;
			length2 = l2;
			break;
		}
		case 1:
		{
			uint16_t l1, l2;
			in >> l1 >> l2;
			length1 = l1;
			length2 = l2;
			break;
		}
		case 2:
		{
			in >> length1 >> length2;
			break;
		}
		default:
			cerr << "Error in parsing change forms: type is 3?" << endl;
			return;
		}

		if (formType == 16) // Known ingredients
		{
			if (length1 == 4 && !length2)
			{
				uint32_t formID = getFormID(refID);
				ingredientsRefIDs << hex << uppercase << setfill('0') << setw(8) << formID 
					<< " " << setw(2) << (int)refID[0] 
					<< setw(2) << (int)refID[1]
					<< setw(2) << (int)refID[2] << endl;

				uint8_t modID = formID >> 24;
				uint32_t ingID = formID & 0x00FFFFFF;
				knownIngredients << hex << uppercase << setfill('0') << setw(8) << ingID << endl;
				knownIngredients << m_plugins[modID] << endl;

				uint8_t data;
				in >> data;
				in.jump(3);

				for (int j = 0; j < 4; ++j)
					knownIngredients << ((data & (1 << j)) != 0) << " ";
				knownIngredients << endl;
			}
			else
				in.jump(length1);
		}
		else if (formType == 0 || formType == 1)
		{
			uint32_t formID = getFormID(refID);
			if (formType == 1 && formID != 0x14) // If actor, only parse player
			{
				in.jump(length1);
				continue;
			}
			
			if (length1)
			{
				std::vector<unsigned char> decompressedData;

				if (length2)
				{
					std::vector<unsigned char> compressedData;
					compressedData.resize(length1);
					decompressedData.resize(length2);
					in >> compressedData;

					uLongf decSize = length2;
					uncompress(&decompressedData[0], &decSize, compressedData.data(), length1);
				}
				else
				{
					decompressedData.resize(length1);
					in >> decompressedData;
				}

				if ((formID & 0xFF000000) == 0x2B000000 && decompressedData.size() > 250)
			//	if (isPresent(decompressedData, { 0x43, 0xAD, 0x5B }) && isPresent(decompressedData, { 0x43, 0xAD, 0x60 }))
				{
					cout << "Found container: i " << i << " formType " << (int)formType << " formID ";
					cout << hex << uppercase << formID << dec << endl;
					cout << "changeFlags " << hex << changeFlags << dec << endl;
					cout << "l1 " << length1 << " l2 " << length2 << endl;

					ostringstream ss;
					ss << "data/0x" << hex << uppercase << setfill('0') << setw(8) << formID << ".data";
					ofstream out(ss.str(), ios::binary | ios::out);
					out.write(reinterpret_cast<char*>(decompressedData.data()), decompressedData.size());
				}
			}
		}
		else
			in.jump(length1);
	}
}

void Save::parseFormIDArray()
{
	in.seekg(m_formIDArrayCountOffset);
	uint32_t count;
	in >> count;

	m_formIDArray.resize(count);
	in >> m_formIDArray;

	ofstream out("data/formIDs.data", ios::binary);
	out.write(reinterpret_cast<char*>(m_formIDArray.data()), count * 4);
}

uint32_t Save::getFormID(const RefID& refID)
{
	uint8_t type = refID[0] >> 6;
	uint32_t value = ((refID[0] & 0x3F) << 16) | (refID[1] << 8) | refID[2];
	switch (type)
	{
	case 0:
		if (!value)
			return 0;
		else
			return m_formIDArray[value - 1];
	case 1:
		return value;
	case 2:
		return 0xFF000000 | value;
	default:
	case 3:
		cerr << "Error in parsing form ID: type is 3?" << endl;
		return 0;
	}
}