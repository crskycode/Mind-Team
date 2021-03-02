
#include <windows.h>
#include <ICommandLine.h>

bool CreateBlobProcess(ICommandLine *cmd)
{
	if (cmd->CheckParm("-blob"))
		return false;

	char name[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), name, _countof(name));

	STARTUPINFO si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi;

	cmd->AppendParm("-blob", NULL);

	if (!CreateProcess(name, (LPSTR)cmd->GetCmdLine(), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
		return true;

	LPVOID buf = VirtualAllocEx(pi.hProcess, (LPVOID)0x1900000, 0x2000000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!buf)
	{
		TerminateProcess(pi.hProcess, 1);
		return true;
	}

	ResumeThread(pi.hThread);
	//WaitForSingleObject(pi.hProcess, INFINITE);
	return true;
}