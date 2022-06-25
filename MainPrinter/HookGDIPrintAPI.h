#pragma once
#define DETOURS_INTERNAL

#include "detours.h"
#include <wingdi.h>
#include <winspool.h>
#include <Shlwapi.h>

int WINAPI NewStartDocA(HDC hdc, CONST DOCINFOA* lpdi);
int WINAPI NewStartDocW(HDC hdc, CONST DOCINFOW* lpdi);
int WINAPI NewEndDoc(HDC hdc);
int WINAPI NewStartPage(HDC hdc);
int WINAPI NewEndPage(HDC hdc);

BOOL WINAPI NewTrueOpenPrinterA(
    _In_opt_    LPSTR             pPrinterName,
    _Out_       LPHANDLE            phPrinter,
    _In_opt_    LPPRINTER_DEFAULTSA pDefault
);

BOOL WINAPI NewTrueOpenPrinterW(
    _In_opt_    LPWSTR             pPrinterName,
    _Out_       LPHANDLE            phPrinter,
    _In_opt_    LPPRINTER_DEFAULTSW pDefault
);

DWORD
WINAPI
NewStartDocPrinterA(
    _In_             HANDLE  hPrinter,
    _In_range_(1, 3) DWORD    Level,
    _When_(Level == 1, _In_reads_bytes_(sizeof(DOC_INFO_1)))
    _When_(Level == 3, _In_reads_bytes_(sizeof(DOC_INFO_3)))
    LPBYTE  pDocInfo
);
DWORD
WINAPI
NewStartDocPrinterW(
    _In_             HANDLE  hPrinter,
    _In_range_(1, 3) DWORD    Level,
    _When_(Level == 1, _In_reads_bytes_(sizeof(DOC_INFO_1)))
    _When_(Level == 3, _In_reads_bytes_(sizeof(DOC_INFO_3)))
    LPBYTE  pDocInfo
);