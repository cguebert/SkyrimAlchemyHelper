#pragma once

#include <fstream>
#include <array>
#include <vector>

class Parser
{
public:
	void setStream(std::ifstream&& stream) { m_in = std::move(stream); }

	std::ifstream& stream() { return m_in; }
	std::streamoff tellg() { return m_in.tellg(); }
	void seekg(std::streamoff pos) { m_in.seekg(pos); }
	bool eof() { return m_in.eof(); }

	template <class T, int N>
	Parser& operator>>(std::array<T, N>& a)
	{
		m_in.read(reinterpret_cast<char*>(a.data()), N * sizeof(T));
		return *this;
	}

	template <class T>
	Parser& operator>>(std::vector<T>& v)
	{
		if(!v.empty())
			m_in.read(reinterpret_cast<char*>(v.data()), v.size() * sizeof(T));
		return *this;
	}

	template <class T>
	Parser& operator>>(T& t)
	{
		m_in.read(reinterpret_cast<char*>(&t), sizeof(T));
		return *this;
	}

protected:
	std::ifstream m_in;
};

