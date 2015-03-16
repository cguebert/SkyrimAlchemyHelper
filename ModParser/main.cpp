#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <array>
#include <vector>

#include "Parser.h"

using namespace std;

using Type = array<char, 4>;

Parser in;
bool parsingIngredient = false;

struct Ingredient
{
	std::string name;
	uint32_t effectId[4], duration[4];
	float magnitude[4];
} ingredient;

template <class T, int N>
void print(const std::array<T, N>& array)
{
	for(const auto& a : array)
		cout << a;
	cout << endl;
}

template <class T, int N>
void printHex(const std::array<T, N>& array)
{
	for(const auto& a : array)
		cout << uppercase << hex << static_cast<int>(a);
	cout << endl;
}

bool isType(const Type& type, const std::string& name)
{
	if(name.size() != 4)
		return false;

	for(int i=0; i<4; ++i)
	{
		if(type[i] != name[i])
			return false;
	}

	return true;
}

void clearIngredient()
{
	ingredient.name.clear();
	for(int i=0; i<4; ++i)
	{
		ingredient.effectId[i] = 0;
		ingredient.magnitude[i] = 0;
		ingredient.duration[i] = 0;
	}
}

void parseGenericField()
{
	uint16_t dataSize;
	in >> dataSize;

	std::vector<uint8_t> data(dataSize);
	in >> data;
}

void parseName()
{
	uint16_t dataSize;
	in >> dataSize;

	std::string& name = ingredient.name;
	name.resize(dataSize);
	in.stream().read(&name[0], dataSize);
}

void parseEffectID()
{
	uint16_t dataSize;
	in >> dataSize;

	uint32_t id;
	in >> id;

	for(int i=0; i<4; ++i)
	{
		if(!ingredient.effectId[i])
		{
			ingredient.effectId[i] = id;
			break;
		}
	}
}

void parseEffectItem()
{
	uint16_t dataSize;
	in >> dataSize;

	uint32_t area;
	int i = 0;
	while(i<3 && ingredient.effectId[i+1])
		++i;
	in >> ingredient.magnitude[i] >> area
			>> ingredient.duration[i];
}

void parseField()
{
	Type type{};
	in >> type;
//	print(type);

	if(parsingIngredient)
	{
		if(isType(type, "FULL"))
			parseName();
		else if(isType(type, "EFID"))
			parseEffectID();
		else if(isType(type, "EFIT"))
			parseEffectItem();
		else
			parseGenericField();
	}
	else
		parseGenericField();
}

void parseGenericRecord()
{
	uint32_t dataSize, flags, id, revision;
	uint16_t version, unknown;
	in >> dataSize >> flags >> id >> revision >> version >> unknown;

	auto start = in.tellg();
	while(in.tellg() - start < dataSize)
		parseField();
}

void parseRecord();

void parseGroup()
{
	auto start = in.tellg() - 4;
	Type label;
	uint32_t groupSize, groupType;
	uint16_t stamp, unknown, version, unknown2;
	in >> groupSize >> label >> groupType >> stamp >>
			unknown >> version >> unknown2;

//	print(label);
	if(isType(label, "INGR"))
	{
		while(in.tellg() - start < groupSize)
			parseRecord();
	}
	else
		in.seekg(start + groupSize);
}

float round(float v, int d)
{
	float p = pow(10.0f, d);
	return round(v * p) / p;
}

void parseIngredient()
{
	parsingIngredient = true;
	clearIngredient();
	parseGenericRecord();

	cout << ingredient.name << endl;
	for(int i=0; i<4; ++i)
	{
		cout << hex << uppercase << ingredient.effectId[i];
		cout << "\t" << round(ingredient.magnitude[i], 6);
		cout << dec << "\t" << ingredient.duration[i];
		cout << endl;
	}
	parsingIngredient = true;
}

void parseRecord()
{
	Type type{};
	in >> type;
//	print(type);

	if(in.stream().eof())
		return;

	if(isType(type, "GRUP"))
		parseGroup();
	else if(isType(type, "INGR"))
		parseIngredient();
	else
		parseGenericRecord();
}

void parseMod(std::string fileName)
{
	ifstream stream;
	stream.open(fileName, ios::binary | ios::in);
	if(!stream.is_open())
	{
		cout << "Cannot open " << fileName << endl;
		return;
	}

	cout << "Parsing " << fileName << endl;
	in.setStream(std::move(stream));

	while(!in.stream().eof())
		parseRecord();
}

int main(int argc, char** argv)
{
	if(argc < 2)
		parseMod("data/test.esp");
	else
		parseMod(argv[1]);
	return 0;
}

