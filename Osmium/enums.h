#pragma once

#define UENGINE_OFFSET 0x4FC15D8
#define UWORLD_OFFSET 0x4FC3160
#define SCOI_OFFSET 0x15B5410
#define PE_OFFSET 0x158FB20

enum EWorldStatus
{
	None,
	InLobby,
	Constructing,
	InGame,
	Destructing
};
