#include "StdAfx.h"
#include "MSExcel.h"
#include "OLEMethod.h"

CMSExcel::CMSExcel(void)
{
    m_hr=S_OK;
    m_pEApp=NULL;
    m_pBooks=NULL;
    m_pActiveBook=NULL;
}

HRESULT CMSExcel::Initialize(bool bVisible)
{
    CoInitialize(NULL);
    CLSID clsid;
    m_hr = CLSIDFromProgID(L"Excel.Application", &clsid);
    if(SUCCEEDED(m_hr))
    {
        m_hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&m_pEApp);
        if(FAILED(m_hr)) m_pEApp=NULL;
    }
    {
        m_hr=SetVisible(bVisible);
    }
    return m_hr;
}


HRESULT CMSExcel::SetVisible(bool bVisible)
{
/*	DISPID dispID;
    VARIANT pvResult;
    LPOLESTR ptName=_T("Visible");
    m_hr = m_pWApp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
    if(SUCCEEDED(m_hr))
    {
        VARIANT x;
        x.vt = VT_I4;
        x.lVal =bVisible?1:0;
        DISPID prop=DISPATCH_PROPERTYPUT;

        DISPPARAMS dp = { NULL,NULL,0,0 };
        dp.cArgs =1;
        dp.rgvarg =&x;
        dp.cNamedArgs=1;
        dp.rgdispidNamedArgs= &prop;
        m_hr = m_pWApp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT,
                                &dp, &pvResult, NULL, NULL);
    }*/
    if(!m_pEApp) return E_FAIL;
    VARIANT x;
    x.vt = VT_I4;
    x.lVal = bVisible;
    m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, m_pEApp, L"Visible", 1, x);

    return m_hr;
}

HRESULT CMSExcel::OpenExcelBook(LPCTSTR szFilename, bool bVisible)
{
    if(m_pEApp==NULL)
    {
        if(FAILED(m_hr=Initialize(bVisible)))
            return m_hr;
    }

    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pEApp, L"Workbooks", 0);
        m_pBooks = result.pdispVal;
    }

    {
        COleVariant sFname(szFilename);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pBooks, L"Open", 1,sFname.Detach());
        m_pActiveBook = result.pdispVal;
    }
    return m_hr;
}


HRESULT CMSExcel::NewExcelBook(bool bVisible)
{
    if(m_pEApp==NULL)
    {
        if(FAILED(m_hr=Initialize(bVisible)))
            return m_hr;
    }

    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pEApp, L"Workbooks", 0);
        m_pBooks = result.pdispVal;
    }

    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_METHOD, &result, m_pBooks, L"Add", 0);
        m_pActiveBook = result.pdispVal;
    }
    return m_hr;
}

HRESULT CMSExcel::SaveAs(LPCTSTR szFilename, int nSaveAs)
{
    COleVariant varFilename(szFilename);
    VARIANT varAs;
    varAs.vt=VT_I4;
    varAs.intVal=nSaveAs;
    m_hr=OLEMethod(DISPATCH_METHOD,NULL,m_pActiveBook,L"SaveAs",2,varAs,varFilename.Detach());
    return m_hr;
}

HRESULT CMSExcel::ProtectExcelWorkbook(LPCTSTR szPassword)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    COleVariant olePassword(szPassword);
    return m_hr=OLEMethod(DISPATCH_METHOD, NULL, m_pActiveBook, L"Protect", 1, olePassword.Detach());
}

HRESULT CMSExcel::UnProtectExcelWorkbook(LPCTSTR szPassword)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    COleVariant olePassword(szPassword);
    return m_hr=OLEMethod(DISPATCH_METHOD, NULL, m_pActiveBook, L"Unprotect", 1, olePassword.Detach());
}

