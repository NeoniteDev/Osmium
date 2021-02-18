#pragma once

namespace Patterns
{
	namespace Curl
	{
		constexpr const char* CurlEasySetOpt = "\x89\x54\x24\x10\x4C\x89\x44\x24\x18\x4C\x89\x4C\x24\x20\x48\x83\xEC\x28\x48\x85\xC9\x75\x08\x8D\x41\x2B\x48\x83\xC4\x28\xC3\x4C";
		constexpr const char* CurlSetOpt = "\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x30\x33\xED\x49\x8B\xF0\x48\x8B\xD9";
	}
}

namespace Masks
{
	namespace Curl
	{
		constexpr const char* CurlEasySetOpt = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		constexpr const char* CurlSetOpt = "xxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	}	
}

namespace Strings
{
	namespace Maps
	{
		constexpr const char* Frontend = "Frontend_Lobby_Default?game=/Script/FortniteGame.FortGameModeFrontEnd";
		constexpr const char* AthenaTerrain = "Athena_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
		constexpr const char* AthenaTerrainBase = "Athena_Terrain?game=/Script/FortniteGame.FortGameModeBase";
	}
	namespace Curl
	{
		constexpr const char* HostURL = "https://aurora-staging-api-2.slushia.dev";
	}
}