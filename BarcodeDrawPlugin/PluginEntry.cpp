#include "Plugin.h"
#include "DrawBarcode.h"

class DrawBarcode;

DrawBarcode* g_pDrawInterface = NULL;

DrawInterface* GetDrawObject()
{
    if (g_pDrawInterface)
        return g_pDrawInterface;
    else
    {
        g_pDrawInterface = new DrawBarcode;
        return g_pDrawInterface;
    }
}

void ReleaseDrawObject()
{
    if (g_pDrawInterface)
    {
        delete g_pDrawInterface;
        g_pDrawInterface = NULL;
    }
}

BOOL WINAPI DllMain(HINSTANCE hInst, WORD wReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hInst);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (wReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            break;
        }
        case DLL_THREAD_ATTACH:
        {
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
        case DLL_THREAD_DETACH:
        {
            break;
        }
    }
    return TRUE;
}