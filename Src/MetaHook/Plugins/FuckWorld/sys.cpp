#include <metahook.h>

void Sys_Error(const char *fmt, ...)
{
	static char output[1024];
	va_list ptr;

	va_start(ptr, fmt);
	vsnprintf(output, sizeof(output) - 1, fmt, ptr);
	va_end(ptr);

	HWND hwnd = GetActiveWindow();
	MessageBoxA(hwnd, output, "FATAL ERROR", MB_ICONERROR | MB_TOPMOST);

	_exit(-1);
}