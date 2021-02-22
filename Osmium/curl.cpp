#include "cURL.h"
#include "util.h"
#include "constants.h"
#include "veh.h"

namespace osmium 
{
	const std::regex rEpicGames("https:\\/\\/(.*)\\.ol\\.epicgames.com");

	INT (*cURLSetopt)(LPVOID, INT, va_list) = NULL;
	INT cURLSetoptVa(LPVOID lpContext, INT iOption, ...) 
	{
		va_list list{};
		va_start(list, iOption);

		INT iResult = cURLSetopt(lpContext, iOption, list);

		va_end(list);

		return iResult;
	}

	INT cURLEasySetopt(LPVOID lpContext, INT iTag, ...) 
	{
		if (!lpContext)
			return 43; // cURLE_BAD_FUNCTION_ARGUMENT

		va_list list{}, copy{}; // Copy only exists for our tag overrides.
		va_start(list, iTag);
		va_copy(copy, list);

		INT iResult = 0;
		switch (iTag) 
		{
			case 64: // cURLOPT_SSL_VERIFYPEER
				iResult = cURLSetoptVa(lpContext, iTag, FALSE); // Disables VerifyPeer.
				break;

			case 10002: // cURLOPT_URL
			{
				std::string sUrl(va_arg(copy, PCHAR));

				if (sUrl.find("ClientQuestLogin") != std::string::npos) isInLobby = !isInLobby;
				if (sUrl.find("matchmakingservice") != std::string::npos) hasStarted = !hasStarted;

				// Check if the URLs host is EpicGames.
				if (std::regex_search(sUrl, rEpicGames))
					sUrl = std::regex_replace(sUrl, rEpicGames, Strings::cURL::HostURL);

				iResult = cURLSetoptVa(lpContext, iTag, sUrl.c_str());
				break;
			}

			default: // Everything else.
				iResult = cURLSetopt(lpContext, iTag, list);
				break;
		}

		va_end(copy);
		va_end(list);

		return iResult;
	}

	cURL::cURL() 
	{
		auto pcURLSetoptAddress = Util::FindPattern(Patterns::cURL::cURLSetOpt, Masks::cURL::cURLSetOpt);
		VALIDATE_ADDRESS(pcURLSetoptAddress, "Finding pattern for cURLSetopt has failed, exiting immediately!\n")
		cURLSetopt = reinterpret_cast<decltype(cURLSetopt)>(pcURLSetoptAddress);

		auto pcURLEasySetoptAddress = Util::FindPattern(Patterns::cURL::cURLEasySetOpt, Masks::cURL::cURLEasySetOpt);		
		VALIDATE_ADDRESS(pcURLEasySetoptAddress, "Finding pattern for cURLEasySetopt has failed, exiting immediately!\n")

		VEH::EnableHook(pcURLEasySetoptAddress, cURLEasySetopt);
		
		return;
	}
	
	cURL::~cURL() 
	{
	}
}
