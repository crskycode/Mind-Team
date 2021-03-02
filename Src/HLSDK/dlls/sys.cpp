#include "extdll.h"

void Sys_Error(const char *error, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, error);
	vsnprintf(string, sizeof(string), error, argptr);
	va_end(argptr);

	HWND hwnd = GetActiveWindow();
	MessageBoxA(hwnd, string, "FATAL ERROR", MB_ICONERROR | MB_TOPMOST | MB_OK);

	exit(-1);
}