#pragma once
#include "BarcodeGenerator.h"
class BarcodeGeneratorImp :
    public BarcodeGenerator
{
public:
    // Inherited via BarcodeGenerator
    virtual bool CreateBarcode(P_BARCODEDATA pBarCodeData, BYTE* pContent, int iLen) override;
};