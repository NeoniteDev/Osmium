#include "World.h"
#include "native.h"
#include "globals.h"

using namespace osmium;

void GameThread()
{
	while (true)
	{
		bool wantsToSprint = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController)->bWantsToSprint;
		if (!osWorld->bIsSprinting)
		{
			osWorld->bIsSprinting = true;
			if (osWorld->osPlayerPawn->CurrentWeapon && !osWorld->osPlayerPawn->CurrentWeapon->IsReloading() && !osWorld->osPlayerPawn->CurrentWeapon->bIsTargeting) osWorld
				->osPlayerPawn->
				CurrentMovementStyle =
				wantsToSprint ? EFortMovementStyle::Sprinting : EFortMovementStyle::Running;
		}
		else osWorld->bIsSprinting = false;

		if (GetAsyncKeyState(VK_SPACE))
		{
			if (!osWorld->bHasJumped)
			{
				osWorld->bHasJumped = !osWorld->bHasJumped;

				bool isInAircraft = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController)->IsInAircraft();
				if (!isInAircraft)
				{
					if (osWorld->osPlayerPawn->IsSkydiving() && !osWorld->osPlayerPawn->IsParachuteOpen() && !osWorld->osPlayerPawn->IsParachuteForcedOpen()) osWorld->bWantsToOpenGlider = true;
					else if (osWorld->osPlayerPawn->IsSkydiving() && osWorld->osPlayerPawn->IsParachuteOpen() && !osWorld->osPlayerPawn->IsParachuteForcedOpen()) osWorld->bWantsToSkydive = true;
					else if (osWorld->osPlayerPawn->IsJumpProvidingForce())
						//osWorld->bWantsToJump = true;
						osWorld->osPlayerPawn->Jump();
				}
			}
		}
		else osWorld->bHasJumped = false;

		Sleep(1000 / 30);
	}
}

/// <summary>
/// Construct Osmium.
/// </summary>
World::World() : Status(EWorldStatus::Constructing)
{
	osPlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;
	osPlayerPawn = static_cast<AFortPlayerPawnAthena*>(osPlayerController->AcknowledgedPawn);

	Native::InitCheatManager();

	osPlayerController->CheatManager->God();
	osPlayerController->CheatManager->DestroyAll(AFortHLODSMActor::StaticClass());

	auto Location = osPlayerPawn->K2_GetActorLocation();
	Location.Z = Location.Z + 4000;

	FRotator Rotation;
	Rotation.Pitch = 0;
	Rotation.Yaw = 0;
	Rotation.Roll = 0;

	osPlayerPawn->K2_SetActorLocationAndRotation(Location, Rotation, false, true, new FHitResult());

	osPlayerPawn->OnRep_CustomizationLoadout();

	auto PlayerState = reinterpret_cast<AFortPlayerStateAthena*>(osPlayerPawn->PlayerState);
	auto HeroCharParts = reinterpret_cast<AFortPlayerControllerAthena*>(osPlayerController)->StrongMyHero->CharacterParts;

	for (auto i = 0; i < HeroCharParts.Num(); i++) PlayerState->CharacterParts[i] = HeroCharParts[i];

	reinterpret_cast<AFortPlayerState*>(PlayerState)->OnRep_CharacterParts();
	reinterpret_cast<AFortPlayerState*>(PlayerState)->OnRep_ShowHeroBackpack();

	reinterpret_cast<AFortPlayerController*>(osPlayerController)->ServerReadyToStartMatch();
	reinterpret_cast<AGameMode*>(GEngine->GameViewport->World->AuthorityGameMode)->StartMatch();

	//CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&GameThread), nullptr, NULL, nullptr);

	return;
}


/// <summary>
///  Deconstruct Osmium.
/// </summary>
World::~World()
{
}
