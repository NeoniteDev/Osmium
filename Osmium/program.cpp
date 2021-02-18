#include "util.h"
#include "program.h"

namespace osmium
{
	Program::Program() 
	{
		Util::InitConsole();

		printf("LogOsmium: Init!\n");

		m_pCurl = new Curl;
	}
	Program::~Program() 
	{
		if (m_pCurl)
			delete m_pCurl;
	}
}
