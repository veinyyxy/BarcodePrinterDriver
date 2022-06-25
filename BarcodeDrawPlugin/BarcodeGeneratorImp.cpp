#include "BarcodeGeneratorImp.h"
#include "zint.h"
bool BarcodeGeneratorImp::CreateBarcode(P_BARCODEDATA pBarCodeData, BYTE* pContent, int iLen)
{
    struct zint_symbol* pZintSymbol;
    pZintSymbol = ZBarcode_Create();
    pZintSymbol->output_options = 0;
    pZintSymbol->input_mode = UNICODE_MODE;
    pZintSymbol->symbology = pBarCodeData->type;
    pZintSymbol->whitespace_width = 0;
    pZintSymbol->border_width = 1;
    pZintSymbol->width = pBarCodeData->width;
    pZintSymbol->height = pBarCodeData->height;
    pZintSymbol->option_1 = -1;
    pZintSymbol->option_2 = 0;
    pZintSymbol->option_3 = 928;
    pZintSymbol->show_hrt = 1;
    pZintSymbol->scale = 2;

    int error_number = ZBarcode_Encode(pZintSymbol, pContent, iLen);
    error_number = ZBarcode_Buffer(pZintSymbol, 0);

    pBarCodeData->bits_per_pixel = 24;
    pBarCodeData->color_width = pZintSymbol->bitmap_width;
    pBarCodeData->color_height = pZintSymbol->bitmap_height;
    int iByteSize = pZintSymbol->bitmap_width * pZintSymbol->bitmap_height * (24 / 8);
    pBarCodeData->color_buffer = new BYTE[iByteSize];
    memcpy(pBarCodeData->color_buffer, pZintSymbol->bitmap, iByteSize);
    ZBarcode_Delete(pZintSymbol);
    return error_number;
}
