#pragma once
#include <windef.h>

class DrawInterface
{
public:
	virtual int PageStartDraw(HDC hdc) = 0;
	virtual int PageEndDraw(HDC hdc) = 0;
	inline virtual ~DrawInterface() = 0;
};

typedef DrawInterface* (*Fun_GetDrawObject)();
typedef void (*Fun_ReleaseDrawObject)();

typedef void* DRAW_HANDLE;