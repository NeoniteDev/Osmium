#pragma once
#include "framework.h"

namespace osmium
{
	class World
	{
	public:
		bool bHasJumped = false;

		AFortPlayerPawn* osFortPlayerPawn;
		UFortAnimInstance* osFortAnimInstance;
		APlayerController* osPlayerController;
		AFortPlayerController* osFortPlayerController;
		AFortPlayerControllerAthena* osFortPlayerControllerAthena;
		AFortPlayerPawnAthena* osAthenaPlayerPawn;

		World();
		~World();

		static UObject* FindActor(UClass* pClass);

		void Spawn();
		void Respawn();
		void Despawn();
		void Tick();
		void EquipPickaxe();
	};
}