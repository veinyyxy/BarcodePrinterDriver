#include "DrawBarcode.h"
#include "BarcodeGeneratorImp.h"
#include "zint.h"
#include "Plugin.h"

DrawBarcode::DrawBarcode() : m_pGenerator(nullptr)
{
    m_pGenerator = new BarcodeGeneratorImp();
}

DrawBarcode::~DrawBarcode()
{
    if (NULL != m_pGenerator)
    {
        delete m_pGenerator;
        m_pGenerator = NULL;
    }
}

int DrawBarcode::PageStartDraw(HDC hdc)
{
    hdc;
    return 0;
}

int DrawBarcode::PageEndDraw(HDC hdc)
{
    int iColorPlans = GetDeviceCaps(hdc, PLANES);
    int iColorNum = GetDeviceCaps(hdc, BITSPIXEL);
    int NumColors = GetDeviceCaps(hdc, NUMCOLORS);

    int iBitCount = 32;
    if (NumColors == -1)
    {
        iBitCount = iColorNum;
    }
    else
    {
        return FALSE;
    }

    BARCODEDATA bd;
    bd.type = BARCODE_PDF417;
    bd.height = 50;
    bd.width = 0;
    BYTE text[] = "This is a test message.";
    m_pGenerator->CreateBarcode(&bd, text, sizeof(text));
    HDC mdc = CreateCompatibleDC(hdc);
    BYTE* bitmapBuffer = bd.color_buffer;

    if (iBitCount == 32)
    {
        UINT bmpSize = bd.color_width * bd.color_height;
        BYTE* pBits = new BYTE[static_cast<__int64>(bmpSize) * 4];
        memset(pBits, 0xFF, static_cast<__int64>(bmpSize) * 4);

        for (__int64 i = 1; i < bmpSize; i++)
        {
            *(pBits + (i * 4) + 0) = *(bitmapBuffer + (i * 3) + 0);
            *(pBits + (i * 4) + 1) = *(bitmapBuffer + (i * 3) + 1);
            *(pBits + (i * 4) + 2) = *(bitmapBuffer + (i * 3) + 2);
            //* (pBits + (i * 4) + 3) = *(my_symbol->bitmap + (i * 3) + 0);
        }

        bitmapBuffer = pBits;

        delete[] bd.color_buffer;
    }
    

    HBITMAP hbmpbit = CreateBitmap(bd.color_width, bd.color_height, iColorPlans, iBitCount, bitmapBuffer);
    SelectObject(mdc, hbmpbit);

    BOOL bResult = BitBlt(hdc, 0, 0, bd.color_width, bd.color_height, mdc, 0, 0, SRCCOPY);

    delete[] bitmapBuffer;
    DeleteObject(hbmpbit);
    DeleteDC(mdc);

    return bResult;
}