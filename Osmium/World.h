#pragma once
#include "framework.h"

namespace osmium
{
	class World
	{
	public:
		bool bHasJumped = false;

		AFortPlayerPawn* osFortPlayerPawn;
		APlayerController* osPlayerController;
		AFortPlayerPawnAthena* osAthenaPlayerPawn;

		World();
		~World();

		static UObject* FindActor(UClass* pClass);

		void Spawn();
		void Respawn();
		void Despawn();
	};
}