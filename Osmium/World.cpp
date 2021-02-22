#include "World.h"
#include "native.h"
#include "globals.h"

using namespace osmium;

inline bool isReady;

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
World::World()
{
	osPlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;

	Native::InitCheatManager();

	osPlayerController->CheatManager->God();

	osPlayerController->CheatManager->DestroyAll(AFortHLODSMActor::StaticClass());

	osPlayerController->CheatManager->Summon(L"PlayerPawn_Athena_C");

	osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(FindActor(AFortPlayerPawnAthena::StaticClass()));

	osPlayerController->Possess(osAthenaPlayerPawn);

	auto Location = osAthenaPlayerPawn->K2_GetActorLocation();
	Location.Z = Location.Z + 4000;

	FRotator Rotation;
	Rotation.Pitch = 0;
	Rotation.Yaw = 0;
	Rotation.Roll = 0;

	osAthenaPlayerPawn->K2_SetActorLocationAndRotation(Location, Rotation, false, true, new FHitResult());

	auto AthenaPlayerState = reinterpret_cast<AFortPlayerStateAthena*>(osAthenaPlayerPawn->PlayerState);

	auto PlayerState = reinterpret_cast<AFortPlayerState*>(AthenaPlayerState);

	const auto AthenaPlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(osPlayerController);

	auto FortPlayerController = reinterpret_cast<AFortPlayerController*>(osPlayerController);

	auto HeroCharParts = AthenaPlayerController->StrongMyHero->CharacterParts;

	/*auto KismetSysLib_C = UKismetSystemLibrary::StaticClass();
	auto KismetSysLib = static_cast<UKismetSystemLibrary*>(KismetSysLib_C->CreateDefaultObject());

	auto MasterMesh = reinterpret_cast<USkeletalMesh*>(KismetSysLib->STATIC_Conv_SoftObjectReferenceToObject(reinterpret_cast<void*>(HeroCharParts[0]->MasterSkeletalMeshes[0])));
	osAthenaPlayerPawn->Mesh->SetSkeletalMesh(MasterMesh, true);

	osAthenaPlayerPawn->Mesh->SetAnimInstanceClass(UFortnite_M_Avg_Player_AnimBlueprint_C::StaticClass());*/

	for (auto i = 0; i < HeroCharParts.Num(); i++) AthenaPlayerState->CharacterParts[i] = HeroCharParts[i];

	PlayerState->OnRep_CharacterParts();

	osAthenaPlayerPawn->OnRep_CustomizationLoadout();

	FortPlayerController->ServerReadyToStartMatch();

	auto GameMode = reinterpret_cast<AGameMode*>(GEngine->GameViewport->World->AuthorityGameMode);

	GameMode->StartMatch();

	/*auto MovieSceneSequencePlayer = UMovieSceneSequencePlayer::StaticClass()->CreateDefaultObject<UMovieSceneSequencePlayer>();
	auto SequenceToPlay = UObject::FindObject<void*>("LevelSequencePlayer thethinghere");

	MovieSceneSequencePlayer->ProcessEvent(nullptr, SequenceToPlay);


	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&GameThread), nullptr, NULL, nullptr);*/

	return;
}


auto World::FindActor(UClass* pClass) -> UObject*
{
	TArray<AActor*> Actors;

	auto GameplayStatics = UGameplayStatics::StaticClass()->CreateDefaultObject<UGameplayStatics>();
	GameplayStatics->STATIC_GetAllActorsOfClass(GEngine->GameViewport->World, pClass, &Actors);

	if (Actors.Num() > 0)
	{
		AActor* pActor = Actors.operator[](0);
		if (pActor) return pActor;
	}

	return nullptr;
}

/// <summary>
///  Deconstruct Osmium.
/// </summary>
World::~World()
{
}