#include <metahook.h>
#include "plugins.h"

HWND (WINAPI *g_pfnCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = NULL;

WNDPROC g_lpPrevWndFunc;

LRESULT CALLBACK NewWndProcFunc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_NCHITTEST:
		{
			LRESULT lResult = CallWindowProc(g_lpPrevWndFunc, hWnd, Msg, wParam, lParam);

			if (lResult >= HTLEFT && lResult <= HTBORDER)
			{
				return HTNOWHERE;
			}

			break;
		}
	}

	return CallWindowProc(g_lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

HWND WINAPI NewCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (HIWORD(lpClassName) && !strcmp(lpClassName, "Valve001"))
	{
		if (dwStyle & WS_SYSMENU)
		{
			dwStyle |= WS_SIZEBOX;
		}

		if (g_pInterface->CommandLine->CheckParm("-noborder"))
		{
			dwStyle &= ~WS_CAPTION;
			dwStyle &= ~WS_SYSMENU;
			dwStyle &= ~WS_SIZEBOX;
		}

		HWND hWnd = g_pfnCreateWindowExA(dwExStyle, lpClassName, "Mind-Team", dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

		g_lpPrevWndFunc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)&NewWndProcFunc);

		return hWnd;
	}

	return g_pfnCreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

void WindowMode_InstallHook(void)
{
	InlineHook(&CreateWindowExA, &NewCreateWindowExA, (void *&)g_pfnCreateWindowExA);
}