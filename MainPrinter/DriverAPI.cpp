#include "driverhead.h"

class DrawInterface;
typedef DrawInterface* (*Fun_GetDrawObject)();
typedef void (*Fun_ReleaseDrawObject)();

void HookPrint(DrawInterface* dh, PTCHAR pStrDriverName);
void UnhookPrint();
TCHAR g_strDriverName[1024] = { 0 };

typedef BOOL(*FUN_DrvEnableDriver)(
    ULONG         iEngineVersion,
    ULONG         cj,
    DRVENABLEDATA* pded
    );

typedef void (*FUN_DrvDisableDriver)();

typedef BOOL(*FUN_DrvQueryDriverInfo)(
    DWORD  dwMode,
    PVOID  pBuffer,
    DWORD  cbBuf,
    PDWORD pcbNeeded
    );

FUN_DrvEnableDriver oldDrvEnableDriver = nullptr;
FUN_DrvDisableDriver oldDrvDisableDriver = nullptr;
FUN_DrvQueryDriverInfo oldDrvQueryDriverInfo = nullptr;

BOOL GetConfig(const PTCHAR lpAppName, P_DRV_CONFIG_ITEM drvConfigItem
    , const PTCHAR pStrModulePath, const PTCHAR configFile)
{
    TCHAR configFilePath[1024] = { 0 };
    size_t bufferSize = sizeof(configFilePath);
    StringCchCopy(configFilePath, bufferSize, pStrModulePath);
    //StrCat(configFilePath, configFile);
    StringCbCatEx(configFilePath, bufferSize, configFile, NULL, NULL, STRSAFE_NO_TRUNCATION);
    DWORD dwLen1 = GetPrivateProfileString(lpAppName, TEXT("FileName"), TEXT(""), drvConfigItem->module_path
        , sizeof(drvConfigItem->module_path), configFilePath);
    DWORD dwLen2 = GetPrivateProfileString(lpAppName, TEXT("FindMode"), TEXT(""), drvConfigItem->find_mode
        , sizeof(drvConfigItem->find_mode), configFilePath);
    if (dwLen1 != 0 && dwLen2 != 0)
        return TRUE;
    else
        return FALSE;
}

BOOL GetMoudlePath(HINSTANCE hInst, PTCHAR pStrModulePath, DWORD dwSize, PTCHAR pStrModuleName, DWORD dwSize2)
{
#ifdef _MBCS
#define CHARACTER_BYTE_SIZE 1
#else
#define CHARACTER_BYTE_SIZE 2
#endif

    TCHAR wSplit = TEXT('\\');
    TCHAR dynamicFilePath[2048] = { 0 };
    DWORD dwStrLen = GetModuleFileName(hInst, dynamicFilePath, sizeof(dynamicFilePath));
    if (dwStrLen)
    {
        PTCHAR posAddress = StrRChr(dynamicFilePath, NULL, wSplit);
        ULONG iPos = static_cast<ULONG>(posAddress - dynamicFilePath);

        if (pStrModuleName != NULL && dwSize2 != 0)
        {
            if (dwSize2 / CHARACTER_BYTE_SIZE > dwStrLen - iPos + 1)
            {
                StrCpyN(pStrModuleName, dynamicFilePath + iPos + 1, dwStrLen - iPos - 4);//The pStrModuleName only contain dll's name except 'Extension Name'.
            }
        }
        ULONG iCount = iPos + 1;//It is character's count.
        if (dwSize / CHARACTER_BYTE_SIZE < iCount + 1) return FALSE;

        StrCpyN(pStrModulePath, dynamicFilePath, iCount);
        return TRUE;
    }
    return FALSE;
}

HMODULE LoadPluginModule(PTCHAR strModuleKey, const PTCHAR pStrModulePath, const PTCHAR configFile)
{
    HMODULE hModule = NULL;
    DWORD dwStrLen = lstrlen(pStrModulePath);
    if (dwStrLen)
    {
        PTCHAR oldDllName = NULL;
        PTCHAR pathMode = NULL;

        DRV_CONFIG_ITEM drvConfigItem;

        if (GetConfig(strModuleKey, &drvConfigItem, pStrModulePath, configFile))
        {
            oldDllName = drvConfigItem.module_path;
            pathMode = drvConfigItem.find_mode;

            if (StrCmp(pathMode, TEXT("absolutely")) == 0)
            {
                hModule = LoadLibrary(oldDllName);
            }
            else
            {
                TCHAR thisDrivePath[1024] = { 0 };
                size_t bufferSize = sizeof(thisDrivePath);
                StringCbCatEx(thisDrivePath, bufferSize, pStrModulePath, NULL, NULL, STRSAFE_NO_TRUNCATION);
                StringCbCatEx(thisDrivePath, bufferSize, TEXT("\\"), NULL, NULL, STRSAFE_NO_TRUNCATION);
                StringCbCatEx(thisDrivePath, bufferSize, oldDllName, NULL, NULL, STRSAFE_NO_TRUNCATION);
                hModule = LoadLibrary(thisDrivePath);
            }
        }
    }
    return hModule;
}

