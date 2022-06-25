// TestDllLoad.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <winddi.h>

typedef BOOL (*FUN_DrvEnableDriver)(
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
void writeReport(const wchar_t* szMsg);

void writeLogFile(const char* text)
{
    FILE* pLogFile = fopen("C:\\Users\\C\\Documents\\Projects\\TestDllLoad\\log\\log.txt", "a");
    fprintf(pLogFile, "%s\n", text);
    //fflush(pLogFile);
    fclose(pLogFile);
}

BOOL setDrvFnTable(ULONG ulID, PFN pFN, PDRVENABLEDATA pDrvEnableData)
{
    PFN newFN = pFN;
    for (ULONG i = 0; i < pDrvEnableData->c; i++)
    {
        if (pDrvEnableData->pdrvfn[i].iFunc == ulID)
        {
            pDrvEnableData->pdrvfn[i].pfn = pFN;
            //pDrvEnableData->pdrvfn[i].iFunc = 1234;
            return TRUE;
        }
    }

    return FALSE;
}

PFN getDrvFnFromTable(ULONG ulID, PDRVENABLEDATA pDrvEnableData)
{
    for (ULONG i = 0; i < pDrvEnableData->c; i++)
    {
        if (pDrvEnableData->pdrvfn[i].iFunc == ulID)
        {
            return pDrvEnableData->pdrvfn[i].pfn;
        }
    }

    return NULL;
}

int main()
{
    //writeLogFile("This is my testing message.");
    //HMODULE pModule = LoadLibrary(L"C:\\Windows\\system32\\WINSPOOL.DRV");
    //HMODULE pModule = LoadLibrary(L"C:\\Windows\\System32\\spool\\drivers\\x64\\3\\UNIDRV.DLL");
    //HMODULE pModule = LoadLibrary(TEXT("C:\\Users\\C\\Documents\\Projects\\sp-client-driver-windows\\trunk\\GC3342dn\\Lanxum\\hxpl2zxp.dll"));
    //HMODULE pModule = LoadLibrary(TEXT("C:\\Users\\C\\Documents\\Projects\\sp-client-driver-windows\\trunk\\GC2632dn\\Lanxum\\x64\\TZo1t0dr.dll"));
    HMODULE pModule = LoadLibrary(L"LanxumPrinter.dll");
    DWORD dwError = GetLastError();
    FUN_DrvEnableDriver DrvEnableDriver = nullptr;
    FUN_DrvDisableDriver DrvDisableDriver = nullptr;
    FUN_DrvQueryDriverInfo DrvQueryDriverInfo = nullptr;

    if (pModule)
    {
        DRVENABLEDATA pded = { 0 };
        DrvEnableDriver = (FUN_DrvEnableDriver)GetProcAddress(pModule, "DrvEnableDriver");
        if (DrvEnableDriver(DDI_DRIVER_VERSION_NT5_01_SP1, sizeof(DRVENABLEDATA), &pded))
        {
            ULONG funCount = pded.c;
            for (ULONG i = 0; i < funCount; i++)
            {
                DRVFN pDrvfun = pded.pdrvfn[i];
                std::cout << pDrvfun.iFunc << ":" << pDrvfun.pfn <<std::endl;
            }
        }

        std::cout << getDrvFnFromTable(0, &pded) << std::endl;
        DrvDisableDriver = (FUN_DrvDisableDriver)GetProcAddress(pModule, "DrvDisableDriver");
        DrvQueryDriverInfo = (FUN_DrvQueryDriverInfo)GetProcAddress(pModule, "DrvQueryDriverInfo");
    }

    FreeLibrary(pModule);

    return 0;
}

void writeReport( const wchar_t* szMsg)
{
    const wchar_t* SVCNAME = L"GC2632 Printer Driver";
    HANDLE hEventSource;
    DWORD dwEventIdentifer = 1;
    DWORD dwValue = 0;
    DWORD* pValue = &dwEventIdentifer;
    // ǰ�������wErrorType�������¼���������ʽ��֯����
    int wErrorType = EVENTLOG_INFORMATION_TYPE;

    switch (wErrorType)
    {
    case EVENTLOG_SUCCESS:
        dwEventIdentifer = 0x00;
        break;
    case EVENTLOG_INFORMATION_TYPE:
        dwEventIdentifer = 0x01;
        break;
    case EVENTLOG_WARNING_TYPE:
        dwEventIdentifer = 0x02;
        break;
    case EVENTLOG_ERROR_TYPE:
        dwEventIdentifer = 0x03;
        break;
    default:
        break;
    }

    dwEventIdentifer <<= 30;// ��λ���Sev��ǰ������� wErrorType Ϊ EVENTLOG_ERROR_TYPE����Ӧ����ͼ ������ һ����ʾ������ͼ��
    dwValue = 1;
    dwEventIdentifer |= (dwValue << 29);

    //dwEventIdentifer |= (WORD)wCustumerCode; // ǰ���Զ�����Code����Ӧ����ͼ�� �¼�ID 20

    hEventSource = RegisterEventSource(NULL, SVCNAME); // �� SVCNAME �����˷��������

    if (NULL != hEventSource)
    {

        LPCTSTR lpszStrings[2] = { SVCNAME, szMsg };//Ҫд����־����Ϣ�����У��ֱ��� ����������ǰ�������szMsg����Ӧ����ͼ�������ǰ������¼��е���Ϣ��

        ReportEvent(
            hEventSource,      // event log handle
            wErrorType,        // event type
            0,                 // event category
            dwEventIdentifer,  // event identifier
            NULL,              // no security identifier
            2,                 // size of lpszStrings array
            0,                 // no binary data
            lpszStrings,       // array of strings
            NULL);             // no binary data
        DeregisterEventSource(hEventSource);
    }
}