#pragma once
#include "framework.h"
#include "native.h"
#include "globals.h"
#include "curl.h"

//Used to log processevent
#define LOGGING

inline void* (*ProcessEvent)(void* pUObject, void* pUFunction, void* pParams);

/***************************(NOTE) kemo*****************************
 * This is a processevent hook, we usually don't call it at all;
 * But instead We call the native function directly so our call don't
 * get hijacked, Process Event Hook is used to track
 * what function is called and have a time window to
 * manipulate/edit it or do something before it's actaully called,
 * usually after we are done with the manipulation we do "goto out"
 * to jump to the end and return the params directly.
 ******************************************************************/
inline void* ProcessEventDetour(UObject* pObj, UObject* pFunc, void* pParams)
{
	const auto nObj = pObj->GetName();
	const auto nFunc = pFunc->GetName();

	if (isInLobby && nFunc == "ReadyToStartMatch" && osWorldStatus == InLobby)
	{
		osWorld = new osmium::World();
		osWorldStatus = Constructing;
	}


out:
#ifdef LOGGING
	printf("[Object]: %s [Function]: %s\n", nObj.c_str(), nFunc.c_str());
#endif

	return ProcessEvent(pObj, pFunc, pParams);
}

inline bool InitPEH()
{
	const auto ProcessEventAdd = osmium::Util::BaseAddress() + PE_OFFSET;

	ProcessEvent = decltype(ProcessEvent)(ProcessEventAdd);

	DetourTransactionBegin();

	DetourAttach(reinterpret_cast<void**>(&ProcessEvent), ProcessEventDetour);

	const auto error = DetourTransactionCommit();

	if (error == NO_ERROR)
	{
		return true;
	}

	MessageBoxA(nullptr, "Couldn't hook process event", "Osmium", MB_OK);

	return false;
}
