#include "metahook.h"

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

extern "C"
{
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}