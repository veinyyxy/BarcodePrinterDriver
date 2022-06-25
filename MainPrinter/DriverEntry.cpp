#pragma once
#include "driverhead.h"
#include "DrawInterface.h"

BOOL InitDrvPlugin(HMODULE oldDrvModule);
BOOL InitDrawPlugin(HMODULE hDrvModule, PTCHAR pStrDriverName);
void FreeDrvPluginModule(HMODULE oldDrvModule);
void FreeDrawPluginModule(HMODULE hDrvModule);
BOOL GetMoudlePath(HINSTANCE hInst, PTCHAR pStrModulePath, DWORD dwSize, PTCHAR pStrModuleName = NULL, DWORD dwSize2 = 0);
HMODULE LoadPluginModule(PTCHAR strModuleKey, const PTCHAR pStrModulePath, const PTCHAR configFile);
void GetDriverName(HMODULE hDrvModule, PTCHAR strDriverName, DWORD dwSizeOfByte);

HMODULE g_oldDrvModule = NULL;
HMODULE g_drawModule = NULL;
INT32 g_LoadCount = 0;

void writeLogFile(const char* logFile, const char* format, ...);

BOOL LoadModule(HINSTANCE hInst, PTCHAR pStrDriverName)
{
    if (g_oldDrvModule && g_LoadCount)
    {
        g_LoadCount++;
        return TRUE;
    }
    else
    {
        TCHAR strModulePath[1024] = { 0 };
        TCHAR strConfigFile[256] = { 0 };
        TCHAR strConfigFullFile[256] = { 0 };
        StringCbCatEx(strConfigFullFile, sizeof(strConfigFullFile), TEXT("\\"), NULL, NULL, STRSAFE_NO_TRUNCATION);
        BOOL bRg = GetMoudlePath(hInst, strModulePath, sizeof(strModulePath), strConfigFile, sizeof(strConfigFile));
        StringCbCatEx(strConfigFile, sizeof(strConfigFile), TEXT(".ini"), NULL, NULL, STRSAFE_NO_TRUNCATION);
        StringCbCatEx(strConfigFullFile, sizeof(strConfigFullFile), strConfigFile, NULL, NULL, STRSAFE_NO_TRUNCATION);
        if (!bRg) return FALSE;

        g_oldDrvModule = LoadPluginModule(TEXT("DriverPlugin"), strModulePath, strConfigFullFile);
        bRg = InitDrvPlugin(g_oldDrvModule);
        if (!bRg) return FALSE;

        g_drawModule = LoadPluginModule(TEXT("DrawingPlugin"), strModulePath, strConfigFullFile);
        bRg = InitDrawPlugin(g_drawModule, pStrDriverName);
        if (!bRg) return FALSE;
        g_LoadCount++;
        return TRUE;
    }
}

void UnloadModule()
{
    if(g_LoadCount > 0)
        g_LoadCount--;

    if (g_LoadCount == 0 && g_oldDrvModule && g_drawModule)
    {
        FreeDrvPluginModule(g_oldDrvModule);
        g_oldDrvModule = NULL;
        FreeDrawPluginModule(g_drawModule);
        g_drawModule = NULL;
    }
}

BOOL WINAPI DllMain(HINSTANCE hInst, WORD wReason, LPVOID lpReserved)
{
    BOOL bResult = TRUE;
    //UNREFERENCED_PARAMETER(hInst);
    UNREFERENCED_PARAMETER(lpReserved);

#ifdef _DEBUG
    DWORD dwThreadID = GetCurrentThreadId();
    char buff[255] = { 0 };
    sprintf(buff, "thread ID = %X ,oldDrvModule = %d\n", dwThreadID, wReason);
    writeLogFile("C:\\Users\\C\\Documents\\Projects\\LanxumGDIDriver\\GC2632_Debug2\\log.txt", buff);
#endif // DEBUG

    switch (wReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            TCHAR strDriverName[1024] = { 0 };
            GetDriverName(hInst, strDriverName, sizeof(strDriverName));
            bResult = LoadModule(hInst, strDriverName);
            break;
        }
        case DLL_THREAD_ATTACH:
        {
            //bResult = LoadModule(hInst);
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            UnloadModule();
            break;
        }
        case DLL_THREAD_DETACH:
        {
            //UnloadModule();
            break;
        }
    }
    return bResult;
}