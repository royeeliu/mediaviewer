#include "pch.h"
#include "Diagnisis.h"

namespace mapi {

void mapi::ProcessFFmpegError(const char* api, int code, Error& err, const char* file, int line, const char* func)
{
	err = { MAPIERR_FAIL, code };
}

} // namespace mapi
