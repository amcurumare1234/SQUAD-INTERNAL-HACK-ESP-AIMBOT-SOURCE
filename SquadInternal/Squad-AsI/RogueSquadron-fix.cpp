#include <windows.h>
#include "../WidescreenHackLib/PerspectiveCorrection.h"

PerspectiveCorrection* perspectiveCorrection;

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		HMODULE mod = GetModuleHandle(NULL);
		perspectiveCorrection = new PerspectiveCorrection("");

		char moduleName[MAX_PATH];
		GetModuleFileName(mod, moduleName, MAX_PATH);

		std::string str = (std::string)moduleName;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		auto filename = str.substr(str.find_last_of('\\') + 1);
		if(filename.find("rogue") != std::string::npos && filename.find("squadron") != std::string::npos)
			perspectiveCorrection->FixStuff();

	}
	else if (reason == DLL_PROCESS_DETACH) {
	}
	return TRUE;
}