HRESULT CMSExcel::ProtectExcelSheet(int nSheetNo, LPCTSTR szPassword)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = nSheetNo;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    {
        COleVariant olePassword(szPassword);
        m_hr=OLEMethod(DISPATCH_METHOD, NULL, pSheet, L"Protect", 1, olePassword.Detach());
    }
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::UnProtectExcelSheet(int nSheetNo, LPCTSTR szPassword)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = nSheetNo;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    {
        COleVariant olePassword(szPassword);
        m_hr=OLEMethod(DISPATCH_METHOD, NULL, pSheet, L"Unprotect", 1, olePassword.Detach());
    }
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::SetExcelCellFormat(LPCTSTR szRange, LPCTSTR szFormat)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    IDispatch* pRange;
    {
        COleVariant oleParam(szRange);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleParam.Detach());
        pRange = result.pdispVal;
    }

    {
        COleVariant oleParam(szFormat);
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pRange, L"NumberFormat", 1, oleParam.Detach());
    }
    pRange->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::SetExcelSheetName(int nSheetNo, LPCTSTR szSheetName)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = nSheetNo;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }
    {
        COleVariant oleName(szSheetName);
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pSheet, L"Name", 1, oleName.Detach());
    }
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::GetExcelValue(LPCTSTR szCell, CString &sValue)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;

    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    IDispatch* pRange;
    {
        COleVariant oleRange(szCell);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleRange.Detach());
        pRange = result.pdispVal;
    }

    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pRange, L"Value", 0);
        sValue = CString(result.bstrVal);
    }

    pRange->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::SetExcelBackgroundColor(LPCTSTR szRange, COLORREF crColor, int nPattern)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    IDispatch* pRange;
    {
        COleVariant oleRange(szRange);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleRange.Detach());
        pRange = result.pdispVal;
    }

    IDispatch *pInterior;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pRange, L"Interior",0);
        pInterior=result.pdispVal;
    }
    {
        VARIANT x;
        x.vt = VT_I4;
        x.lVal = crColor;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pInterior, L"Color", 1, x);
        x.lVal = nPattern;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pInterior, L"Pattern", 1, x);
    }

    pInterior->Release();
    pRange->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::SetExcelFont(LPCTSTR szRange, LPCTSTR szName, int nSize, COLORREF crColor, bool bBold, bool bItalic)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }
    IDispatch* pRange;
    {
        COleVariant oleRange(szRange);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleRange.Detach());
        pRange = result.pdispVal;
    }

    IDispatch *pFont;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pRange, L"Font",0);
        pFont=result.pdispVal;
    }
    {
        COleVariant oleName(szName);
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Name", 1, oleName.Detach());
        VARIANT x;
        x.vt = VT_I4;
        x.lVal = nSize;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Size", 1, x);
        x.lVal = crColor;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Color", 1, x);
        x.lVal = bBold?1:0;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Bold", 1, x);
        x.lVal = bItalic?1:0;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Italic", 1, x);
    }
    pFont->Release();
    pSheet->Release();
    pRange->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::SetExcelValue(LPCTSTR szRange,LPCTSTR szValue,bool bAutoFit, int nAlignment)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    IDispatch* pRange;
    {
        COleVariant oleRange(szRange);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleRange.Detach());
        pRange = result.pdispVal;
    }

    {
        COleVariant oleValue(szValue);
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pRange, L"Value", 1, oleValue.Detach());
    }

    if(bAutoFit)
    {
        IDispatch* pEntireColumn;
        {
            VARIANT result;
            VariantInit(&result);
            m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pRange, L"EntireColumn",0);
            pEntireColumn= result.pdispVal;
        }

        {
            m_hr=OLEMethod(DISPATCH_METHOD, NULL, pEntireColumn, L"AutoFit", 0);
        }
        pEntireColumn->Release();
    }

    {
        VARIANT x;
        x.vt = VT_I4;
        x.lVal = nAlignment;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pRange, L"HorizontalAlignment", 1, x);
    }

    pRange->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

