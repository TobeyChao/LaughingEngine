#pragma once

#include "PCH.h"

namespace Utility
{
#ifdef _CONSOLE
	inline void Print(const char* msg) { printf("%s", msg); }
	inline void Print(const wchar_t* msg) { wprintf(L"%ws", msg); }
#else
	inline void Print(const char* msg) { OutputDebugStringA(msg); }
	inline void Print(const wchar_t* msg) { OutputDebugString(msg); }
#endif

	inline void Printf(const char* format, ...)
	{
		char buffer[256];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, 256, format, ap);
		va_end(ap);
		Print(buffer);
	}

	inline void Printf(const wchar_t* format, ...)
	{
		wchar_t buffer[256];
		va_list ap;
		va_start(ap, format);
		vswprintf(buffer, 256, format, ap);
		va_end(ap);
		Print(buffer);
	}

	std::wstring UTF8ToWideString(const std::string& str);
	std::string WideStringToUTF8(const std::wstring& wstr);
	std::string ToLower(const std::string& str);
	std::wstring ToLower(const std::wstring& str);
	std::string GetBasePath(const std::string& str);
	std::wstring GetBasePath(const std::wstring& str);
	std::string RemoveBasePath(const std::string& str);
	std::wstring RemoveBasePath(const std::wstring& str);
	std::string GetFileExtension(const std::string& str);
	std::wstring GetFileExtension(const std::wstring& str);
	std::string RemoveExtension(const std::string& str);
	std::wstring RemoveExtension(const std::wstring& str);

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = Utility::UTF8ToWideString(__FILE__);                       \
    if(FAILED(hr__)) { throw Utility::DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif // !ThrowIfFail

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif // !ReleaseCom

	class DxException
	{
	public:
		DxException() = default;
		DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber)
			:
			ErrorCode(hr),
			FunctionName(functionName),
			Filename(filename),
			LineNumber(lineNumber)
		{
		}

		std::wstring ToString() const
		{
			// Get the string description of the error code.
			_com_error err(ErrorCode);
			std::wstring msg = err.ErrorMessage();

			return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
		}

		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		std::wstring Filename;
		int LineNumber = -1;
	};
}