#pragma once
#include "framework.h"

namespace osmium
{
	class World
	{
	public:
		BOOL Status;

		bool bHasJumped;
		bool bIsSprinting;

		AFortPlayerPawn* osFortPlayerPawn;
		APlayerController* osPlayerController;
		AFortPlayerPawnAthena* osAthenaPlayerPawn;

		World();
		~World();

		AActor* FindActor(UClass* pClass);
	};
}