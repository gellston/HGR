

#include "hgr/image.h"
#include "hgr/memoryBlock.h"
#include "hgr/memoryPool.h"
#include "hgr/memoryToken.h"

#include "hgr/clipSampler.h"

#include "hgr/hgr.h"


#include <windows.h>

static HMODULE g_hModule = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);
    }
    return TRUE;
}

HMODULE GetThisModule()
{
    return g_hModule;
}