//
// http://forums.codeguru.com/showthread.php?272007-Open-Excel-in-VC-and-write-data/page2 참조
//
HRESULT CMSExcel::SetExcelImage(LPCTSTR szPath, int nLeft, int nTop, int nWidth, int nHeight)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    BSTR b1 = CString(szPath).AllocSysString();

    IDispatch *pXlShapes;
    {
        VARIANT result;
        VariantInit(&result);

        OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Shapes", 0);
        pXlShapes = result.pdispVal;
    }

    {
        VARIANT result;
        VariantInit(&result);
        VARIANT fname;
        fname.vt = VT_BSTR;
        fname.bstrVal=::SysAllocString(b1);
        VARIANT xpropf; // file
        xpropf.vt=VT_BOOL;
        xpropf.boolVal=FALSE;
        VARIANT xpropt; // SaveWithDocument
        xpropt.vt=VT_BOOL;
        xpropt.boolVal=TRUE;

        VARIANT xtop;
        xtop.vt=VT_R8;
        xtop.dblVal=nTop;
        VARIANT xleft;
        xleft.vt=VT_R8;
        xleft.dblVal=nLeft;
        VARIANT xwidth;
        xwidth.vt=VT_R8;
        xwidth.dblVal=nWidth;
        VARIANT xheight;
        xheight.vt=VT_R8;
        xheight.dblVal=nHeight;

        //I need to put the arguments in reverse order
        m_hr=OLEMethod(DISPATCH_METHOD, &result, pXlShapes, L"AddPicture", 7,xheight,xwidth,xtop,xleft,xpropt,xpropf,fname);
    }
    SysFreeString(b1);

    pXlShapes->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

//
// Excel 2007에서는 이미지는 좌측상단에 Paste되에 활용용도가 없을것 같다. (XP는 2013을 지원하지 않는다)
// => 2013에서는 정상적으로 되는데...
//
HRESULT CMSExcel::SetExcelImagePaste(LPCTSTR szRange)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

#if 1 // Range로 위치 설정
    IDispatch* pRange;
    {
        COleVariant oleRange(szRange);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleRange.Detach());
        pRange = result.pdispVal;
    }

    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_METHOD, &result, pRange, L"Select", 0);
    }

    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_METHOD, &result, pSheet, L"Paste", 0);
    }

    pRange->Release();

#else // Row, Col으로 위치 설정

    IDispatch *pCells;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT row, col;
        row.vt =VT_I4;
        row.lVal =nRow;
        col.vt =VT_I4;
        col.lVal =nCol;
        OLEMethod(DISPATCH_PROPERTYGET, &result, pXLApp, L"Cells", 2,col,row);
        pCells=result.pdispVal;
    }
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_METHOD, &result, pCells, L"Select", 0);
        nCol=result.iVal;
    }

    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_METHOD, &result, pSheet, L"Paste", 0);
    }


    pCells->Release();

#endif

    pSheet->Release();
    pSheets->Release();

    return m_hr;
}


HRESULT CMSExcel::SetExcelBorder(LPCTSTR szRange, int nStyle)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    IDispatch* pRange;
    {
        COleVariant oleParam(szRange);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleParam.Detach());
        pRange = result.pdispVal;
    }

    {
        VARIANT x;
        x.vt = VT_I4;
        x.lVal = nStyle;
        m_hr=OLEMethod(DISPATCH_METHOD, NULL, pRange, L"BorderAround", 1, x);
    }
    pRange->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::MergeExcelCells(LPCTSTR szRange)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }

    IDispatch* pRange;
    {
        COleVariant oleParam(szRange);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleParam.Detach());
        pRange = result.pdispVal;
    }

    {
        m_hr=OLEMethod(DISPATCH_METHOD, NULL, pRange, L"Merge", 0);
    }
    pRange->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::AutoFitExcelColumn(LPCTSTR szColumn)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = 1;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }
    IDispatch* pRange;
    {
        COleVariant oleParam(szColumn);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Columns", 1, oleParam.Detach());
        pRange = result.pdispVal;
    }
    IDispatch* pEntireColumn;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pRange, L"EntireColumn",0);
        pEntireColumn= result.pdispVal;
    }

    {
        m_hr=OLEMethod(DISPATCH_METHOD, NULL, pEntireColumn, L"AutoFit", 0);
    }
    pEntireColumn->Release();
    pRange->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}

