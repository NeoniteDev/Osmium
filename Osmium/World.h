#pragma once
#include "framework.h"

namespace osmium
{
	class World
	{
	public:
		BOOL Status;
		APlayerController* osPlayerController;

		World();
		~World();
	};
}