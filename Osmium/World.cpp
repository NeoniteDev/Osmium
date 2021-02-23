#include "World.h"
#include "native.h"
#include "globals.h"

using namespace osmium;

void GameThread()
{
	while (true)
	{
		bool wantsToSprint = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController)->bWantsToSprint;

		if (osWorld->osAthenaPlayerPawn->CurrentWeapon && !osWorld->osAthenaPlayerPawn->CurrentWeapon->IsReloading() && !osWorld->osAthenaPlayerPawn->CurrentWeapon->bIsTargeting)
		{
			osWorld->osAthenaPlayerPawn->CurrentMovementStyle = wantsToSprint ? EFortMovementStyle::Sprinting : EFortMovementStyle::Running;
		}

		if (GetAsyncKeyState(VK_SPACE))
		{
			if (!osWorld->bHasJumped)
			{
				osWorld->bHasJumped = !osWorld->bHasJumped;

				bool isInAircraft = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController)->IsInAircraft();
				if (!isInAircraft)
				{
					if (osWorld->osAthenaPlayerPawn->IsJumpProvidingForce()) osWorld->osAthenaPlayerPawn->Jump();
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

	osPlayerController->CheatManager->DestroyAll(AFortHLODSMActor::StaticClass());

	Spawn();

	auto FortPlayerController = reinterpret_cast<AFortPlayerController*>(osPlayerController);

	auto Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
	auto AthenaGameState = static_cast<AFortGameStateAthena*>(GEngine->GameViewport->World->GameState);
	AthenaGameState->CurrentPlaylistData = Playlist;
	AthenaGameState->OnRep_CurrentPlaylistData();

	FortPlayerController->ServerReadyToStartMatch();
	auto GameMode = reinterpret_cast<AGameMode*>(GEngine->GameViewport->World->AuthorityGameMode);
	GameMode->StartMatch();

	UE4_CONSOLE_LOG(L"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	UE4_CONSOLE_LOG(L"Made with ♥ By @xkem0x, @DarkbladeEU, @NotMakks and @SizzyLeaks.\nPlease credit us for our hard work ♥.");

	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&GameThread), nullptr, NULL, nullptr);

	return;
}

auto World::FindActor(UClass* pClass) -> UObject*
{
	TArray<AActor*> Actors;

	auto GameplayStatics = UGameplayStatics::StaticClass()->CreateDefaultObject<UGameplayStatics>();
	GameplayStatics->STATIC_GetAllActorsOfClass(GEngine->GameViewport->World, pClass, &Actors);

	if (Actors.Num() > 0)
	{
		auto pActor = Actors.operator[](0);
		if (pActor) return pActor;
	}

	return nullptr;
}

auto World::Spawn() -> void
{
	osPlayerController->CheatManager->Summon(L"PlayerPawn_Athena_C");
	osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(FindActor(AFortPlayerPawnAthena::StaticClass()));
	osPlayerController->Possess(osAthenaPlayerPawn);

	osPlayerController->CheatManager->God();

	auto Location = osAthenaPlayerPawn->K2_GetActorLocation();
	Location.Z = Location.Z + 4000;

	FRotator Rotation;
	Rotation.Pitch = 0;
	Rotation.Yaw = 0;
	Rotation.Roll = 0;

	osAthenaPlayerPawn->K2_SetActorLocationAndRotation(Location, Rotation, false, true, new FHitResult());

	auto AthenaPlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(osPlayerController);
	auto AthenaPlayerState = reinterpret_cast<AFortPlayerStateAthena*>(osAthenaPlayerPawn->PlayerState);
	auto PlayerState = reinterpret_cast<AFortPlayerState*>(AthenaPlayerState);

	std::vector<UCustomCharacterPart*> CharPartsArray;

	auto HeroCharParts = AthenaPlayerController->StrongMyHero->CharacterParts;

	for (auto i = 0; i < HeroCharParts.Num(); i++) CharPartsArray.push_back(HeroCharParts[i]);

	auto Backpack = AthenaPlayerController->CustomizationLoadout.Backpack;
	auto BackpackCharPart = Backpack->GetCharacterParts()[0];
	CharPartsArray.push_back(BackpackCharPart);

	for (auto i = 0; i < CharPartsArray.size(); i++)
	{
		if (CharPartsArray[i]->AdditionalData->IsA(UCustomCharacterHeadData::StaticClass()))
			osAthenaPlayerPawn->ServerChoosePart(EFortCustomPartType::Head, CharPartsArray[i]);

		else if (CharPartsArray[i]->AdditionalData->IsA(UCustomCharacterBodyPartData::StaticClass()))
			osAthenaPlayerPawn->ServerChoosePart(EFortCustomPartType::Body, CharPartsArray[i]);

		else if (CharPartsArray[i]->AdditionalData->IsA(UCustomCharacterHatData::StaticClass()))
			osAthenaPlayerPawn->ServerChoosePart(EFortCustomPartType::Hat, CharPartsArray[i]);

		else if (CharPartsArray[i]->AdditionalData->IsA(UCustomCharacterBackpackData::StaticClass()))
			osAthenaPlayerPawn->ServerChoosePart(EFortCustomPartType::Backpack, CharPartsArray[i]);
	}

	auto Pickaxe = AthenaPlayerController->CustomizationLoadout.Pickaxe->CreateTemporaryItemInstanceBP(1, 3);
	auto Weapon = Pickaxe->GetSchematicCraftingResultOrCraftedWeaponBP();

	if (!Weapon) Weapon = UObject::FindObject<UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_HolidayCandyCane_Athena.WID_Harvest_Pickaxe_HolidayCandyCane_Athena");
	else MessageBoxA(nullptr, Weapon->GetFullName().c_str(), "test", MB_OK);

	FGuid guid;
	guid.A = rand();
	guid.B = rand();
	guid.C = rand();
	guid.D = rand();

	osAthenaPlayerPawn->EquipWeaponDefinition(Weapon, guid);

	PlayerState->OnRep_CharacterParts();
	osAthenaPlayerPawn->OnRep_CustomizationLoadout();
}

auto World::Respawn() -> void
{
	Despawn();
	osPlayerController->CheatManager->Summon(L"PlayerPawn_Athena_C");
	osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(FindActor(AFortPlayerPawnAthena::StaticClass()));
	osPlayerController->Possess(osAthenaPlayerPawn);
}

auto World::Despawn() const -> void
{
	if (osAthenaPlayerPawn) osAthenaPlayerPawn->K2_DestroyActor();
}

/// <summary>
///  Deconstruct Osmium.
/// </summary>
World::~World()
{
}
