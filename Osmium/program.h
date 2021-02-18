#pragma once

#include "curl.h"

namespace osmium
{
	class Program 
	{
	private:
		Curl* m_pCurl;

	public:
		Program();
		~Program();
	};
}