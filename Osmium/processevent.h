#pragma once
#include "curl.h"
#include "World.h"
#include "globals.h"
#include "framework.h"
#include "util.h"

//Used to log processevent
#ifndef _PROD
#define LOGGING
#endif

inline void* (*ProcessEvent)(void* pUObject, void* pUFunction, void* pParams);

/********************************************************************
 This is a ProcessEvent hook. We usually don't call it at all, but
 instead, we call the native function directly so our calls don't
 get hijacked. ProcessEvent Hook is used to track what function is
 being called and have a time window to manipulate it/edit it or do
 something to it before it's actually called. Usually after we're
 done with the manipulation, we do "goto out" to jump to the end and
 return the parameters directly.
********************************************************************/
inline void* ProcessEventDetour(UObject* pObj, UObject* pFunc, void* pParams)
{
	const auto nObj = pObj->GetName();
	const auto nFunc = pFunc->GetName();

	if (nFunc == "Tick")
	{
		osWorld->Tick();

		goto out;
	}

	if (gUrl.find("matchmakingservice") != std::string::npos)
	{
		gUrl.clear();

		auto PlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;
		PlayerController->SwitchLevel(Strings::Maps::AthenaTerrain);
		goto out;
	}

	if (nFunc == "ReadyToStartMatch" && osWorldStatus == EWorldStatus::InLobby)
	{
		osWorld = new osmium::World();

		goto out;
	}

	if (nFunc == "ServerAttemptAircraftJump" || nFunc == "OnAircraftExitedDropZone")
	{
		auto AthenaPlayerController = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController);
		if (AthenaPlayerController->IsInAircraft()) osWorld->Respawn();

		goto out;
	}

	if (nFunc == "ServerPlayEmoteItem")
	{
		auto FortAnimInstance = static_cast<UFortAnimInstance*>(osWorld->osAthenaPlayerPawn->Mesh->GetAnimInstance());

		if (!FortAnimInstance->bIsJumping && !FortAnimInstance->bIsFalling && !osWorld->osFortPlayerController->bIsPlayerActivelyMoving)
		{
			if (osWorld->osAthenaPlayerPawn->bIsCrouched) osWorld->osAthenaPlayerPawn->UnCrouch(true);

			auto EmoteAsset = static_cast<AFortPlayerController_ServerPlayEmoteItem_Params*>(pParams)->EmoteAsset;
			auto Montage = EmoteAsset->GetAnimationHardReference(EFortCustomBodyType::All, EFortCustomGender::Both);

			auto AnimInstance = osWorld->osAthenaPlayerPawn->Mesh->GetAnimInstance();
			AnimInstance->Montage_Play(Montage, 1, EMontagePlayReturnType::Duration, 0, true);
		}

		goto out;
	}

	if (nFunc == "CheatScript")
	{
		const auto ScriptNameF = static_cast<UCheatManager_CheatScript_Params*>(pParams)->ScriptName;

		if (ScriptNameF.IsValid())
		{
			const auto ScriptName = ScriptNameF.ToString();

			if (ScriptName == "dump")
			{
				std::ofstream log("gobjects.log");

				for (int i = 0; i < UObject::GetGlobalObjects().Num(); ++i)
				{
					auto object = UObject::GetGlobalObjects().GetByIndex(i);

					if (object == nullptr) continue;

					log << object->GetFullName() + "\n";
				}
				printf("LogOsmium: Dumped GObjects to gobjects.log\n");
			}
			else if (ScriptName == "event")
			{
				UE4_CONSOLE_LOG(L"The event will start in a few minutes. Enjoy your stay ♥")

				auto EventPlayer = UObject::FindObject<ULevelSequencePlayer>(
					"LevelSequencePlayer Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.LevelSequence_LaunchRocket.AnimationPlayer");
				EventPlayer->Play();

				printf("LogOsmium: Started event\n");
			}
			else if (ScriptName == "test")
			{
				auto Hud = UObject::FindObject<UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");

				FLinearColor WeaponColor{
					100, 100, 100, 100
				};

				if (Hud)
				{
					auto FortRarityData = UObject::FindObject<UFortRarityData>("FortRarityData FortniteGame.Default__FortRarityData");
					auto Rarity = FortRarityData->STATIC_BPGetRarityData(osWorld->osPickaxe->Rarity);

					auto IIAMOPIS = UObject::FindObject<UItemInspectAlterationModOptInScreen_C>(
						"ItemInspectAlterationModOptInScreen_C ItemInspectAlterationModOptInScreen.WidgetArchetype");

					IIAMOPIS->SetupTriangles(Rarity);

					auto Brush = IIAMOPIS->TriangleMaterial->Brush;
					auto Color = IIAMOPIS->TriangleMaterial->ColorAndOpacity;
					Color.A = 1;

					Hud->QuickbarPrimary->QuickbarSlots[0]->EmptyImage->SetBrush(Brush);
					Hud->QuickbarPrimary->QuickbarSlots[0]->EmptyImage->SetColorAndOpacity(Color);

					Hud->QuickbarPrimary->QuickbarSlots[0]->SlotInteraction->SetBrushFromTexture(osWorld->osPickaxe->SmallPreviewImage, true);
					Hud->QuickbarPrimary->QuickbarSlots[0]->SlotInteraction->SetColorAndOpacity(WeaponColor);
				}
			}
		}
		goto out;
	}

out:
#ifdef LOGGING
	if (nFunc != "EvaluateGraphExposedInputs" &&
		nFunc != "ReceiveTick" &&
		nFunc != "Tick" &&
		nFunc != "OnSubmixEnvelope" &&
		nFunc != "OnSubmixSpectralAnalysis" &&
		nFunc != "OnMouse" &&
		nFunc != "GetSubtitleVisibility" &&
		nFunc != "Pulse" &&
		nFunc != "BlueprintUpdateAnimation" &&
		nFunc != "BlueprintPostEvaluateAnimation" &&
		nFunc != "BlueprintModifyCamera" &&
		nFunc != "BlueprintModifyPostProcess" &&
		nFunc != "Loop Animation Curve" &&
		nFunc != "GetValue" &&
		nFunc != "OnSignificantTick" &&
		nFunc != "ReceiveDrawHUD" &&
		nFunc != "Chime Visual" &&
		nFunc != "UpdateTime" &&
		nFunc != "GetMutatorByClass" &&
		nFunc != "OnUpdateDirectionalLightForTimeOfDay" &&
		nFunc != "UpdatePreviousPositionAndVelocity" &&
		nFunc != "IsCachedIsProjectileWeapon" &&
		nFunc != "LockOn" &&
		nFunc != "GetAbilityTargetingLevel" &&
		nFunc != "ReadyToEndMatch")
	{
		printf("[Object]: %s [Function]: %s\n", nObj.c_str(), nFunc.c_str());
	}
#endif

	return ProcessEvent(pObj, pFunc, pParams);
}

inline bool InitPEH()
{
	const auto ProcessEventAdd = osmium::Util::BaseAddress() + Offsets::ProcessEventOffset;
	ProcessEvent = decltype(ProcessEvent)(ProcessEventAdd);

	DetourTransactionBegin();
	DetourAttach(reinterpret_cast<void**>(&ProcessEvent), ProcessEventDetour);
	const auto error = DetourTransactionCommit();

	if (error == NO_ERROR)
	{
		printf("LogOsmium: ProcessEvent hook initiated\n");
		return true;
	}

	printf("LogOsmium: ProcessEvent hook failed\n");
	MessageBoxA(nullptr, "ProcessEvent hook failed", "Osmium", MB_OK);
	return false;
}
