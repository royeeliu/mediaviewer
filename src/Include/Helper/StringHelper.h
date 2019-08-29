#pragma once

#include <windows.h>
#include <string>

namespace StringHelper {

inline std::string WideToMultiStr(const wchar_t* wstr, int length, uint32_t codePage)
{
	std::string mstr;

	if (wstr != nullptr && (length > 0 || (length == -1 && wstr[0] != L'\0')))
	{
		int len = ::WideCharToMultiByte(codePage, 0, wstr, length, nullptr, 0, nullptr, nullptr);
		mstr.resize(len);
		::WideCharToMultiByte(codePage, 0, wstr, length, &mstr[0], len, nullptr, nullptr);
	}

	return mstr;
}

inline std::wstring MultiToWideStr(const char* mstr, int length, uint32_t codePage)
{
	std::wstring wstr;

	if (mstr != nullptr && (length > 0 || (length == -1 && mstr[0] != '\0')))
	{
		int len = ::MultiByteToWideChar(codePage, 0, mstr, length, nullptr, 0);
		wstr.resize(len);
		::MultiByteToWideChar(codePage, 0, mstr, length, &wstr[0], len);
	}

	return wstr;
}

inline std::string W2U8(const std::wstring& wstr)
{
	return WideToMultiStr(wstr.c_str(), (int)wstr.length(), CP_UTF8);
}

inline std::string W2U8(const wchar_t* wstr)
{
	return WideToMultiStr(wstr, -1, CP_UTF8);
}

inline std::string W2Ansi(const std::wstring& wstr)
{
	return WideToMultiStr(wstr.c_str(), (int)wstr.length(), CP_ACP);
}

inline std::string W2Ansi(const wchar_t* wstr)
{
	return WideToMultiStr(wstr, -1, CP_ACP);
}

inline std::wstring U8ToW(const char* str)
{
	return MultiToWideStr(str, -1, CP_UTF8);
}

inline std::wstring U8ToW(const std::string& str)
{
	return MultiToWideStr(str.c_str(), (int)str.length(), CP_UTF8);
}

inline std::wstring AnsiToW(const char* str)
{
	return MultiToWideStr(str, -1, CP_ACP);
}

inline std::wstring AnsiToW(const std::string& str)
{
	return MultiToWideStr(str.c_str(), (int)str.length(), CP_ACP);
}

}
