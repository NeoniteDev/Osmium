#pragma once
#include "framework.h"
#include "globals.h"
#include "curl.h"
#include "World.h"


//Used to log processevent
#ifndef _PROD
#define LOGGING
#endif

inline void* (*ProcessEvent)(void* pUObject, void* pUFunction, void* pParams);

/***************************(NOTE) kemo*****************************
 * This is a processevent hook, we usually don't call it at all;
 * But instead We call the native function directly so our call don't
 * get hijacked, Process Event Hook is used to track
 * what function is called and have a time window to
 * manipulate/edit it or do something before it's actaully called,
 * usually after we are done with the manipulation we do "goto out"
 * to jump to the end and return the params directly.
 ******************************************************************/
inline void* ProcessEventDetour(UObject* pObj, UObject* pFunc, void* pParams)
{
	const auto nObj = pObj->GetName();
	const auto nFunc = pFunc->GetName();

	if (nFunc == "Tick" && osWorldStatus)
	{
		if (osWorldStatus != InLobby)
		{
			auto PlayerController = static_cast<AFortPlayerController*>(osWorld->osPlayerController);
			if (PlayerController->bIsPlayerActivelyMoving)
			{
				auto AnimInstance = static_cast<AFortPlayerPawnAthena*>(osmium::World::FindActor(AFortPlayerPawnAthena::StaticClass()))->Mesh->GetAnimInstance();
				auto CurrentMontage = AnimInstance->GetCurrentActiveMontage();

				if (CurrentMontage)
					if (CurrentMontage->GetName().starts_with("Emote_") || CurrentMontage->GetName().starts_with("Basketball_CMM"))
						AnimInstance->Montage_Stop(1, CurrentMontage);
			}

			bool wantsToSprint = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController)->bHoldingSprint;

			if (wantsToSprint)
				osWorld->osAthenaPlayerPawn->CurrentMovementStyle = EFortMovementStyle::Sprinting;
			else
				osWorld->osAthenaPlayerPawn->CurrentMovementStyle = EFortMovementStyle::Running;

			if (GetAsyncKeyState(VK_SPACE))
			{
				if (osWorld->bHasJumped == false)
				{
					osWorld->bHasJumped = true;

					bool isInAircraft = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController)->IsInAircraft();
					if (!isInAircraft)
					{
						if (!osWorld->osAthenaPlayerPawn->IsParachuteForcedOpen())
						{
							if (osWorld->osAthenaPlayerPawn->IsSkydiving() && !osWorld->osAthenaPlayerPawn->IsParachuteOpen())
								osWorld->osAthenaPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 3);
							else if (osWorld->osAthenaPlayerPawn->IsParachuteOpen())
								osWorld->osAthenaPlayerPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 4);
						}

						osWorld->osAthenaPlayerPawn->OnRep_IsParachuteOpen(osWorld->osAthenaPlayerPawn->IsParachuteOpen()); 

						osWorld->osAthenaPlayerPawn->Jump();
					}
				}
			}
			else osWorld->bHasJumped = false;
		}
	}

	if (gUrl.find("matchmakingservice") != std::string::npos)
	{
		gUrl.clear();

		auto PlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;
		PlayerController->SwitchLevel(Strings::Maps::AthenaTerrain);
		goto out;
	}

	if (nFunc == "ReadyToStartMatch" && osWorldStatus == InLobby)
	{
		osWorld = new osmium::World();
		osWorldStatus = Constructing;
	}

	if (nFunc == "ServerAttemptAircraftJump" || nFunc == "OnAircraftExitedDropZone")
	{
		auto AthenaPlayerController = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController);
		if (AthenaPlayerController->IsInAircraft()) osWorld->Respawn();
	}

	if (nFunc == "Event AcceptOption")
	{
		auto PlayerController = static_cast<AFortPlayerControllerAthena*>(osWorld->osPlayerController);
		auto Dance = PlayerController->CustomizationLoadout.Dances[0];
		auto Montage = Dance->GetAnimationHardReference(EFortCustomBodyType::All, EFortCustomGender::Both);
		auto AnimInstance = static_cast<AFortPlayerPawnAthena*>(osmium::World::FindActor(AFortPlayerPawnAthena::StaticClass()))->Mesh->GetAnimInstance();
		AnimInstance->Montage_Play(Montage, 1, EMontagePlayReturnType::Duration, 0, true);
	}

	if (nObj == "PlayerPawn_Athena_C_2" && nFunc == "OnLanded")
	{
		osWorld->Respawn();
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

	if (nFunc == "ServerAttemptInteract") return nullptr;
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
	const auto ProcessEventAdd = osmium::Util::BaseAddress() + PE_OFFSET;

	ProcessEvent = decltype(ProcessEvent)(ProcessEventAdd);

	DetourTransactionBegin();

	DetourAttach(reinterpret_cast<void**>(&ProcessEvent), ProcessEventDetour);

	const auto error = DetourTransactionCommit();

	if (error == NO_ERROR)
	{
		return true;
	}

	MessageBoxA(nullptr, "Couldn't hook process event", "Osmium", MB_OK);

	return false;
}
