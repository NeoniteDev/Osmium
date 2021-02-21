#include "World.h"
#include "native.h"
#include "globals.h"

using namespace osmium; 

#define READ_POINTER(base, offset) (*(PVOID*)(((PBYTE)base + offset)))
#define READ_DWORD(base, offset) (*(PDWORD)(((PBYTE)base + offset)))

void GameThread()
{
	while (true)
	{
		bool wantsToSprint = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController)->bWantsToSprint;
		if (!osWorld->bIsSprinting)
		{
			osWorld->bIsSprinting = true;
			if (osWorld->osAthenaPlayerPawn->CurrentWeapon && !osWorld->osAthenaPlayerPawn->CurrentWeapon->IsReloading() && !osWorld->osAthenaPlayerPawn->CurrentWeapon->bIsTargeting) 
				osWorld->osAthenaPlayerPawn->CurrentMovementStyle = wantsToSprint ? EFortMovementStyle::Sprinting : EFortMovementStyle::Running;
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
					if (osWorld->osAthenaPlayerPawn->IsSkydiving() && !osWorld->osAthenaPlayerPawn->IsParachuteForcedOpen())
					{
						if (!osWorld->osAthenaPlayerPawn->IsParachuteOpen())
							osWorld->osAthenaPlayerPawn->CharacterMovement->SetMovementMode(EMovementMode::MOVE_Custom, 3);

						if (osWorld->osAthenaPlayerPawn->IsParachuteOpen())
							osWorld->osAthenaPlayerPawn->CharacterMovement->SetMovementMode(EMovementMode::MOVE_Custom, 4);

						osWorld->osAthenaPlayerPawn->OnRep_IsParachuteOpen(osWorld->osAthenaPlayerPawn->IsParachuteOpen());
					}
					else if (osWorld->osAthenaPlayerPawn->IsJumpProvidingForce())
						osWorld->osAthenaPlayerPawn->Jump();
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

	osPlayerController->CheatManager->God();
	osPlayerController->CheatManager->DestroyAll(AFortHLODSMActor::StaticClass());

	Native::InitCheatManager();

	osPlayerController->CheatManager->Summon(L"PlayerPawn_Athena_C");
	osFortPlayerPawn = static_cast<AFortPlayerPawn*>(FindActor(AFortPlayerPawn::StaticClass()));

	if (!osFortPlayerPawn)
		MessageBoxA(nullptr, "Failed to find actor for AFortPlayerPawn!", "Error", MB_ICONERROR);

	osPlayerController->Possess(osFortPlayerPawn);

	osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(osFortPlayerPawn);

	auto Location = osAthenaPlayerPawn->K2_GetActorLocation();
	Location.Z = Location.Z + 4000;

	FRotator Rotation;
	Rotation.Pitch = 0;
	Rotation.Yaw = 0;
	Rotation.Roll = 0;

	osAthenaPlayerPawn->K2_SetActorLocationAndRotation(Location, Rotation, false, true, new FHitResult());

	osAthenaPlayerPawn->OnRep_CustomizationLoadout();

	auto AthenaPlayerState = reinterpret_cast<AFortPlayerStateAthena*>(osAthenaPlayerPawn->PlayerState);
	auto PlayerState = reinterpret_cast<AFortPlayerState*>(AthenaPlayerState);

	auto AthenaPlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(osPlayerController);
	auto FortPlayerController = reinterpret_cast<AFortPlayerController*>(osPlayerController);

	auto HeroCharParts = AthenaPlayerController->StrongMyHero->CharacterParts;
	for (auto i = 0; i < HeroCharParts.Num(); i++) AthenaPlayerState->CharacterParts[i] = HeroCharParts[i];

	PlayerState->OnRep_CharacterParts();
	PlayerState->OnRep_ShowHeroBackpack();

	FortPlayerController->ServerReadyToStartMatch();

	auto GameMode = reinterpret_cast<AGameMode*>(GEngine->GameViewport->World->AuthorityGameMode);
	GameMode->StartMatch();

	//CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&GameThread), nullptr, NULL, nullptr);

	return;
}


AActor* osmium::World::FindActor(UClass* pClass)
{
	auto PersistentLevel = GEngine->GameViewport->World->PersistentLevel;

	const DWORD AActors = 0x98;

	for (int i = 0x00; i < READ_DWORD(PersistentLevel, AActors + sizeof(void*)); i++)
	{
		auto Actors = READ_POINTER(PersistentLevel, AActors);

		auto pActor = static_cast<AActor*>(READ_POINTER(Actors, i * sizeof(void*)));

		if (pActor)
		{
			MessageBoxA(nullptr, pActor->GetFullName().c_str(), "", MB_OK);
			
			if (pActor->IsA(pClass)) return pActor;
		}
	}

	return nullptr;
}

/// <summary>
///  Deconstruct Osmium.
/// </summary>
World::~World()
{
}
