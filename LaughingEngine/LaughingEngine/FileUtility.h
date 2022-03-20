#pragma once

#include <memory>
#include <vector>
#include <string>

namespace Utility
{
	typedef std::shared_ptr<std::vector<std::byte>> ByteArray;

	extern ByteArray NullFile;

	ByteArray LoadFileSync(const std::wstring& filename);
}