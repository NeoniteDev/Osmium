#pragma once
#include "framework.h"

namespace osmium 
{
	class Hook 
	{
	private:
		static uintptr_t m_pTarget;
		static uintptr_t m_pDetour;

		PVOID m_hHandle;
		DWORD m_dwProtect;

	public:
		BOOL bSuccess;

		Hook(uintptr_t pTarget, uintptr_t pDetour);
		~Hook();

	private:
		static LONG WINAPI Handler(EXCEPTION_POINTERS* pExceptionInfo);
		static BOOL IsSamePage(const PBYTE pFirstAddr, const PBYTE pSecondAddr);
	};
}