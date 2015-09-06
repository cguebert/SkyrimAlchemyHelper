#pragma once

#include <parser/parser.h>

class BSAFile
{
public:
	void load(const std::string& fileName);
	std::string extract(const std::string& fileName);

protected:
	std::string extractFile(int64_t dir, int64_t file);

	Parser in;

	int32_t m_flags, m_filesNameLen;
	bool m_archiveCompressed;

	struct FolderRecord
	{
		int64_t hash;
		int32_t count, offset;
	};
	std::vector<FolderRecord> m_folders;

	struct FileRecord
	{
		int64_t hash;
		int32_t size, offset;
	};
};