HRESULT CMSExcel::AddExcelChart(LPCTSTR szRange, LPCTSTR szTitle, int nChartType, int nLeft, int nTop, int nWidth, int nHeight, int nRangeSheet, int nChartSheet)
{
    if(!m_pEApp) return E_FAIL;
    if(!m_pActiveBook) return E_FAIL;
    IDispatch *pSheets;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Sheets", 0);
        pSheets = result.pdispVal;
    }
    IDispatch *pSheet;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = nRangeSheet;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet = result.pdispVal;
    }
    IDispatch *pSheet2;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT itemn;
        itemn.vt = VT_I4;
        itemn.lVal = nChartSheet;
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheets, L"Item", 1, itemn);
        pSheet2 = result.pdispVal;
    }
    VARIANT var;
    IDispatch *pRange;
    {
        COleVariant oleRange(szRange);
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet, L"Range", 1, oleRange.Detach());
        var.vt = VT_DISPATCH;
        var.pdispVal = result.pdispVal;
        pRange = result.pdispVal;
    }

    IDispatch *pChartObjects;
    {
        VARIANT result;
        VariantInit(&result);

        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pSheet2, L"ChartObjects", 0);
        pChartObjects = result.pdispVal;
    }

    IDispatch *pChartObject;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT left, top, width, height;
        left.vt = VT_R8;
        left.dblVal = nLeft;
        top.vt = VT_R8;
        top.dblVal = nTop;
        width.vt = VT_R8;
        width.dblVal = nWidth;
        height.vt = VT_R8;
        height.dblVal = nHeight;

        m_hr=OLEMethod(DISPATCH_METHOD, &result, pChartObjects, L"ADD", 4, height,width,top,left);
        pChartObject = result.pdispVal;
    }

    IDispatch *pChart;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET, &result, pChartObject, L"Chart", 0);
        pChart = result.pdispVal;
    }

    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_METHOD, &result, pChart, L"ChartWizard", 1, var);
    }

    {
        VARIANT result;
        VariantInit(&result);
        VARIANT hastitle;
        hastitle.vt=VT_BOOL;
        hastitle.boolVal=TRUE;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, &result, pChart, L"HasTitle", 1,hastitle);

    }
    IDispatch *pChartTitle;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET,&result,pChart,L"ChartTitle",0);
        pChartTitle=result.pdispVal;
    }
    IDispatch *pChars;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_METHOD,&result,pChartTitle,L"Characters",0);
        pChars=result.pdispVal;
    }

    {
        VARIANT result;
        VariantInit(&result);
        COleVariant oleTitle(szTitle);
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, &result, pChars, L"Text", 1,oleTitle.Detach());
    }
    IDispatch *pFont;
    {
        VARIANT result;
        VariantInit(&result);
        m_hr=OLEMethod(DISPATCH_PROPERTYGET,&result,pChartTitle,L"Font",0);
        pFont=result.pdispVal;
    }
    {
        COleVariant oleName(_T("Arial"));
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Name", 1, oleName.Detach());
        VARIANT x;
        x.vt = VT_I4;
        x.lVal = 10;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Size", 1, x);
        x.lVal = RGB(0,0,0);
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Color", 1, x);
        x.lVal = 0;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Bold", 1, x);
        x.lVal = 0;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pFont, L"Italic", 1, x);
    }

    {
        VARIANT type;
        type.vt = VT_I4;
        type.lVal = nChartType ;
        m_hr=OLEMethod(DISPATCH_PROPERTYPUT, NULL, pChart, L"ChartType", 1, type);
    }

    {
        VARIANT result;
        VariantInit(&result);
        VARIANT plotby;
        plotby.vt =VT_I4;
        plotby.lVal =1;

        m_hr=OLEMethod(DISPATCH_METHOD, &result, pChart, L"SetSourceData", 2, plotby, var);
    }
    pFont->Release();
    pChartTitle->Release();
    pChars->Release();
    pChartObject->Release();
    pChartObjects->Release();
    pRange->Release();
    pSheet2->Release();
    pSheet->Release();
    pSheets->Release();
    return m_hr;
}


