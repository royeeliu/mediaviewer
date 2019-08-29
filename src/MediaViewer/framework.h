// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ 
#include <memory>
#include <functional>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

// boost
#include <boost/signals2.hpp>

// WTL
#include "atlapp.h"
#include "atldlgs.h"

// Leo
#include "Leo/libleo.h"
