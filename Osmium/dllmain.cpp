#include "program.h"

static osmium::Program* spProgram;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) 
        spProgram = new osmium::Program;
    else if (dwReason == DLL_PROCESS_DETACH && spProgram)
        delete spProgram;

    return TRUE;
}