#pragma once

#include "hook.h"

inline bool isInLobby;

namespace osmium
{
	class cURL 
	{
		Hook* m_pcURLEasySetoptHook;

	public:
		cURL();
		~cURL();
	};
}