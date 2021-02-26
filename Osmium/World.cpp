#include "World.h"
#include "native.h"
#include "globals.h"
#include "framework.h"

using namespace osmium;

/// <summary>
/// Construct Osmium.
/// </summary>
World::World()
{
	osWorldStatus = EWorldStatus::Constructing;

	osPlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;

	Native::InitCheatManager();

	osPlayerController->CheatManager->DestroyAll(AFortHLODSMActor::StaticClass());

	Spawn();

	auto FortPlayerController = reinterpret_cast<AFortPlayerController*>(osPlayerController);

	auto Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
	auto AthenaGameState = static_cast<AFortGameStateAthena*>(GEngine->GameViewport->World->GameState);
	AthenaGameState->CurrentPlaylistData = Playlist;
	AthenaGameState->OnRep_CurrentPlaylistData();

	AthenaGameState->TotalPlayers = 1;
	AthenaGameState->PlayersLeft = 1;

	FortPlayerController->ServerReadyToStartMatch();
	auto GameMode = reinterpret_cast<AGameMode*>(GEngine->GameViewport->World->AuthorityGameMode);
	GameMode->StartMatch();

	osWorldStatus = EWorldStatus::InGame;

	UE4_CONSOLE_LOG(L"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	UE4_CONSOLE_LOG(L"Made with ♥ By @xkem0x, @DarkbladeEU and @SizzyLeaks.\nPlease credit us for our hard work ♥.");

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
	Location.X = -127500;
	Location.Y = -110500;
	Location.Z = Location.Z + 4000;

	FRotator Rotation;
	Rotation.Pitch = 0;
	Rotation.Yaw = 0;
	Rotation.Roll = 0;

	osAthenaPlayerPawn->K2_SetActorLocationAndRotation(Location, Rotation, false, true, new FHitResult());

	auto AthenaPlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(osPlayerController);
	auto AthenaPlayerState = reinterpret_cast<AFortPlayerStateAthena*>(osAthenaPlayerPawn->PlayerState);

	std::vector<UCustomCharacterPart*> CharPartsArray;

	auto HeroCharParts = AthenaPlayerController->StrongMyHero->CharacterParts;
	for (auto i = 0; i < HeroCharParts.Num(); i++) 
		CharPartsArray.push_back(HeroCharParts[i]);

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

	AthenaPlayerState->OnRep_CharacterParts();
	osAthenaPlayerPawn->OnRep_CustomizationLoadout();

	EquipPickaxe();
}

auto World::Respawn() -> void
{
	Despawn();
	osPlayerController->CheatManager->Summon(L"PlayerPawn_Athena_C");
	osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(FindActor(AFortPlayerPawnAthena::StaticClass()));
	osPlayerController->Possess(osAthenaPlayerPawn);
}

auto World::Despawn() -> void
{
	if (osAthenaPlayerPawn) 
		osAthenaPlayerPawn->K2_DestroyActor();
}

auto World::EquipPickaxe() -> void
{
	auto AthenaPlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(osPlayerController);

	auto PickaxeID = AthenaPlayerController->CustomizationLoadout.Pickaxe->GetName();
	auto Weapon = UObject::FindObject<UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");

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
				
				Weapon = UObject::FindObject<UFortWeaponMeleeItemDefinition>(AssetName);
			}
		}
	}

	FGuid GUID;
	GUID.A = rand();
	GUID.B = rand();
	GUID.C = rand();
	GUID.D = rand();

	osAthenaPlayerPawn->EquipWeaponDefinition(Weapon, GUID);
}

/// <summary>
///  Deconstruct Osmium.
/// </summary>
World::~World()
{

}