#pragma once
#include "curl.h"
#include "World.h"
#include "globals.h"
#include "framework.h"

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
		if (osWorldStatus == EWorldStatus::InGame)
		{
			auto osFortPlayerController = static_cast<AFortPlayerController*>(osWorld->osPlayerController);

			if (!osWorld->osAthenaPlayerPawn)
				osWorld->osAthenaPlayerPawn = static_cast<AFortPlayerPawnAthena*>(osFortPlayerController->Pawn);

			auto FortAnimInstance = static_cast<UFortAnimInstance*>(osWorld->osAthenaPlayerPawn->Mesh->GetAnimInstance());

			if (FortAnimInstance && (FortAnimInstance->bIsJumping || FortAnimInstance->bIsFalling ||
				osWorld->osAthenaPlayerPawn->bIsCrouched || osFortPlayerController->bIsPlayerActivelyMoving))
			{
				auto CurrentMontage = FortAnimInstance->GetCurrentActiveMontage();

				if (CurrentMontage && (CurrentMontage->GetName().starts_with("Emote_") || CurrentMontage->GetName().starts_with("Basketball_CMM")))
					osWorld->osAthenaPlayerPawn->ServerRootMotionInterruptNotifyStopMontage(CurrentMontage);
			}

			bool bWantsToSprint = osFortPlayerController->bWantsToSprint;
			osWorld->osAthenaPlayerPawn->CurrentMovementStyle = bWantsToSprint ? EFortMovementStyle::Sprinting : EFortMovementStyle::Running;

			if (GetAsyncKeyState(VK_SPACE))
			{
				if (osWorld->bHasJumped == false)
				{
					bool isInAircraft = static_cast<AFortPlayerControllerAthena*>(osFortPlayerController)->IsInAircraft();
					if (!isInAircraft)
					{
						if (!osWorld->osAthenaPlayerPawn->IsParachuteForcedOpen())
						{
							if (osWorld->osAthenaPlayerPawn->IsSkydiving() && !osWorld->osAthenaPlayerPawn->IsParachuteOpen())
								osWorld->osAthenaPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 3);
							else if (osWorld->osAthenaPlayerPawn->IsParachuteOpen())
								osWorld->osAthenaPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 4);

							osWorld->osAthenaPlayerPawn->OnRep_IsParachuteOpen(osWorld->osAthenaPlayerPawn->IsParachuteOpen());
						}

						if (!osWorld->osAthenaPlayerPawn->IsSkydiving() && !osWorld->osAthenaPlayerPawn->IsParachuteOpen())
							osWorld->osAthenaPlayerPawn->Jump();
					}

					osWorld->bHasJumped = true;
				}
			}
			else osWorld->bHasJumped = false;

			if (GetAsyncKeyState(VK_F10))
			{
				auto Location = osWorld->osAthenaPlayerPawn->K2_GetActorLocation();

				auto LocationString = "X: " + std::to_string(Location.X) + "\nY: " + std::to_string(Location.Y) + "\nZ: " + std::to_string(Location.Z);

				MessageBoxA(nullptr, LocationString.c_str(), "K2_GetActorLocation", MB_OK);
			}
		}

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
		if (AthenaPlayerController->IsInAircraft()) 
			osWorld->Respawn();

		goto out;
	}

	if (nFunc == "ServerPlayEmoteItem")
	{
		auto osFortPlayerController = static_cast<AFortPlayerController*>(osWorld->osPlayerController);

		auto FortAnimInstance = static_cast<UFortAnimInstance*>(osWorld->osAthenaPlayerPawn->Mesh->GetAnimInstance());

		if (!FortAnimInstance->bIsJumping && !FortAnimInstance->bIsFalling &&
			!osWorld->osAthenaPlayerPawn->bIsCrouched && !osFortPlayerController->bIsPlayerActivelyMoving)
		{
			auto EmoteAsset = static_cast<AFortPlayerController_ServerPlayEmoteItem_Params*>(pParams)->EmoteAsset;
			auto Montage = EmoteAsset->GetAnimationHardReference(EFortCustomBodyType::All, EFortCustomGender::Both);

			auto AnimInstance = static_cast<AFortPlayerPawnAthena*>(osmium::World::FindActor(AFortPlayerPawnAthena::StaticClass()))->Mesh->GetAnimInstance();
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
			}
			else if (ScriptName == "event")
			{
				UE4_CONSOLE_LOG(L"The event will start in a few minutes. Enjoy your stay ♥")

				auto EventPlayer = UObject::FindObject<ULevelSequencePlayer>(
					"LevelSequencePlayer Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.LevelSequence_LaunchRocket.AnimationPlayer");
				EventPlayer->Play();
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
		return true;

	MessageBoxA(nullptr, "Couldn't hook process event", "Osmium", MB_OK);

	return false;
}
