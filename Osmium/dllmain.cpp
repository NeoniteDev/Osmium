#include "curl.h"
#include "framework.h"

using namespace osmium;

void dllMain()
{
	Util::InitConsole();

	printf("LogOsmium: Init!\n");

	cURL* cURLHook = new cURL;

	while (true)
	{
		//When we are in lobby we let go.
		if (isInLobby)
		{
			MessageBoxA(nullptr, "test", "osmium", MB_OK);
			UFortGameViewportClient* GameViewport = UObject::FindObject<UFortGameViewportClient>("FortGameViewportClient Transient.FortEngine_1.FortGameViewportClient_1");
			GameViewport->GameInstance->LocalPlayers[0]->PlayerController->SwitchLevel(L"Athena_Terrain?game=/Script/Engine.GameModeBase");
			break;
		}

		Sleep(1000 / 30); //30 fps 
	}
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		dllMain();
	}
	else if (dwReason == DLL_PROCESS_DETACH) FreeLibraryAndExitThread(hModule, EXIT_SUCCESS);

	return TRUE;
}
