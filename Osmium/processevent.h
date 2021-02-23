﻿#pragma once
#include "framework.h"
#include "globals.h"
#include "curl.h"
#include "World.h"


//Used to log processevent
#define LOGGING

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
		auto PlayerController = static_cast<AFortPlayerControllerAthena*>(GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController);

		if (PlayerController->IsInAircraft())
		{
			osWorld->Despawn();
			osWorld->Spawn();

			auto Pawn = static_cast<AFortPlayerPawnAthena*>(osmium::World::FindActor(AFortPlayerPawnAthena::StaticClass()));
			if (Pawn)
			{
				auto PlayerState = static_cast<AFortPlayerState*>(Pawn->PlayerState);

				if (PlayerState)
				{
					MessageBoxA(nullptr, "PlayerState is valid.", "Osmium", MB_OK);
				}
			}
		}				
	}

	if (nFunc == "Event AcceptOption")
	{
		auto PlayerController = static_cast<AFortPlayerControllerAthena*>(GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController);
		auto Dance = PlayerController->CustomizationLoadout.Dances[0];
		auto Montage = Dance->GetAnimationHardReference(EFortCustomBodyType::All, EFortCustomGender::Both);
		auto AnimInstance = static_cast<AFortPlayerPawnAthena*>(osmium::World::FindActor(AFortPlayerPawnAthena::StaticClass()))->Mesh->GetAnimInstance();
		AnimInstance->Montage_Play(Montage, 1, EMontagePlayReturnType::Duration, 0, true);
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
	printf("[Object]: %s [Function]: %s\n", nObj.c_str(), nFunc.c_str());
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
