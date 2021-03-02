#include <windows.h>
#include <stdio.h>

#define LAUNCHER_PROFILE ".\\Launcher.ini"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	char szCommandLine[ 256 ];

	DWORD iScreenWidth = GetPrivateProfileInt("Settings", "ScreenWidth", 800, LAUNCHER_PROFILE);
	DWORD iScreenHeight = GetPrivateProfileInt("Settings", "ScreenHeight", 600, LAUNCHER_PROFILE);
	DWORD iScreenWindowed = GetPrivateProfileInt("Settings", "ScreenWindowed", 1, LAUNCHER_PROFILE);

	char szExtraCommand[ 256 ];

	GetPrivateProfileString("Settings", "Command", "", szExtraCommand, sizeof(szExtraCommand), LAUNCHER_PROFILE);

	sprintf(szCommandLine, "-game metric -width %d -height %d %s %s", iScreenWidth, iScreenHeight, iScreenWindowed ? "-windowed" : "-fullscreen", szExtraCommand);

	ShellExecute(NULL, "open", "MetaHook.exe", szCommandLine, NULL, SW_SHOWNORMAL);
	return 0;
}