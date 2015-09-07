#include "Save.h"
#include "zlib/zlib.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

bool Save::parse(const std::string& fileName)
{
	ifstream stream(fileName, ios::binary | ios::in);
	if (!stream.is_open())
	{
		cout << "Cannot open " << fileName << endl;
		return false;
	}

	in.setStream(std::move(stream));

	doParse();
	return true;
}

void Save::doParse()
{
	parseHeader();
	parseFormIDArray();
	computeIngredientsRefIDs();
	parseChangeForms();
}

void Save::parseHeader()
{
	std::array<char, 13> magic;
	in >> magic;

	in.jump(8); // headerSize + saveNumber
	in >> m_header.saveNumber;

	m_header.playerName = in.readWString();
	in >> m_header.playerLevel;
	m_header.playerLocation = in.readWString();

	uint16_t stringSize;
	in >> stringSize; in.jump(stringSize); // gameDate
	in >> stringSize; in.jump(stringSize); // playerRaceEditorId

	in.jump(18); // playerSex + playerCurExp + playerLvlUpExp + filetime
	in >> m_header.ssWidth >> m_header.ssHeight;
	m_header.ssData.resize(3 * m_header.ssWidth * m_header.ssHeight);
	in >> m_header.ssData;

	uint8_t formVersion;
	in >> formVersion;

	uint32_t pluginInfoSize;
	in >> pluginInfoSize;

	uint8_t nbPlugins;
	in >> nbPlugins;
	for (int i = 0; i < nbPlugins; ++i)
		m_plugins.push_back(in.readWString());

	in >> m_formIDArrayCountOffset;
	in.jump(12);
	in >> m_changeFormsOffset;
	in.jump(16);
	in >> m_changeFormCount;
}

bool isPresent(const vector<unsigned char>& buffer, const vector<unsigned char>& value)
{
	return search(buffer.begin(), buffer.end(), value.begin(), value.end()) != buffer.end();
}

void Save::parseChangeForms()
{
	in.seekg(m_changeFormsOffset);

	for (uint32_t i = 0; i < m_changeFormCount; ++i)
	{
		ChangeForm form(in);
		form.formID = getFormID(form.refID);
		
		if (form.formType == 0) // Container
		{
			form.loadData();
		//	if ((formID & 0xFF000000) == 0x2B000000 && decompressedData.size() > 250)
		/*	if (isPresent(form.data, { 0x43, 0xAD, 0x5B }) && isPresent(form.data, { 0x43, 0xAD, 0x60 }))
			{
				ostringstream ss;
				ss << "data/0x" << hex << uppercase << setfill('0') << setw(8) << form.formID << ".data";
				ofstream out(ss.str(), ios::binary | ios::out);
				out.write(reinterpret_cast<char*>(form.data.data()), form.data.size());
			}*/
		}
		else if (form.formType == 1) // Actor
		{
			if (form.formID != 0x14) // Only parse player
			{
				form.ignore();
				continue;
			}

			form.loadData();
			parsePlayer(form);
		}
		else if (form.formType == 16) // Known ingredients
		{
			form.loadData();
			if (form.data.size() == 4)
				parseKnownIngredient(form);
		}
		else
			form.ignore();
	}
}

void Save::parseFormIDArray()
{
	in.seekg(m_formIDArrayCountOffset);
	uint32_t count;
	in >> count;

	m_formIDArray.resize(count);
	in >> m_formIDArray;

//	ofstream out("data/formIDs.data", ios::binary);
//	out.write(reinterpret_cast<char*>(m_formIDArray.data()), count * 4);
}

void Save::parseKnownIngredient(const ChangeForm& form)
{
	uint8_t modID = form.formID >> 24;
	uint32_t ingID = form.formID & 0x00FFFFFF;

	KnownIngredient ing;
	ing.first.mod = m_plugins[modID];
	ing.first.id = ingID;

	for (int j = 0; j < 4; ++j)
		ing.second[j] = ((form.data[0] & (1 << j)) != 0);

	m_knownIngredients.push_back(ing);
}

