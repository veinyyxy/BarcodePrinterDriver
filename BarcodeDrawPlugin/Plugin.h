#pragma once
// Necessary for compiling under VC.
#if(!defined(WINVER) || (WINVER < 0x0500))
#undef WINVER
#define WINVER          0x0500
#endif
#if(!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0500))
#undef _WIN32_WINNT
#define _WIN32_WINNT    0x0500
#endif



// Required header files that shouldn't change often.
#include <shlwapi.h>
#include <STDDEF.H>
#include <STDLIB.H>
#include <OBJBASE.H>
#include <STDARG.H>
#include <STDIO.H>
#include <WINDEF.H>
#include <WINERROR.H>
#include <WINBASE.H>
#include <WINGDI.H>
#include <WINDDI.H>
#include <winddiui.h>
#include <TCHAR.H>
#include <EXCPT.H>
#include <ASSERT.H>
#include <Compstui.h>
#include <PRINTOEM.H>
#include <INITGUID.H>

// Safe integer functions to detect integer overflow/underflow conditions:
#include <intsafe.h>
#include <strsafe.h>