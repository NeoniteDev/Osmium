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
		
		// When we are in lobby and everything is initalized, we are ready to go in-game at any time.
		// NOTE (kemo): This will be moved to a ProcessEvent hook at some point.
		if (isReady && isInLobby && GetAsyncKeyState(VK_F5))
		{
			osWorld = new World();
			break;
		}

		// Run the thread at 30 frames a second.
		Sleep(1000 / 30);
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
