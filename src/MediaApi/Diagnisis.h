#pragma once

#include "Types.h"

namespace mapi {

void ProcessFFmpegError(const char* api, int code, Error& err, const char* file, int line, const char* func);

inline bool CheckFFmpegResult(const char* api, int result, Error& err, const char* file, int line, const char* func)
{
	if (result >= 0)
	{
		return true;
	}

	ProcessFFmpegError(api, result, err, file, line, func);
	return true;
}

} // namespace mapi

#define CHECK_FFMPEG_RESULT(api, result, err)	mapi::CheckFFmpegResult((api), (result), (err), __FILE__, __LINE__, __FUNCTION__)
