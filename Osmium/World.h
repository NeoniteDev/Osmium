#pragma once
#include "framework.h"

namespace osmium
{
	class World
	{
	public:
		bool bHasJumped;
		bool bIsSprinting;

		AFortPlayerPawn* osFortPlayerPawn;
		APlayerController* osPlayerController;
		AFortPlayerPawnAthena* osAthenaPlayerPawn;

		World();
		~World();

		static UObject* FindActor(UClass* pClass);
	};
}