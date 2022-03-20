#include "FileUtility.h"
#include <fstream>

namespace Utility
{
	ByteArray NullFile = std::make_shared<std::vector<std::byte>>(std::vector<std::byte>());

	ByteArray LoadFileSync(const std::wstring& filename)
	{
		//struct _stat64 buf;

		//// Get data associated with filename: 
		//int result = _wstat64(filename.c_str(), &buf);
		//if (result != 0)
		//{
		//	return NullFile;
		//}

		std::ifstream file;

		file.open(filename, std::ios::in | std::ios::binary);
		if (!file.is_open())
		{
			return NullFile;
		}

		file.seekg(0, std::ios::end);
		std::streampos size = file.tellg();
		ByteArray byteArray = std::make_shared<std::vector<std::byte>>(std::vector<std::byte>(size));
		file.seekg(0, std::ios::beg);
		file.read((char*)byteArray->data(), size);
		file.close();

		return byteArray;
	}
}