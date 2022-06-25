#pragma once
#include "DrawInterface.h"

class BarcodeGenerator;

class DrawBarcode : public DrawInterface
{
public:
	DrawBarcode();
	~DrawBarcode();
	virtual int PageStartDraw(HDC hdc) override;
	virtual int PageEndDraw(HDC hdc) override;
private:
	BarcodeGenerator* m_pGenerator;
};

