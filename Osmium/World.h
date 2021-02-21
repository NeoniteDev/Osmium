#pragma once
#include "framework.h"

namespace osmium
{
	class World
	{
	public:
		BOOL Status;

		bool bIsSprinting;

		bool bHasJumped;
		bool bWantsToJump;
		bool bWantsToSkydive;
		bool bWantsToOpenGlider;

		APlayerController* osPlayerController;
		AFortPlayerPawnAthena* osPlayerPawn;

		World();
		~World();
	};
}