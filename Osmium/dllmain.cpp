#include "curl.h"
#include "framework.h"
#include "globals.h"
#include "native.h"
#include "World.h"

using namespace osmium;

using namespace SDK;

static bool isReady;

void WINAPI dllMain()
{
	Util::InitConsole();

	printf("LogOsmium: Init!\n");

	cURL* cURLHook = new cURL;

	while (true)
	{
		if (!isReady && isInLobby)
		{
			Native::Init();
			isReady = !isReady;
		}
		
		//When we are in lobby and everything is initalized, we are ready to go in-game at any time.
		//NOTE (kemo): we will move this to a pe hook or url listening inside the world itself.
		if (isReady && isInLobby && GetAsyncKeyState(VK_F5))
		{
			osWorld = new World();
			break;
		}

		Sleep(1000 / 30); //30 fps 
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&dllMain), nullptr, 0, 0);
	}
	else if (dwReason == DLL_PROCESS_DETACH) FreeLibraryAndExitThread(hModule, EXIT_SUCCESS);

	return TRUE;
}
