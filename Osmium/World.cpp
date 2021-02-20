#include "World.h"
#include "native.h"

using namespace osmium;

/// <summary>
/// Construct Osmium.
/// </summary>
World::World(): Status(EWorldStatus::Constructing)
{
	osPlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;

	Native::InitCheatManager();

	osPlayerController->CheatManager->DestroyAll(AFortHLODSMActor::StaticClass());
	osPlayerController->CheatManager->Summon(FString(L"PlayerPawn_Athena_C"));
}

/// <summary>
///  Deconstruct Osmium.
/// </summary>
World::~World()
{

}