HRESULT CMSExcel::MoveCursor(int nDirection)
{
    if(!m_pEApp || !m_pActiveBook) return E_FAIL;

    IDispatch *pXLApp;
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Application", 0);
        pXLApp= result.pdispVal;
    }
    IDispatch *pActiveCell;
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_PROPERTYGET, &result, pXLApp, L"ActiveCell", 0);
        pActiveCell=result.pdispVal;
    }
    if(pActiveCell)
    {
        int nRow,nCol;
        {
            VARIANT result;
            VariantInit(&result);
            OLEMethod(DISPATCH_PROPERTYGET, &result, pActiveCell, L"Row", 0);
            nRow=result.iVal;
        }
        {
            VARIANT result;
            VariantInit(&result);
            OLEMethod(DISPATCH_PROPERTYGET, &result, pActiveCell, L"Column", 0);
            nCol=result.iVal;
        }

        switch(nDirection)
        {
            case 1:
                if(nCol>1) nCol--;break;
            case 2:
                nCol++;
                break;
            case 3:
                if(nRow>1) nRow--;
                break;
            case 4:
                nRow++;
                break;
        }

        IDispatch *pCells;
        {
            VARIANT result;
            VariantInit(&result);
            VARIANT row, col;
            row.vt =VT_I4;
            row.lVal =nRow;
            col.vt =VT_I4;
            col.lVal =nCol;
            OLEMethod(DISPATCH_PROPERTYGET, &result, pXLApp, L"Cells", 2,col,row);
            pCells=result.pdispVal;
        }
        {
            VARIANT result;
            VariantInit(&result);
            OLEMethod(DISPATCH_METHOD, &result, pCells, L"Select", 0);
            nCol=result.iVal;
        }
        pCells->Release();
        pActiveCell->Release();
    }
    pXLApp->Release();
    return m_hr;
}

HRESULT CMSExcel::GetActiveCell(int &nRow, int &nCol)
{
    if(!m_pEApp || !m_pActiveBook) return E_FAIL;

    IDispatch *pXLApp;
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Application", 0);
        pXLApp= result.pdispVal;
    }
    IDispatch *pActiveCell;
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_PROPERTYGET, &result, pXLApp, L"ActiveCell", 0);
        pActiveCell=result.pdispVal;
    }
    if(pActiveCell)
    {
        {
            VARIANT result;
            VariantInit(&result);
            OLEMethod(DISPATCH_PROPERTYGET, &result, pActiveCell, L"Row", 0);
            nRow=result.iVal;
        }
        {
            VARIANT result;
            VariantInit(&result);
            OLEMethod(DISPATCH_PROPERTYGET, &result, pActiveCell, L"Column", 0);
            nCol=result.iVal;
        }
        pActiveCell->Release();
    }
    pXLApp->Release();
    return m_hr;
}

