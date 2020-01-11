#pragma once

#include "assert.h"
#include <exception>

namespace Leo {
namespace Diag {

inline
__declspec(noreturn)
void OnFatalError(const char* message, const char* file, int line, const char* func)
{
#ifndef NDEBUG
	_CrtDbgBreak();
#endif // DEBUG
	terminate();
}

} // end of namespace Diag
} // end of namespace Leo

#ifndef _LEO_ON_FATAL_ERROR
#define _LEO_ON_FATAL_ERROR(msg)	do { Leo::Diag::OnFatalError((msg), __FILE__, __LINE__, __FUNCTION__); } while(0)
#endif // !_LEO_ON_FATAL_ERROR

#ifndef _LEO_ENSURE
#define _LEO_ENSURE(exp)			do { if (!(exp)) { _LEO_ON_FATAL_ERROR(#exp); } } while(0)
#endif // !_LEO_ENSURE

#ifndef _LEO_ASSERT
#ifdef NDEBUG
#	define _LEO_ASSERT(exp)	(exp)
#else
#	define _LEO_ASSERT(exp)	assert(exp)
#endif // NDEBUG
#endif // !_LEO_ASSERT

#define LEO_ON_FATAL_ERROR	_LEO_ON_FATAL_ERROR
#define LEO_ASSERT			_LEO_ASSERT
#define LEO_ENSURE			_LEO_ENSURE
#define LEO_REQUIRE			_LEO_ENSURE
