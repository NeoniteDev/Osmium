#pragma once

#include "hook.h"

namespace osmium
{
	class Curl 
	{
	private:
		Hook* m_pCurlEasySetoptHook;

	public:
		Curl();
		~Curl();
	};
}