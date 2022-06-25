#include "HookGDIPrintAPI.h"
#include "DrawInterface.h"

TCHAR g_NeedDriverName[1024] = { 0 };
BOOL g_bEnableBarcode = FALSE;
/*----GDI funciton----*/
int (WINAPI* TrueStartDocA)(HDC hdc, CONST DOCINFOA* lpdi) = StartDocA;
int (WINAPI* TrueStartDocW)(HDC hdc, CONST DOCINFOW* lpdi) = StartDocW;
int (WINAPI* TrueEndDoc)(HDC hdc) = EndDoc;

int (WINAPI* TrueStartPage)(HDC hdc) = StartPage;
int (WINAPI* TrueEndPage)(HDC hdc) = EndPage;
/*----winspool function----*/
BOOL (WINAPI *TrueOpenPrinterA)(
    _In_opt_    LPSTR             pPrinterName,
    _Out_       LPHANDLE            phPrinter,
    _In_opt_    LPPRINTER_DEFAULTSA pDefault
) = OpenPrinterA;
BOOL (WINAPI *TrueOpenPrinterW)(
    _In_opt_    LPWSTR             pPrinterName,
    _Out_       LPHANDLE            phPrinter,
    _In_opt_    LPPRINTER_DEFAULTSW pDefault
) = OpenPrinterW;

DWORD (WINAPI* TrueStartDocPrinterA)(
    _In_             HANDLE  hPrinter,
    _In_range_(1, 3) DWORD    Level,
    _When_(Level == 1, _In_reads_bytes_(sizeof(DOC_INFO_1)))
    _When_(Level == 3, _In_reads_bytes_(sizeof(DOC_INFO_3)))
    LPBYTE  pDocInfo
) = StartDocPrinterA;
DWORD (WINAPI* TrueStartDocPrinterW)(
    _In_             HANDLE  hPrinter,
    _In_range_(1, 3) DWORD    Level,
    _When_(Level == 1, _In_reads_bytes_(sizeof(DOC_INFO_1)))
    _When_(Level == 3, _In_reads_bytes_(sizeof(DOC_INFO_3)))
    LPBYTE  pDocInfo
) = StartDocPrinterW;

DrawInterface* g_DrawInterface = NULL;

void HookPrint(DrawInterface* dh, PTCHAR pStrDriverName)
{
    StrCpy(g_NeedDriverName, pStrDriverName);
    g_DrawInterface = dh;

    DetourRestoreAfterWith();

    printf("LanxumPrinter" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
        " Starting.\n");
    fflush(stdout);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    //DetourAttach(&(PVOID&)TrueStartDocA, NewStartDocA);
    //DetourAttach(&(PVOID&)TrueStartDocW, NewStartDocW);
    //DetourAttach(&(PVOID&)TrueEndDoc, NewEndDoc);
    DetourAttach(&(PVOID&)TrueStartPage, NewStartPage);
    DetourAttach(&(PVOID&)TrueEndPage, NewEndPage);
    /*-------------------------------------------------*/
    //DetourAttach(&(PVOID&)TrueOpenPrinterA, NewTrueOpenPrinterA);
    //DetourAttach(&(PVOID&)TrueOpenPrinterW, NewTrueOpenPrinterW);
    DetourAttach(&(PVOID&)TrueStartDocPrinterA, NewStartDocPrinterA);
    DetourAttach(&(PVOID&)TrueStartDocPrinterW, NewStartDocPrinterW);

    LONG error = DetourTransactionCommit();

    if (error == NO_ERROR) {
        printf("LanxumPrinter" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
            " Detoured some GDI functions.\n");
    }
    else {
        printf("LanxumPrinter" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
            " Error detouring GDI functions: %ld\n", error);
    }
}

