#include <algorithm>
#include <sstream>
#include <iostream>

#include "BSAFile.h"

namespace modParser
{

using namespace std;

const int32_t BSA_ARCHIVE_COMPRESSED = 1 << 2;
const int32_t BSA_FILE_COMPRESSED = 1 << 30;

string convertFileName(const string& fileName)
{
	auto pos = fileName.find_last_of(".");
	if (pos != string::npos)
		return fileName.substr(0, pos) + ".bsa";
	else
		return fileName + ".bsa";
}

void BSAFile::load(const string& fileName)
{
	auto bsaPath = convertFileName(fileName);

	ifstream stream(bsaPath, ios::binary | ios::in);
	if (!stream.is_open())
	{
		cerr << "Cannot open " << bsaPath << endl;
		return;
	}

	in.setStream(std::move(stream));

	in.jump(12);

	uint32_t nbFolders, nbFiles, foldersNameLen;
	in >> m_flags >> nbFolders >> nbFiles >> foldersNameLen >> m_filesNameLen;

	m_archiveCompressed = (m_flags & BSA_ARCHIVE_COMPRESSED) != 0;

	in.jump(4);

	m_folders.resize(nbFolders);
	in >> m_folders;
}

uint64_t stringHash(string s)
{
	uint32_t hash = 0;
	for (auto c : s)
		hash = hash * 0x1003F + c;
	return hash;
}

uint64_t fileExtHash(string file, string ext)
{
	uint64_t hash1 = 0, hash2 = 0, hash3 = 0;
	auto len = file.length();

	if (len > 0)
		hash1 = file[len - 1] + (len > 2 ? (file[len - 2] << 8) : 0) + (len << 16) + (file[0] << 24);

	if (len > 3)
		hash2 = stringHash(file.substr(1, len - 3));

	if (ext.length() > 0)
	{
		hash3 = stringHash(ext);

		if		(ext == ".kf")	hash1 |= 0x80;
		else if (ext == ".nif") hash1 |= 0x8000;
		else if (ext == ".dds") hash1 |= 0x8080;
		else if (ext == ".wav") hash1 |= 0x80000000;
	}

	hash2 = (hash2 + hash3) & 0xFFFFFFFF;
	return (hash2 << 32) + hash1;
}

uint64_t pathHash(string path)
{
	transform(path.begin(), path.end(), path.begin(), ::tolower);
	replace(path.begin(), path.end(), '/', '\\');

	string file, ext;
	auto pos = path.find_last_of(".");
	if (pos != string::npos)
	{
		ext = path.substr(pos);
		file = path.substr(0, pos);
	}
	else
		file = path;

	return fileExtHash(file, ext);
}

template <typename T>
T swap_endian(T u)
{
	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;

	source.u = u;

	for (size_t k = 0; k < sizeof(T); k++)
		dest.u8[k] = source.u8[sizeof(T) - k - 1];

	return dest.u;
}

string BSAFile::extract(const string& path)
{
	string dir, file;
	auto pos = path.find_last_of("/\\");
	if (pos != string::npos)
	{
		dir = path.substr(0, pos);
		file = path.substr(pos + 1);
	}
	else
		file = path;

	auto dirHash = pathHash(dir);
	auto fileHash = pathHash(file);

	return extractFile(dirHash, fileHash);
}

string BSAFile::extractFile(int64_t dir, int64_t file)
{
	auto dirIt = find_if(m_folders.begin(), m_folders.end(), [dir](const FolderRecord& f){
		return f.hash == dir;
	});
	if (dirIt == m_folders.end())
		return "";

	auto offset = dirIt->offset - m_filesNameLen;
	in.seekg(offset);

	uint8_t nameSize;
	in >> nameSize;
	in.jump(nameSize);

	std::vector<FileRecord> fileRecords(dirIt->count);
	in >> fileRecords;

	auto fileIt = find_if(fileRecords.begin(), fileRecords.end(), [file](const FileRecord& f){
		return f.hash == file;
	});

	if (fileIt == fileRecords.end())
		return "";

	in.seekg(fileIt->offset);

	auto size = fileIt->size;
	bool fileCompressionToggle = (size & BSA_FILE_COMPRESSED) != 0;
	if ((m_archiveCompressed && !fileCompressionToggle) || (!m_archiveCompressed && fileCompressionToggle))
	{
		cerr << "File is compressed, not (yet) implemented !" << endl;
		return "";
	}
	
	string result;
	result.resize(size);
	in.stream().read(&result[0], size);
	return result;
}

} // namespace modParser
