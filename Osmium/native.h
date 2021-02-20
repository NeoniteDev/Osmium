#pragma once
#include "framework.h"

using namespace osmium;

inline UWorld* GWorld;
inline UEngine* GEngine;
inline UObject* (*StaticConstructObject)(
	UClass* Class,
	UObject* InOuter,
	void* Name,
	int SetFlags,
	unsigned int InternalSetFlags,
	UObject* Template,
	bool bCopyTransientsFromClassDefaults,
	void* InstanceGraph,
	bool bAssumeTemplateIsArchetype
);


namespace Native
{
	inline void CheatManager()
	{
		/*
		 * Gets the local player controller from the local players array in the gameinstance
		 * Then Constructs a cheatmanager then assigns it to the player
		 */
		auto PlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;
		
		UObject* CheatManager = StaticConstructObject(
			UCheatManager::StaticClass(),
			PlayerController,
			nullptr,
			0,
			0,
			nullptr,
			false,
			nullptr,
			false
		);

		PlayerController->CheatManager = static_cast<UCheatManager*>(CheatManager);
	}

	//Unlocks console with cheat manager
	inline void UnlockConsole()
	{
		//we construct a console then we assign it to the viewport console
		UObject* Console = StaticConstructObject(
			GEngine->ConsoleClass,
			GEngine->GameViewport,
			nullptr,
			0,
			0,
			nullptr,
			false,
			nullptr,
			false
		);

		GEngine->GameViewport->ViewportConsole = static_cast<UConsole*>(Console);

		CheatManager();
	}

	inline void Init()
	{
		GEngine = *reinterpret_cast<UEngine**>(reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)) + 0x4FC15D8);

		StaticConstructObject = decltype(StaticConstructObject)(reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)) + 0x15B5410);

		GWorld = *reinterpret_cast<UWorld**>(reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)) + 0x4FC3160);

		UnlockConsole();
	}
}
