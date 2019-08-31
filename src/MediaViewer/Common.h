#pragma once

#include "Application.h"
#include <stdio.h>
#include <assert.h>

#define PRINTF	printf
#define WPRINTF wprintf 

#define ASSERT assert

extern CComModule	g_module;
extern HWND			g_hwndConsole;



#define SHOW_ERROR_MESSAGE(fmt, ...)	Application::Current.ShowErrorMessage(fmt, __VA_ARGS__)
