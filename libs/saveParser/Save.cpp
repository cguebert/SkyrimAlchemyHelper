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

void Save::parseChangeForms()
{
	m_containers.clear();
	in.seekg(m_changeFormsOffset);

	for (uint32_t i = 0; i < m_changeFormCount; ++i)
	{
		ChangeForm form(in);
		form.formID = getFormID(form.refID);
		
		if (form.formType == 0) // Container
		{
		//	form.ignore();
			form.loadData();
			parseContainer(form);
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
	out.write(reinterpret_cast<const char*>(&form.changeFlags), 4);
	out.write(reinterpret_cast<const char*>(form.data.data()), form.data.size());
*/
	m_inventory.clear();
	searchForIngredients(form, m_inventory);
}

void Save::parseContainer(const ChangeForm& form)
{
	Inventory container;
	searchForIngredients(form, container);

//	if (container.size() > 10)
//		m_containers.push_back(container);
}

void Save::searchForIngredients(const ChangeForm& form, Inventory& inventory)
{
	// Naive approach: look for each possible ingredient
	//  If found, the next int32 is its count
/*	auto db = begin(form.data), de = end(form.data);
	for (int i = 0, nb = m_ingredientsRefID.size(); i < nb; ++i)
	{
		const auto& refID = m_ingredientsRefID[i];
		auto it = search(db, de, begin(refID), end(refID));
		if (it != de)
		{
			int32_t nb = *reinterpret_cast<const int32_t*>(form.data.data() + (it - db) + 3);
			inventory.emplace_back(i, nb);
		}
	}
	*/
	// Using our search helper
	int pos = 0, refId = 0, prevRefId = 0;
	while ((refId = m_searchHelper.search(form.data, pos)) != -1)
	{
		pos += 3;

		// Observation : if we give the ingredients sorted by name, the refId in the save will only increment
		if (refId < prevRefId)
			break;

		int32_t nb = *reinterpret_cast<const int32_t*>(form.data.data() + pos);
		inventory.emplace_back(refId, nb);

		prevRefId = refId;
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
	m_ingredientsRefID.clear();
	for (const auto& posIng : m_possibleIngredients)
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
		m_listedIngredients.push_back(posIng);
	}

	m_searchHelper.setup(m_ingredientsRefID);
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
	if (length1)
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

//****************************************************************************//

template <class C, class V>
void addUnique(C& container, const V& value)
{
	if (find(begin(container), end(container), value) != end(container))
		return;
	container.push_back(value);
}

void Save::SearchHelper::setup(const RefIDs& refIDs)
{
	// Prepare the relations between the levels
	vector<vector<int8_t>> l1In(256), l2In(256);
	for (const auto& refID : refIDs)
	{
		addUnique(l1In[refID[1]], refID[0]);
		addUnique(l2In[refID[2]], refID[1]);
	}
	for (int i = 0; i < 256; ++i)
	{
		sort(begin(l1In[i]), end(l1In[i]));
		sort(begin(l2In[i]), end(l2In[i]));
	}

	// Size necessary for data
	int dataSize = 1;
	for (int i = 0; i < 256; ++i)
	{
		if (!l1In[i].empty()) dataSize += 1 + l1In[i].size() * 2;
		if (!l2In[i].empty()) dataSize += 1 + l2In[i].size() * 2;
	}

	memset(levels, 0, 2 * 3 * 256);
	data.clear();
	data.resize(dataSize, 0);

	// Compute the offsets in the data of each value at each level
	int16_t pos = 1; // We start at 1 (0 is always an invalid index)
	for (int i = 0; i < 256; ++i) // Second level offsets
	{
		if (l1In[i].empty())
			continue;
		int16_t nb = l1In[i].size();
		levels[1][i] = pos;
		data[pos] = nb;
		pos += 1 + nb * 2;
	}

	for (int i = 0; i < 256; ++i) // Third level offsets
	{
		if (l2In[i].empty())
			continue;
		int16_t nb = l2In[i].size();
		levels[2][i] = pos;
		data[pos] = nb;
		pos += 1 + nb * 2;
	}

	// First level is easy, just say if there is at least one RefID beginning with this value or not
	for (const auto& refID : refIDs)
		levels[0][refID[0]] = 1;

	// For each RefID, build our way backward and add the necessary information in data
	int16_t maxId = 0;
	for (int itRef = 0, nbRefs = refIDs.size(); itRef < nbRefs; ++itRef)
	{
		const auto& refID = refIDs[itRef];
		uint16_t id = 0, r0 = refID[0], r1 = refID[1], r2 = refID[2];

		// Write into data for the second level at the offset for refID[1]
		const uint16_t l1 = levels[1][r1];
		uint16_t* p = &data[l1];
		uint16_t nb = *p++;
		for (uint16_t i = 0; i < nb; ++i) // Look for the id corresponding to refID[0] & refID[1]
		{
			if (*(p + 1) && *p != r0) // Has an id but value is different
			{
				p += 2;
				continue;
			}
		
			id = *(p + 1);
			if (!id) // Add a new id if necessary
			{
				*p++ = r0;
				*p = id = ++maxId;
			}
			break;
		}

		// Write into data for the third level at the offset for refID[1]
		const uint16_t l2 = levels[2][r2];
		p = &data[l2];
		nb = *p++;
		for (uint16_t i = 0; i < nb; ++i) // Look for the id we just got
		{
			if (*p && *p != id)
			{
				p += 2;
				continue;
			}
			
			*p++ = id;
			*p = itRef; // Write the value that we want to return from the search (index into refIDs)
			break;
		}
	}
}

int Save::SearchHelper::search(const Buffer& buffer, int& pos)
{
	int bufSize = buffer.size() - 2;
	for (; pos < bufSize; ++pos)
	{
		// First verify that each level has something for these bytes
		const uint8_t d0 = buffer[pos];
		if (!levels[0][d0])
			continue;

		const uint8_t d1 = buffer[pos + 1];
		const uint8_t d2 = buffer[pos + 2];
		const uint16_t l1 = levels[1][d1], l2 = levels[2][d2];
		if (!l1 || !l2)
			continue;

		// Obtain the id for the first 2 bytes
		uint16_t* p = &data[l1];
		uint16_t nb = *p++, id = 0;
		for (uint16_t i = 0; i < nb; ++i)
		{
			if (*p++ == d0)
			{
				id = *p;
				break;
			}
		}

		if (!id)
			continue;

		// Does the whole 3 bytes have a corresponding value ?
		p = &data[l2];
		nb = *p++;
		for (uint16_t i = 0; i < nb; ++i)
		{
			if (*p++ == id)
				return *p;
			else
				++p;
		}
	}

	return -1;
}