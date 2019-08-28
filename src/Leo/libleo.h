#pragma once

#ifndef LEO_FILE_LINE_HELP
#define LEO_FILE_LINE_HELP

#define _LEO_LINENUMBER_TO_STRING(x)	#x
#define LEO_LINE_NUMBER_TO_STRING(x)	_LEO_LINENUMBER_TO_STRING(x)
#define __LEO_LINE_NUMBER__				LEO_LINE_NUMBER_TO_STRING(__LINE__)

#define LEO_INCLUDE_FILE_LINE(s)		__FILE__ "(" __LEO_LINE_NUMBER__ ") : " s
#define LEO_INCLUDE_FILE_LINE_			__FILE__ "(" __LEO_LINE_NUMBER__ ") : "

#endif // LEO_FILE_LINE_HELP


//////////////////////////////////////////////////////////////////////////
// Helps linking with the right library 
#if defined(_MSC_VER) 
#	if _MSC_VER >= 1910 && _MSC_VER < 1920
#		define _LIB_MSVC_VERNUM		"141"
#	elif _MSC_VER >= 1920 && _MSC_VER < 1930
#		define _LIB_MSVC_VERNUM		"142"
#	else
#		pragma message(INCLUDE_FILE_LINE_"error: not surported")
#	endif
#endif

#if defined(_WIN64)
#	define  _LIB_PLATFORM			"_x64"
#else
#	define  _LIB_PLATFORM			""
#endif

#if defined(_MSC_VER)
#	ifdef _DEBUG
#		pragma comment(lib, "libleo_vc" _LIB_MSVC_VERNUM _LIB_PLATFORM "_d.lib")
#	else
#		pragma comment(lib, "libleo_vc" _LIB_MSVC_VERNUM _LIB_PLATFORM ".lib")
#	endif
#endif

#undef _LIB_MSVC_VERNUM
#undef _LIB_PLATFORM
