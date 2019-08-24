#include "pch.h"
#include "Diagnisis.h"

namespace mapi {

void OnFatalError(const char* message, const char* file, int line, const char* func)
{
#ifdef DEBUG
	_CrtDbgBreak();
#endif // DEBUG

	terminate();
}

void mapi::ProcessFFmpegError(const char* api, int code, Error& err, const char* file, int line, const char* func)
{
	err = { MAPIERR_FAIL, code };
}

void ProcessComError(const char* api, int code, Error& err, const char* file, int line, const char* func)
{
	err = { MAPIERR_COM, code };
}

void ProcessSystemError(const char* api, uint32_t code, Error& err, const char* file, int line, const char* func)
{
	err = { MAPIERR_SYSTERM, (int)code };
}

} // namespace mapi