BOOL InitDrawPlugin(HMODULE hDrvModule, PTCHAR pStrDriverName)
{
    Fun_GetDrawObject GetDrawObject = (Fun_GetDrawObject)GetProcAddress(hDrvModule, "GetDrawObject");
    if (GetDrawObject)
    {
        DrawInterface* pDrawInterface = GetDrawObject();
        if (pDrawInterface)
        {
            HookPrint(pDrawInterface, pStrDriverName);
            return TRUE;
        }
        else
            return FALSE;
    } 
    else
        return FALSE;
}

void GetDriverName(HMODULE hDrvModule, PTCHAR strDriverName, DWORD dwSizeOfByte)
{
    TCHAR dynamicFilePath[1024] = { 0 };
    TCHAR infFilePathName[2048] = { 0 };
    DWORD dwStrLen = GetModuleFileName(hDrvModule, dynamicFilePath, sizeof(dynamicFilePath));
    
    DWORD dwFrontLen = dwStrLen - 4 + 1;
    StrCpyN(infFilePathName, dynamicFilePath, dwFrontLen);
    StrCat(infFilePathName, TEXT(".ini"));

    GetPrivateProfileString(TEXT("DriverInfo"), TEXT("Name"), TEXT(""), strDriverName, dwSizeOfByte
        , infFilePathName);
    DWORD dwResult = GetLastError();
    printf("%d", dwResult);
}

void FreeDrvPluginModule(HMODULE hDrvModule)
{
    FreeLibrary(hDrvModule);
    hDrvModule = 0;
}

void FreeDrawPluginModule(HMODULE hDrvModule)
{
    UnhookPrint();
    FreeLibrary(hDrvModule);
    hDrvModule = 0;
}

BOOL InitDrvPlugin(HMODULE hDrvModule)
{
    if (hDrvModule)
    {
        oldDrvEnableDriver = (FUN_DrvEnableDriver)GetProcAddress(hDrvModule, "DrvEnableDriver");
        oldDrvDisableDriver = (FUN_DrvDisableDriver)GetProcAddress(hDrvModule, "DrvDisableDriver");
        oldDrvQueryDriverInfo = (FUN_DrvQueryDriverInfo)GetProcAddress(hDrvModule, "DrvQueryDriverInfo");

        if (oldDrvEnableDriver && oldDrvDisableDriver && oldDrvQueryDriverInfo)
        {
            return TRUE;
        }
        else
            return FALSE;
    }
    return FALSE;
}

BOOL DrvEnableDriver(ULONG iEngineVersion, ULONG cj, _In_reads_bytes_(cj) DRVENABLEDATA* pded)
{
    if(oldDrvEnableDriver)
        return oldDrvEnableDriver(iEngineVersion, cj, pded);
    else
        return FALSE;
}

void DrvDisableDriver()
{
    if(oldDrvDisableDriver)
        return oldDrvDisableDriver();
}

BOOL APIENTRY
APIENTRY
DrvQueryDriverInfo(
    DWORD   dwMode,
    _Out_writes_bytes_(cbBuf) PVOID   pBuffer,
    DWORD   cbBuf,
    _Out_writes_(1) PDWORD  pcbNeeded
)
{
    switch (dwMode)
    {
    case DRVQUERY_USERMODE:
        *pcbNeeded = sizeof(DWORD);

        if (pBuffer == NULL || cbBuf < sizeof(DWORD))
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        *((PDWORD)pBuffer) = TRUE;
        return TRUE;

    default:
        *pcbNeeded = 0;
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
}

void writeLogFile(const char* logFile, const char* format, ...)
{
    FILE* pLogFile = fopen(logFile, "a");
    va_list args;
    const char* args1;
    va_start(args, format);
    args1 = va_arg(args, const char*);
    va_end(args);
    fprintf(pLogFile, format, args1);
    //printf("format=%s args1=%s", format, args1);
    fclose(pLogFile);
}