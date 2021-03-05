﻿#include "World.h"
#include "native.h"
#include "globals.h"
#include "framework.h"

using namespace osmium;

/// <summary>
/// Construct Osmium.
/// </summary>
World::World()
{
	printf("LogOsmium: Constructing world\n");
	osWorldStatus = EWorldStatus::Constructing;

	osPlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;
	osFortPlayerController = reinterpret_cast<AFortPlayerController*>(osPlayerController);
	osFortPlayerControllerAthena = static_cast<AFortPlayerControllerAthena*>(osFortPlayerController);

	Native::InitCheatManager();

	osPlayerController->CheatManager->DestroyAll(AFortHLODSMActor::StaticClass());

	Spawn();

	auto Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_Playground.Playlist_Playground");
	auto AthenaGameState = static_cast<AFortGameStateAthena*>(GEngine->GameViewport->World->GameState);
	AthenaGameState->CurrentPlaylistData = Playlist;
	AthenaGameState->OnRep_CurrentPlaylistData();
	printf("LogOsmium: Current playlist set to Playlist_Playground\n");

	auto WidgetLibrary = UObject::FindClass("Class UMG.WidgetBlueprintLibrary")->CreateDefaultObject<UWidgetBlueprintLibrary>();

	AthenaGameState->MiniMapBackgroundDrawingMaterial = nullptr;
	AthenaGameState->MinimapBackgroundImage = UObject::FindObject<UTexture2D>("Texture2D MiniMapAthena.MiniMapAthena");
	AthenaGameState->MinimapBackgroundBrush = WidgetLibrary->STATIC_MakeBrushFromTexture(AthenaGameState->MinimapBackgroundImage, 2048, 2048);

	FSlateBrush EmptyBrush = WidgetLibrary->STATIC_NoResourceBrush();

	AthenaGameState->MinimapCircleMID = nullptr;
	AthenaGameState->MinimapNextCircleMID = nullptr;
	AthenaGameState->FullmapCircleMID = nullptr;
	AthenaGameState->FullmapNextCircleMID = nullptr;
	AthenaGameState->MiniMapCircleDrawingMaterial = nullptr;
	AthenaGameState->MiniMapNextCircleDrawingMaterial = nullptr;

	AthenaGameState->MinimapCircleBrush = EmptyBrush;
	AthenaGameState->MinimapSafeZoneBrush = EmptyBrush;
	AthenaGameState->MinimapNextCircleBrush = EmptyBrush;
	AthenaGameState->FullMapCircleBrush = EmptyBrush;
	AthenaGameState->FullMapNextCircleBrush = EmptyBrush;
	AthenaGameState->MinimapSafeZoneFinalPosBrush = EmptyBrush;

	AthenaGameState->TotalPlayers = 1;
	AthenaGameState->PlayersLeft = 1;

	printf("LogOsmium: Starting match\n");
	osFortPlayerController->ServerReadyToStartMatch();
	auto GameMode = reinterpret_cast<AGameMode*>(GEngine->GameViewport->World->AuthorityGameMode);
	GameMode->StartMatch();

	osWorldStatus = EWorldStatus::InGame;

	UE4_CONSOLE_LOG(L"Osmium by @xkem0x, @DarkbladeEU and @SizzyLeaks.\nPlease credit us for our hard work ♥\nHuge thanks to @NotMakks for providing an SDK and crash fixes.");

	return;
}

auto World::FindActors(UClass* pClass) -> TArray<AActor*>
{
	TArray<AActor*> Actors;

	auto GameplayStatics = UGameplayStatics::StaticClass()->CreateDefaultObject<UGameplayStatics>();
	GameplayStatics->STATIC_GetAllActorsOfClass(GEngine->GameViewport->World, pClass, &Actors);

	return Actors;
}

auto World::Spawn() -> void
{
	osPlayerController->CheatManager->Summon(L"PlayerPawn_Athena_C");

	auto PlayerPawn = FindActors(AFortPlayerPawnAthena::StaticClass())[0];
	if (!PlayerPawn)
	{
		printf("LogOsmium: Failed to find actors of class AFortPlayerPawnAthena!\n");
		MessageBoxA(nullptr, "Failed to find AthenaPlayerPawn!", "Osmium", MB_OK);
	}

	osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(PlayerPawn);
	osPlayerController->Possess(osAthenaPlayerPawn);
	printf("LogOsmium: AthenaPlayerPawn summoned and possessed\n");

	osPlayerController->CheatManager->God();
	printf("LogOsmium: PlayerController now has god-mode\n");

	auto Location = osAthenaPlayerPawn->K2_GetActorLocation();
	Location.X = -127500;
	Location.Y = -110500;
	Location.Z = Location.Z + 4000;

	FRotator Rotation;
	Rotation.Pitch = 0;
	Rotation.Yaw = 0;
	Rotation.Roll = 0;

	osAthenaPlayerPawn->K2_SetActorLocationAndRotation(Location, Rotation, false, true, new FHitResult());
	printf("LogOsmium: Set AthenaPlayerPawn's location to spawn-island\n");

	auto AthenaPlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(osPlayerController);
	auto AthenaPlayerState = reinterpret_cast<AFortPlayerStateAthena*>(osAthenaPlayerPawn->PlayerState);

	std::vector<UCustomCharacterPart*> CharPartsArray;

	auto HeroCharParts = AthenaPlayerController->StrongMyHero->CharacterParts;
	for (auto i = 0; i < HeroCharParts.Num(); i++)
		CharPartsArray.push_back(HeroCharParts[i]);

	auto Backpack = AthenaPlayerController->CustomizationLoadout.Backpack;

	if (Backpack)
	{
		try
		{
			auto BackpackCharPart = Backpack->GetCharacterParts()[0];
			CharPartsArray.push_back(BackpackCharPart);
		}
		catch (...) {}
	}

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

	AthenaPlayerState->OnRep_CharacterParts();
	osAthenaPlayerPawn->OnRep_CustomizationLoadout();

	printf("LogOsmium: Added character parts and customization loadout\n");

	EquipPickaxe();
}