HRESULT CMSExcel::SetActiveCellText(LPCTSTR szText)
{
    if(!m_pEApp || !m_pActiveBook) return E_FAIL;

    IDispatch *pXLApp;
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_PROPERTYGET, &result, m_pActiveBook, L"Application", 0);
        pXLApp= result.pdispVal;
    }
    IDispatch *pActiveCell;
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_PROPERTYGET, &result, pXLApp, L"ActiveCell", 0);
        pActiveCell=result.pdispVal;
    }

    int nRow,nCol;
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_PROPERTYGET, &result, pActiveCell, L"Row", 0);
        nRow=result.iVal;
    }
    {
        VARIANT result;
        VariantInit(&result);
        OLEMethod(DISPATCH_PROPERTYGET, &result, pActiveCell, L"Column", 0);
        nCol=result.iVal;
    }


    IDispatch *pCells;
    {
        VARIANT result;
        VariantInit(&result);
        VARIANT row, col;
        row.vt =VT_I4;
        row.lVal =nRow;
        col.vt =VT_I4;
        col.lVal =nCol;
        OLEMethod(DISPATCH_PROPERTYGET, &result, pXLApp, L"Cells", 2,col,row);
        pCells=result.pdispVal;
    }
    {
        COleVariant val(szText);
        OLEMethod(DISPATCH_PROPERTYPUT, NULL, pCells, L"Value", 1,val.Detach());
    }
    pCells->Release();
    pActiveCell->Release();
    pXLApp->Release();
    return m_hr;
}

HRESULT CMSExcel::Quit()
{
    if(m_pEApp==NULL) return E_FAIL;
    DISPID dispID;
    LPOLESTR ptName=L"Quit";
    m_hr = m_pEApp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);

    if(SUCCEEDED(m_hr))
    {
        DISPPARAMS dp = { NULL, NULL, 0, 0 };
        m_hr = m_pEApp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
                                    &dp, NULL, NULL, NULL);
    }
    m_pEApp->Release();
    m_pEApp=NULL;
    return m_hr;
}


CMSExcel::~CMSExcel(void)
{
    if (m_pEApp != NULL)
    {
        Quit();
        CoUninitialize();
    }
}

#if 0  // 참고용

_Worksheet		m_sheet;
LPDISPATCH	lpDispShape;
lpDispShape = m_sheet.GetShapes();
CShapes shapes;
shapes.AttachDispatch(lpDispShape);
shapes.AddPicture(
    a_sPicturePath, //Filename
    FALSE,			//LinkTofile
    TRUE,			//SaveWithDocument
    a_fLeft,		//Left
    a_fTop,			//Top
    a_fWidth,		//Width
    a_fHeight);		//Height

shapes.ReleaseDispatch();



_Application objApp;
        _Workbook objBook;
        Workbooks objBooks;
        Worksheets objSheets;
        _Worksheet objSheet;
        COleVariant VOptional((long) DISP_E_PARAMNOTFOUND,VT_ERROR);
        COleVariant vTrue((short)TRUE), vFalse((short) FALSE);
        Range range;
        LPDISPATCH lpDisp;
        objApp.CreateDispatch("Excel.Application");
        objApp.SetVisible(TRUE);
        lpDisp = objApp.GetWorkbooks();
        objBooks.AttachDispatch(lpDisp);
        lpDisp = objBooks.Add(VOptional);
        objBook.AttachDispatch(lpDisp);
        lpDisp = objBook.GetSheets();
        objSheets.AttachDispatch(lpDisp);
        lpDisp = objSheets.GetItem(COleVariant((short)1));
        objSheet.AttachDispatch(lpDisp);
        Range oRange = objSheet.GetRange((COleVariant)"A1", COleVariant)"O34");
        Shapes oShapes = objSheet.GetShapes();
        Shape oShape;
         oShapes.AddLine(200,200,300,200);/* Adding Line */
         oShapes.AddTextbox(1,10,10,100,100); /* Adding Textbox */
         oShapes.AddShape(1, 156, 105, 319, 75);/* Adding Rectangle */
Regards
Pitambar


CShapes shapes;
shapes = excel_sheet.get_Shapes();

CShape tmpLine = shapes.AddLine(100.0f, 100.0f, 300.0f, 300.0f);
CShape tmpShape2 = shapes.AddShape(4L, 350.0f, 50.0f, 100.0f, 100.0f);
CShape tmpShape1 = shapes.AddShape(4L, 50.0f, 50.0f, 100.0f, 100.0f);

