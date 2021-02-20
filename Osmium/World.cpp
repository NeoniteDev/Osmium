#include "World.h"
#include "native.h"

using namespace osmium;

World::World(): Status(EOsmiumWorldStatus::Constructing)
{
	osPlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;

	Native::CheatManager();

	osPlayerController->CheatManager->DestroyAll(AFortHLODSMActor::StaticClass());

	osPlayerController->CheatManager->Summon(FString(L"PlayerPawn_Athena_C"));
}