void Save::parsePlayer(const ChangeForm& form)
{
/*	ostringstream ss;
	ss << "data/0x" << hex << uppercase << setfill('0') << setw(8) << form.formID << ".data";
	ofstream out(ss.str(), ios::binary | ios::out);
	out.write(reinterpret_cast<const char*>(form.data.data()), form.data.size());
*/

	// Naive approach: look for each possible ingredient
	//  If found, the next int32 is its count
	auto db = begin(form.data), de = end(form.data);
	for (int i = 0, nb = m_ingredientsRefID.size(); i < nb; ++i)
	{
		const auto& refID = m_ingredientsRefID[i];
		auto it = search(db, de, begin(refID), end(refID));
		if (it != de)
		{
			int32_t nb = *reinterpret_cast<const int32_t*>(form.data.data() + (it - db) + 3);
			m_inventory.emplace_back(m_possibleIngredients[i], nb);
		}
	}
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

template <class C, class V>
int indexOf(const C& container, const V& value)
{
	auto b = begin(container), e = end(container);
	auto it = find(b, e, value);
	if (it == e)
		return -1;
	return it - b;
}

Save::RefID Save::getRefID(uint32_t formID)
{
	RefID ref;
	ref.fill(0);

	uint32_t id = formID & 0x00FFFFFF;
	uint8_t modId = formID >> 24;
	if (!modId) // Skyrim.esm
	{
		// Convert to a refID
		ref[0] = (id >> 16) & 0xFF | 0x40;
		ref[1] = (id >> 8) & 0xFF;
		ref[2] = id & 0xFF;
		return ref;
	}
	else if (modId == 0xFF) // Created
	{
		// Convert to a refID
		ref[0] = (id >> 16) & 0xFF | 0x80;
		ref[1] = (id >> 8) & 0xFF;
		ref[2] = id & 0xFF;
		return ref;
	}
	else
	{
		// Look for the FormID in the array
		id = indexOf(m_formIDArray, formID);
		if (id == -1)
			return ref;
		++id; // No value at 0

		// Convert to a refID
		ref[0] = (id >> 16) & 0xFF;
		ref[1] = (id >> 8) & 0xFF;
		ref[2] = id & 0xFF;
		return ref;
	}
}

void Save::computeIngredientsRefIDs()
{
	// We also modify the ingredients list, so that the 2 lists do correspond
	Ingredients ingredients;
	ingredients.swap(m_possibleIngredients);

	m_ingredientsRefID.clear();
	for (const auto& posIng : ingredients)
	{
		// Get the id of the mod
		int modId = indexOf(m_plugins, posIng.mod);
		if (modId == -1)
			continue;

		// Compute the formID
		int32_t formID = (modId << 24) | (posIng.id & 0x00FFFFFF);

		auto refID = getRefID(formID);
		if (!refID[0] && !refID[1] && !refID[2])
			continue;

		m_ingredientsRefID.push_back(refID);
		m_possibleIngredients.push_back(posIng);
	}
}

const Save::Header& Save::header() const
{
	return m_header;
}

const Save::KnownIngredients& Save::knownIngredients() const
{
	return m_knownIngredients;
}

void Save::setPossibleIngredients(const Ingredients& ingredients)
{
	m_possibleIngredients = ingredients;
}

const Save::Inventory& Save::inventory() const
{
	return m_inventory;
}

//****************************************************************************//

Save::ChangeForm::ChangeForm(Parser& parser)
	: in(parser)
{
	in >> refID >> changeFlags;

	uint8_t type, lengthSize, version;
	in >> type >> version;
	formType = type & 0x3F;
	lengthSize = type >> 6;

	if (version != 74)
	{
		cerr << "Error in parsing change forms: wrong version" << endl;
		return;
	}

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
}

void Save::ChangeForm::ignore()
{
	in.jump(length1);
}

void Save::ChangeForm::loadData()
{
	if (length1)
	{
		if (length2)
		{
			std::vector<unsigned char> compressedData;
			compressedData.resize(length1);
			data.resize(length2);
			in >> compressedData;

			uLongf decSize = length2;
			uncompress(&data[0], &decSize, compressedData.data(), length1);
		}
		else
		{
			data.resize(length1);
			in >> data;
		}
	}
}