void UnhookPrint()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    
    //DetourDetach(&(PVOID&)TrueStartDocA, StartDocA);
    //DetourDetach(&(PVOID&)TrueStartDocW, StartDocW);
    //DetourDetach(&(PVOID&)TrueEndDoc, EndDoc);
    DetourDetach(&(PVOID&)TrueStartPage, StartPage);
    DetourDetach(&(PVOID&)TrueEndPage, EndPage);
    /*------------------------------------------------*/
    //DetourDetach(&(PVOID&)TrueOpenPrinterA, NewTrueOpenPrinterA);
    //DetourDetach(&(PVOID&)TrueOpenPrinterW, NewTrueOpenPrinterW);
    DetourDetach(&(PVOID&)TrueStartDocPrinterA, NewStartDocPrinterA);
    DetourDetach(&(PVOID&)TrueStartDocPrinterW, NewStartDocPrinterW);
    DetourTransactionCommit();
    if (g_DrawInterface)
    {
        delete g_DrawInterface;
        g_DrawInterface = NULL;
    }
}

int WINAPI NewStartDocA(HDC hdc, const DOCINFOA* lpdi)
{
    return TrueStartDocA(hdc, lpdi);
}

int WINAPI NewStartDocW(HDC hdc, const DOCINFOW* lpdi)
{
    return TrueStartDocW(hdc, lpdi);
}

int WINAPI NewEndDoc(HDC hdc)
{
    return TrueEndDoc(hdc);
}

int WINAPI NewStartPage(HDC hdc)
{
    int iResult = TrueStartPage(hdc);
    if (g_DrawInterface && g_bEnableBarcode)
        g_DrawInterface->PageStartDraw(hdc);
    return iResult;
}

int WINAPI NewEndPage(HDC hdc)
{
    if(g_DrawInterface && g_bEnableBarcode)
        g_DrawInterface->PageEndDraw(hdc);
    return TrueEndPage(hdc);
}

BOOL WINAPI NewTrueOpenPrinterA(
    _In_opt_    LPSTR             pPrinterName,
    _Out_       LPHANDLE            phPrinter,
    _In_opt_    LPPRINTER_DEFAULTSA pDefault
)
{
    return TrueOpenPrinterA(pPrinterName, phPrinter, pDefault);
}

BOOL WINAPI NewTrueOpenPrinterW(
    _In_opt_    LPWSTR             pPrinterName,
    _Out_       LPHANDLE            phPrinter,
    _In_opt_    LPPRINTER_DEFAULTSW pDefault
)
{
    return TrueOpenPrinterW(pPrinterName, phPrinter, pDefault);
}

void CheckPrinter(HANDLE  hPrinter)
{
    PRINTER_INFO_2 printerInfo2;
    printerInfo2.pPortName = new TCHAR[1024];
    //BYTE printerInfo2[5000] = {NULL};
    DWORD pcbNeeded = 0;

    GetPrinter(hPrinter, 2, 0, 0, &pcbNeeded);
    if (pcbNeeded)
    {
        PRINTER_INFO_2* pInfo = (PRINTER_INFO_2*)malloc(pcbNeeded);
        BOOL bVal = GetPrinter(hPrinter, 2, (LPBYTE)pInfo, pcbNeeded, &pcbNeeded);
        bVal;
        if (StrCmp(g_NeedDriverName, pInfo->pDriverName) == 0)
        {
            g_bEnableBarcode = TRUE;
        }
        else
        {
            g_bEnableBarcode = FALSE;
        }
        delete pInfo;
    }
}

DWORD
WINAPI
NewStartDocPrinterA(
    _In_             HANDLE  hPrinter,
    _In_range_(1, 3) DWORD    Level,
    _When_(Level == 1, _In_reads_bytes_(sizeof(DOC_INFO_1)))
    _When_(Level == 3, _In_reads_bytes_(sizeof(DOC_INFO_3)))
    LPBYTE  pDocInfo
)
{
    CheckPrinter(hPrinter);
    return TrueStartDocPrinterA(hPrinter, Level, pDocInfo);
}

DWORD
WINAPI
NewStartDocPrinterW(
    _In_             HANDLE  hPrinter,
    _In_range_(1, 3) DWORD    Level,
    _When_(Level == 1, _In_reads_bytes_(sizeof(DOC_INFO_1)))
    _When_(Level == 3, _In_reads_bytes_(sizeof(DOC_INFO_3)))
    LPBYTE  pDocInfo
)
{
    CheckPrinter(hPrinter);
    return TrueStartDocPrinterW(hPrinter, Level, pDocInfo);
}