#include <regex>
#include <string>
#include <windows.h>

#include "curl.h"
#include "util.h"
#include "constants.h"
#include "framework.h"

namespace osmium 
{
	const std::regex rEpicGames("https:\\/\\/(.*)\\.ol\\.epicgames.com");

	INT (*CurlSetopt)(LPVOID, INT, va_list) = NULL;
	INT CurlSetoptVa(LPVOID lpContext, INT iOption, ...) 
	{
		va_list list{};
		va_start(list, iOption);

		INT iResult = CurlSetopt(lpContext, iOption, list);

		va_end(list);

		return iResult;
	}

	INT CurlEasySetopt(LPVOID lpContext, INT iTag, ...) 
	{
		if (!lpContext)
			return 43; // CURLE_BAD_FUNCTION_ARGUMENT

		va_list list{}, copy{}; // Copy only exists for our tag overrides.
		va_start(list, iTag);
		va_copy(copy, list);

		INT iResult = 0;
		switch (iTag) 
		{
			case 64: // CURLOPT_SSL_VERIFYPEER
				iResult = CurlSetoptVa(lpContext, iTag, FALSE); // Disables VerifyPeer.
				break;

			case 10002: // CURLOPT_URL
			{
				std::string sUrl(va_arg(copy, PCHAR));

				// Check if the URLs host is EpicGames.
				if (std::regex_search(sUrl, rEpicGames))
					sUrl = std::regex_replace(sUrl, rEpicGames, Strings::Curl::HostURL);

				iResult = CurlSetoptVa(lpContext, iTag, sUrl.c_str());
				break;
			}

			default: // Everything else.
				iResult = CurlSetopt(lpContext, iTag, list);
				break;
		}

		va_end(copy);
		va_end(list);

		return iResult;
	}

	Curl::Curl() 
	{
		auto pCurlSetoptAddress = Util::FindPattern(Patterns::Curl::CurlSetOpt, Masks::Curl::CurlSetOpt);
		if (!pCurlSetoptAddress) 
		{
			printf("Finding pattern for CurlSetopt has failed, exiting immediately!\n");
			exit(EXIT_FAILURE);
		}

		CurlSetopt = reinterpret_cast<decltype(CurlSetopt)>(pCurlSetoptAddress);

		auto pCurlEasySetoptAddress = Util::FindPattern(Patterns::Curl::CurlEasySetOpt, Masks::Curl::CurlEasySetOpt);
		if (!pCurlEasySetoptAddress) 
		{
			printf("Finding pattern for CurlEasySetopt has failed, exiting immediately!\n");
			exit(EXIT_FAILURE);
		}

		m_pCurlEasySetoptHook = new Hook(reinterpret_cast<uintptr_t>(pCurlEasySetoptAddress), reinterpret_cast<uintptr_t>(CurlEasySetopt));
		if (!m_pCurlEasySetoptHook->bSuccess) 
		{
			printf("Initializing hook for CurlEasySetopt has failed, exiting immediately!\n");
			exit(EXIT_FAILURE);
		}
	}
	Curl::~Curl() 
	{
		if (m_pCurlEasySetoptHook)
			delete m_pCurlEasySetoptHook;
	}
}