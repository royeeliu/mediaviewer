#pragma once

#include "MediaApi.h"
#include "Helper/StringHelper.h"
#include <sstream>

namespace MediaApiHelper {

inline std::wstring FormatError(MAPI_Error const& err)
{
	std::wostringstream oss;
	oss << L"{" << err.code 
		<< L"." << err.detail
		;

	if (err.message != nullptr)
	{
		oss << L": " << StringHelper::U8ToW(err.message);
	}

	oss << L"}";

	return oss.str();
}

inline const char* MediaTypeName(MAPI_MediaType type)
{
	switch (type)
	{
	case MAPI_MediaType::Unknown:
		return "Unknown";
	case MAPI_MediaType::Video:
		return "Video";
	case MAPI_MediaType::Audio:
		return "Audio";
	default:
		_ASSERTE(false);
		return "(Invalid Value)";
	}
}

}

