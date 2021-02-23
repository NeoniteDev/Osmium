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
			if (osWorld->osAthenaPlayerPawn->CurrentWeapon && !osWorld->osAthenaPlayerPawn->CurrentWeapon->IsReloading() && !osWorld->osAthenaPlayerPawn->CurrentWeapon->bIsTargeting)
				osWorld ->osAthenaPlayerPawn->CurrentMovementStyle = wantsToSprint ? EFortMovementStyle::Sprinting : EFortMovementStyle::Running;
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

	auto AthenaPlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(osPlayerController);
	auto FortPlayerController = reinterpret_cast<AFortPlayerController*>(osPlayerController);
	auto AthenaPlayerState = reinterpret_cast<AFortPlayerStateAthena*>(osAthenaPlayerPawn->PlayerState);
	auto PlayerState = reinterpret_cast<AFortPlayerState*>(AthenaPlayerState);

	auto HeroCharParts = AthenaPlayerController->StrongMyHero->CharacterParts;

	for (auto i = 0; i < HeroCharParts.Num(); i++) AthenaPlayerState->CharacterParts[i] = HeroCharParts[i];

	auto FortWorker = static_cast<UFortWorker*>(AthenaPlayerController->StrongMyHero);
	auto Gender = static_cast<EFortCustomGender>(FortWorker->Gender);

	auto MaleSkeleton = UObject::FindObject<USkeletalMesh>("SkeletalMesh SK_M_MALE_Base_Skeleton.SK_M_MALE_Base_Skeleton");
	auto FemaleSkeleton = UObject::FindObject<USkeletalMesh>("SkeletalMesh SK_M_Female_Base_Skeleton.SK_M_Female_Base_Skeleton");

	if (Gender == EFortCustomGender::Female) osAthenaPlayerPawn->Mesh->SetSkeletalMesh(FemaleSkeleton, true);
	else osAthenaPlayerPawn->Mesh->SetSkeletalMesh(MaleSkeleton, true);

	PlayerState->OnRep_CharacterParts();
	osAthenaPlayerPawn->OnRep_CustomizationLoadout();

	FortPlayerController->ServerReadyToStartMatch();
	auto GameMode = reinterpret_cast<AGameMode*>(GEngine->GameViewport->World->AuthorityGameMode);
	GameMode->StartMatch();

	UE4_CONSOLE_LOG(L"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	UE4_CONSOLE_LOG(L"Made with ♥ By @xkem0x, @DarkbladeEU, @NotMakks and @SizzyLeaks,\nPlease credit us for our hard work ♥.");

	//CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&GameThread), nullptr, NULL, nullptr);

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