auto World::Respawn() -> void
{
	if (osAthenaPlayerPawn) osAthenaPlayerPawn->K2_DestroyActor();

	osPlayerController->CheatManager->Summon(L"PlayerPawn_Athena_C");

	auto PlayerPawn = FindActors(AFortPlayerPawnAthena::StaticClass())[0];
	if (!PlayerPawn)
	{
		printf("LogOsmium: Failed to find actors of class AFortPlayerPawnAthena!\n");
		MessageBoxA(nullptr, "Failed to find AthenaPlayerPawn!", "Osmium", MB_OK);
	}

	osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(PlayerPawn);
	osPlayerController->Possess(osAthenaPlayerPawn);

	printf("LogOsmium: Respawned player\n");
}

auto World::Tick() -> void
{
	if (osWorldStatus == EWorldStatus::InGame)
	{
		if (!osFortPlayerController)
			osFortPlayerController = static_cast<AFortPlayerController*>(osPlayerController);

		if (!osAthenaPlayerPawn) 
			osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(osFortPlayerController->Pawn);

		if (!osFortPlayerControllerAthena) 
			osFortPlayerControllerAthena = static_cast<AFortPlayerControllerAthena*>(osFortPlayerController);

		osFortAnimInstance = static_cast<UFortAnimInstance*>(osAthenaPlayerPawn->Mesh->GetAnimInstance());

		if (!osFortPlayerControllerAthena->IsInAircraft() && !osAthenaPlayerPawn->IsSkydiving() && osFortAnimInstance &&
			(osAthenaPlayerPawn->bIsCrouched || osFortPlayerController->bIsPlayerActivelyMoving || osFortAnimInstance->bIsJumping || osFortAnimInstance->bIsFalling))
		{
			auto CurrentMontage = osFortAnimInstance->GetCurrentActiveMontage();

			if (CurrentMontage && (CurrentMontage->GetName().starts_with("Emote_") || CurrentMontage->GetName().starts_with("Basketball_CMM")))
				osAthenaPlayerPawn->ServerRootMotionInterruptNotifyStopMontage(CurrentMontage);
		}

		if (!osAthenaPlayerPawn->CurrentWeapon && !osFortPlayerControllerAthena->IsInAircraft())
			EquipPickaxe();

		bool bWantsToSprint = osFortPlayerController->bWantsToSprint;
		osAthenaPlayerPawn->CurrentMovementStyle = bWantsToSprint ? EFortMovementStyle::Charging : EFortMovementStyle::Sprinting;

		if (GetAsyncKeyState(VK_SPACE))
		{
			if (bHasJumped == false)
			{
				if (!osFortPlayerControllerAthena->IsInAircraft())
				{
					if (!osAthenaPlayerPawn->IsParachuteForcedOpen())
					{
						if (osAthenaPlayerPawn->IsSkydiving() && !osAthenaPlayerPawn->IsParachuteOpen()) 
							osAthenaPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 3);
						else if (osAthenaPlayerPawn->IsParachuteOpen()) 
							osAthenaPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 4);

						osAthenaPlayerPawn->OnRep_IsParachuteOpen(osAthenaPlayerPawn->IsParachuteOpen());
					}

					if (!osAthenaPlayerPawn->IsSkydiving() || osAthenaPlayerPawn->IsParachuteOpen())
						osAthenaPlayerPawn->Jump();
				}

				bHasJumped = true;
			}
		}
		else bHasJumped = false;
	}
}

auto World::EquipPickaxe() -> void
{
	auto PickaxeID = osFortPlayerControllerAthena->CustomizationLoadout.Pickaxe->GetName();

	if (osPickaxe)
	{
		FGuid GUID;
		GUID.A = 0;
		GUID.B = 0;
		GUID.C = 0;
		GUID.D = 0;

		osAthenaPlayerPawn->EquipWeaponDefinition(osPickaxe, GUID);

		printf("LogOsmium: Equipped pickaxe %s\n", PickaxeID.c_str());

		return;
	}

	osPickaxe = UObject::FindObject<UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");

	httplib::Client client("https://fortnite-api.com");
	client.set_follow_location(true);

	std::string route = "/v2/cosmetics/br/search?id=" + PickaxeID;

	if (auto response = client.Get(route.c_str()))
	{
		if (response->status == 200)
		{
			auto Data = nlohmann::json::parse(response->body)["data"];

			if (!Data.is_null())
			{
				std::string WeaponID = Data["definitionPath"].get<std::string>();
				WeaponID = WeaponID.erase(0, WeaponID.find("WID"));

				auto AssetName = "FortWeaponMeleeItemDefinition " + WeaponID + "." + WeaponID;

				osPickaxe = UObject::FindObject<UFortWeaponMeleeItemDefinition>(AssetName);
			}
		}
	}

	FGuid GUID;
	GUID.A = 0;
	GUID.B = 0;
	GUID.C = 0;
	GUID.D = 0;

	auto Weapon = osAthenaPlayerPawn->EquipWeaponDefinition(osPickaxe, GUID);

	printf("LogOsmium: Equipped pickaxe %s\n", PickaxeID.c_str());
}

/// <summary>
///  Deconstruct Osmium.
/// </summary>
World::~World()
{

}