#pragma once
#include "framework.h"
#include "processevent.h"

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
	/// <summary>
	/// Initializes CheatManager for the player.
	/// </summary>
	inline void InitCheatManager()
	{
		// Get the PlayerController from the GameInstances' LocalPlayers array.
		auto PlayerController = GEngine->GameViewport->GameInstance->LocalPlayers[0]->PlayerController;

		// Construct a CheatManager.
		UObject* CheatManager = StaticConstructObject(
			UCheatManager::StaticClass(),
			PlayerController,
			nullptr, 0, 0, nullptr,
			false, nullptr, false
		);

		// Assign the CheatManager to the PlayerController.
		PlayerController->CheatManager = static_cast<UCheatManager*>(CheatManager);
	}

	/// <summary>
	/// Unlocks the console and initializes CheatManager for the player.
	/// </summary>
	inline void UnlockConsole()
	{
		// Construct a Console.
		UObject* Console = StaticConstructObject(
			GEngine->ConsoleClass,
			GEngine->GameViewport,
			nullptr, 0, 0, nullptr, 
			false, nullptr, false
		);

		// Assign the constructed console to the GameViewports' ViewportConsole.
		GEngine->GameViewport->ViewportConsole = static_cast<UConsole*>(Console);

		// Initialize CheatManager for the player.
		InitCheatManager();
	}

	/// <summary>
	/// Initializes Osmium.
	/// </summary>
	inline void Init()
	{
		const auto ModuleBase = Util::BaseAddress();

		GEngine = *reinterpret_cast<UEngine**>(ModuleBase + UENGINE_OFFSET);
		GWorld = *reinterpret_cast<UWorld**>(ModuleBase + UWORLD_OFFSET);

		StaticConstructObject = decltype(StaticConstructObject)(ModuleBase + SCOI_OFFSET);

		if (InitPEH())
		{
			osWorldStatus = InLobby;
			UnlockConsole();
		}
	}
}
