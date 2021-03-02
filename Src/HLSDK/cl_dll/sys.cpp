#include "cl_dll.h"

void Sys_Error(const char *fmt, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string) - 1, fmt, argptr);
	va_end(argptr);

	HWND hwnd = GetActiveWindow();
	MessageBoxA(hwnd, string, "FATAL ERROR", MB_ICONERROR | MB_TOPMOST | MB_OK);

	exit(-1);
}