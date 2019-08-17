#pragma once

#include "Types.h"

namespace mapi {

namespace details {

void ProcessFFmpegError(const char* api_name, int result, Error& err, const char* file, int line, const char* func);

} // End of namespace details

inline bool CheckFFmpegResult(const char* api_name, int result, Error& err, const char* file, int line, const char* func)
{
	if (result == 0)
	{
		return true;
	}

	details::ProcessFFmpegError(api_name, result, err, file, line, func);
	return false;
}

} // End of namespace mapi

#define CHECK_FFMPEG_RESULT(name, result, err)	mapi::CheckFFmpegResult(name, result, err, __FILE__, __LINE__, __FUNCTION__)
