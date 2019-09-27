#pragma once

#include "Types.h"
#include <assert.h>

namespace mapi {

void OnFatalError(const char* message, const char* file, int line, const char* func);
void SetErrorCode(Error& err, ERROR_CODE code, int detail, const char* message, const char* file, int line, const char* func);
void ProcessFFmpegError(const char* api, int code, Error& err, const char* file, int line, const char* func);
void ProcessComError(const char* api, int code, Error& err, const char* file, int line, const char* func);
void ProcessSystemError(const char* api, uint32_t code, Error& err, const char* file, int line, const char* func);

inline bool CheckFFmpegResult(const char* api, int result, Error& err, const char* file, int line, const char* func)
{
	if (result >= 0)
	{
		return true;
	}

	ProcessFFmpegError(api, result, err, file, line, func);
	return false;
}

inline bool CheckComResult(const char* api, int hresult, Error& err, const char* file, int line, const char* func)
{
	if (hresult >= 0)
	{
		return true;
	}

	ProcessComError(api, hresult, err, file, line, func);
	return false;
}

} // namespace mapi

#define ASSERT assert
#define _ENSURE(expression) do { if (!(expression)) mapi::OnFatalError((#expression), __FILE__, __LINE__, __FUNCTION__); } while(0) 
#define ENSURE _ENSURE

#define PROCESS_SYSTEM_ERROR(api, code, err)	mapi::ProcessSystemError((api), (code), (err), __FILE__, __LINE__, __FUNCTION__)
#define CHECK_FFMPEG_RESULT(api, result, err)	mapi::CheckFFmpegResult((api), (result), (err), __FILE__, __LINE__, __FUNCTION__)
#define CHECK_COM_RESULT(api, result, err)		mapi::CheckComResult((api), (result), (err), __FILE__, __LINE__, __FUNCTION__)

#define VERIFY_CURRENT_THREAD_ID_IS(id)			ASSERT(::GetCurrentThreadId() == (id))
