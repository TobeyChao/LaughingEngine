#include "PCH.h"
#include "Utility.h"
#include <locale>

std::wstring Utility::UTF8ToWideString(const std::string& str)
{
	wchar_t wstr[MAX_PATH];
	if (!MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1, wstr, MAX_PATH))
		wstr[0] = L'\0';
	return wstr;
}

std::string Utility::WideStringToUTF8(const std::wstring& wstr)
{
	char str[MAX_PATH];
	if (!WideCharToMultiByte(CP_ACP, MB_PRECOMPOSED, wstr.c_str(), -1, str, MAX_PATH, nullptr, nullptr))
		str[0] = L'\0';
	return str;
}

std::string Utility::ToLower(const std::string& str)
{
	std::string lower_case = str;
	std::locale loc;
	for (char& s : lower_case)
		s = std::tolower(s, loc);
	return lower_case;
}

std::wstring Utility::ToLower(const std::wstring& str)
{
	std::wstring lower_case = str;
	std::locale loc;
	for (wchar_t& s : lower_case)
		s = std::tolower(s, loc);
	return lower_case;
}

std::string Utility::GetBasePath(const std::string& filePath)
{
	size_t lastSlash;
	if ((lastSlash = filePath.rfind('/')) != std::string::npos)
		return filePath.substr(0, lastSlash + 1);
	else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
		return filePath.substr(0, lastSlash + 1);
	else
		return "";
}

std::wstring Utility::GetBasePath(const std::wstring& filePath)
{
	size_t lastSlash;
	if ((lastSlash = filePath.rfind(L'/')) != std::wstring::npos)
		return filePath.substr(0, lastSlash + 1);
	else if ((lastSlash = filePath.rfind(L'\\')) != std::wstring::npos)
		return filePath.substr(0, lastSlash + 1);
	else
		return L"";
}

std::string Utility::RemoveBasePath(const std::string& filePath)
{
	size_t lastSlash;
	if ((lastSlash = filePath.rfind('/')) != std::string::npos)
		return filePath.substr(lastSlash + 1, std::string::npos);
	else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
		return filePath.substr(lastSlash + 1, std::string::npos);
	else
		return filePath;
}

std::wstring Utility::RemoveBasePath(const std::wstring& filePath)
{
	size_t lastSlash;
	if ((lastSlash = filePath.rfind(L'/')) != std::string::npos)
		return filePath.substr(lastSlash + 1, std::string::npos);
	else if ((lastSlash = filePath.rfind(L'\\')) != std::string::npos)
		return filePath.substr(lastSlash + 1, std::string::npos);
	else
		return filePath;
}

std::string Utility::GetFileExtension(const std::string& filePath)
{
	std::string fileName = RemoveBasePath(filePath);
	size_t extOffset = fileName.rfind('.');
	if (extOffset == std::wstring::npos)
		return "";

	return fileName.substr(extOffset + 1);
}

std::wstring Utility::GetFileExtension(const std::wstring& filePath)
{
	std::wstring fileName = RemoveBasePath(filePath);
	size_t extOffset = fileName.rfind(L'.');
	if (extOffset == std::wstring::npos)
		return L"";

	return fileName.substr(extOffset + 1);
}

std::string Utility::RemoveExtension(const std::string& filePath)
{
	return filePath.substr(0, filePath.rfind("."));
}

std::wstring Utility::RemoveExtension(const std::wstring& filePath)
{
	return filePath.substr(0, filePath.rfind(L"."));
}
