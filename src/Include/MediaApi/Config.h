#pragma once

#include <inttypes.h>

#ifndef MEDIA_API_EXPORTS
#define MEDIA_API __declspec(dllimport)
#else
#define MEDIA_API __declspec(dllexport)
#endif // !MEDIAAPI_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus



#ifdef __cplusplus
}
#endif // _cplusplus


