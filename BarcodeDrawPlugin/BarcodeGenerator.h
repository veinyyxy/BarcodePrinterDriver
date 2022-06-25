#pragma once
#include <minwindef.h>

typedef struct
{
	int type;//barcode type example£ºpdf147, qrcode
	UINT width;//Width of the barcode
	UINT height;//Height of the barcode
	UINT color_width;//pixel of the buffer
	UINT color_height;
	UINT bits_per_pixel;//The bits of per pixel of the barcode
	BYTE *color_buffer;//The color data of the barcode
} BARCODEDATA, * P_BARCODEDATA;

class BarcodeGenerator
{
public:
	virtual bool CreateBarcode(P_BARCODEDATA pBarCodeData, BYTE* pContent, int iLen) = 0;
	virtual ~BarcodeGenerator() {};
};