CConnectorFormat conector = tmpLine.get_ConnectorFormat();
conector.BeginConnect(tmpShape1, 1);
conector.EndConnect(tmpShape2, 1);

tmpLine.RerouteConnections();


-------------------

    아래 두 함수를 한번 살표 보세요

ExcelInsertImage
SaveBitmapToExcel


// Variant V;
Variant excel_app;
Variant excel_window;
Variant excel_book;
Variant my_worksheet;

//----------------------------------------------------------------------------
void __fastcall TDLG_RPT_EXCEL::ExcelOpen()
{
excel_app=Variant::CreateObject("excel.application");
excel_app.OlePropertySet("Visible", (Variant)false);
}
//---------------------------------------------------------------------------
void __fastcall TDLG_RPT_EXCEL::ExcelExit()
{
excel_app.OleFunction("Quit");
excel_app = Unassigned;
}
//---------------------------------------------------------------------------

//excel파일을 읽어온다.
void __fastcall TDLG_RPT_EXCEL::ExcelReportFileOpen(AnsiString asFileName)
{
excel_app.OlePropertyGet("WorkBooks").OleProcedure("Open",asFileName.c_str());
excel_window = excel_app.OlePropertyGet("ActiveWindow");
excel_book=excel_app.OlePropertyGet("ActiveWorkbook");
my_worksheet=excel_book.OlePropertyGet("ActiveSheet");

}
//---------------------------------------------------------------------------

// image파일을 특정sheet의 특정 cell에 insert 하는 코드

Variant __fastcall TDLG_RPT_EXCEL::ExcelInsertImage(AnsiString sh_name, int iRow, int iCol, AnsiString iFileName)
{
Variant cells;
my_worksheet = excel_book.OlePropertyGet("WorkSheets", sh_name.c_str());
my_worksheet.OleFunction("Select");
cells = my_worksheet.OlePropertyGet("Cells", iRow, iCol);
cells.OleFunction("Select");
Variant c_image;
c_image = my_worksheet.OlePropertyGet("Pictures").OleFunction("Insert", WideString(iFileName));
c_image.OleFunction("Select");
return c_image;
}
//---------------------------------------------------------------------------

//Bitmap을 특정 Sheet 특정 Cell에 Paste하는 코드
//클립보드를 이용함

void __fastcall TDLG_RPT_EXCEL::SaveBitmapToExcel(Graphics::TBitmap *bmp,String sSheetName,int iRow,int iCol)
{
if(bmp)
{
Clipboard()->Assign(bmp);
my_worksheet = excel_book.OlePropertyGet("WorkSheets", WideString(sSheetName));
my_worksheet.OlePropertyGet("Cells", iRow, iCol).OleProcedure("Select");
my_worksheet.OleProcedure("Paste");
}
}

lpdisp=sheet.GetShapes();
Shapes shps(lpdisp);
Shape shp;
shp=shps.AddPicture("c:\\My Documents\\MyLogo.bmp",0,1,0,0,80,36);

/* AddPicture(FileName, LinkToFile, SaveWithDocument, Left, Top, Width, Height) */

COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
OpenClipboard(); // Reserve clipboard for this program
          EmptyClipboard();
          CBitmap MyBitmap;
          MyBitmap.LoadBitmap(IDB_BITMAP1);  // A Bitmap you drew in the
          //  Resource Editor
          HBITMAP MyBitmapHandle = (HBITMAP)MyBitmap;  // Cast it to a HBITMAP
          SetClipboardData(CF_BITMAP, MyBitmapHandle);
          CloseClipboard();
          app.SetVisible(TRUE);
          app.SetUserControl(TRUE);
          sheet.Activate();
          range = sheet.GetRange(COleVariant("A1"), COleVariant("a1"));
          range.Select();
          sheet.Paste(covOptional, covOptional);

#endif
