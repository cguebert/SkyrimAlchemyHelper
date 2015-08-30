#include <algorithm>
#include <sstream>
#include <iostream>

#include "BSAFile.h"

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

// From the code of BSAOpt
uint32_t GenOBHashStr(string s)
{
	uint32_t hash = 0;

	for (size_t i = 0, len = s.length(); i < len; ++i) 
	{
		hash *= 0x1003F;
		hash += (unsigned char)s[i];
	}

	return hash;
}

uint64_t GenOBHashPair(string fle, string ext) 
{
	uint64_t hash = 0;
	auto len = fle.length();

	if (len > 0) 
	{
		hash = (uint64_t)(
			(((unsigned char)fle[len - 1]) * 0x1) +
			((len > 2 ? (unsigned char)fle[len - 2] : (unsigned char)0) * 0x100) +
			(len * 0x10000) +
			(((unsigned char)fle[0]) * 0x1000000)
			);

		if (len > 3)
			hash += (uint64_t)(GenOBHashStr(fle.substr(1, len - 3)) * 0x100000000);
	}

	if (ext.length() > 0) 
	{
		hash += (uint64_t)(GenOBHashStr(ext) * 0x100000000LL);

		unsigned char i = 0;
		if (ext == ".nif") i = 1;
		if (ext == ".kf")  i = 2;
		if (ext == ".dds") i = 3;
		if (ext == ".wav") i = 4;

		if (i != 0) 
		{
			unsigned char a = (unsigned char)(((i & 0xfc) << 5) + (unsigned char)((hash & 0xff000000) >> 24));
			unsigned char b = (unsigned char)(((i & 0xfe) << 6) + (unsigned char)(hash & 0x000000ff));
			unsigned char c = (unsigned char)((i << 7) + (unsigned char)((hash & 0x0000ff00) >> 8));

			hash -= hash & 0xFF00FFFF;
			hash += (unsigned int)((a << 24) + b + (c << 8));
		}
	}

	return hash;
}

uint64_t GenOBHash(string path) 
{
	transform(path.begin(), path.end(), path.begin(), ::tolower);
	replace(path.begin(), path.end(), '/', '\\');

	string file;
	string ext;

	auto pos = path.find_last_of(".");
	if (pos != string::npos) 
	{
		ext = path.substr(pos);
		file = path.substr(0, pos);
	}
	else {
		ext = "";
		file = path;
	}

	return GenOBHashPair(file, ext);
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

	auto dirHash = GenOBHash(dir);
	auto fileHash = GenOBHash(file);

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
