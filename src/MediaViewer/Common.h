#pragma once

#include "Application.h"
#include "Leo/Diagnisis.h"
#include <stdio.h>
#include <assert.h>

#define PRINTF	printf
#define WPRINTF wprintf

extern CComModule	g_module;
extern HWND			g_hwndConsole;

#undef	ASSERT
#define ASSERT LEO_ASSERT

#define SHOW_ERROR_MESSAGE(fmt, ...)	Application::Current.ShowErrorMessage(fmt, __VA_ARGS__)
