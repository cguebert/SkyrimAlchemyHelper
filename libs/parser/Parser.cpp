#include <algorithm>

#include "Parser.h"

namespace parser
{

using namespace std;

void Parser::jump(std::streamoff off)
{
	const int maxJump = 4096;
	static char dummy[4096];
	if (off <= maxJump)
		m_in->read(dummy, off); // Reading is most often faster than doing a seekg that can flush the caches
	else
		m_in->seekg(off, std::ios_base::cur);
}

uint32_t Parser::readVSVal()
{
	uint8_t byte1;
	read(byte1);
	uint8_t type = byte1 & 0x03;
	if (!type)
		return byte1 >> 2;
	else if (type == 1)
	{
		uint8_t byte2;
		read(byte2);
		return (byte1 | (byte2 << 8)) >> 2;
	}
	else if (type == 2)
	{
		uint8_t byte2, byte3, byte4;
		read(byte2);
		read(byte3);
		read(byte4);
		return (byte1 | (byte2 << 8) | (byte3 << 16) | (byte4 << 24)) >> 2;
	}

	return 0;
}

std::string Parser::readBString()
{
	uint8_t size;
	read(size);
	string text;
	text.resize(size);
	m_in->read(&text[0], size);
	return text;
}

std::string Parser::readBZString()
{
	uint8_t size;
	read(size);
	string text;
	text.resize(size);
	m_in->read(&text[0], size);
	m_in->seekg(1, ios::cur);
	return text;
}

std::string Parser::readWString()
{
	uint16_t size;
	read(size);
	string text;
	text.resize(size);
	m_in->read(&text[0], size);
	return text;
}

std::string Parser::readWZString()
{
	uint16_t size;
	read(size);
	string text;
	text.resize(size);
	m_in->read(&text[0], size);
	m_in->seekg(1, ios::cur);
	return text;
}

std::string Parser::readZString()
{
	vector<char> buf;

	auto pos = m_in->tellg();
	int size = 64, prev = 0;
	buf.resize(size);
	m_in->read(&buf.front(), size);

	while (find(buf.begin() + prev, buf.end(), 0) == buf.end())
	{
		prev = size;
		buf.resize(size * 2);
		m_in->read(&buf.front() + prev, size);
		size *= 2;
	}

	auto end = find(buf.begin(), buf.end(), 0);
	streamoff delta = end - buf.begin();
	m_in->seekg(pos + delta);
	return string(buf.begin(), end);
}

std::string Parser::readString(int size)
{
	string text;
	text.resize(size);
	m_in->read(&text[0], size);
	return text;
}

} // namespace parser
