// drawdoc.cpp : implementation of the CDrawDoc class
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2008 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
// modify : jerry1455@gmail.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DrawDoc.h"
#include "DrawVw.h"
#include "DrawObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


DocType doctypes[CMAX_IMAGE_FORMATS] =
{
    // mim file은 CxImage에서는 Tiff Format으로 인식된다.
    { -1, TRUE, TRUE, _T("Supported files"), _T("*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wbmp;*.wmf;*.emf;*.j2k;*.jp2;*.jbg;*.j2c;*.jpc;*.pgx;*.pnm;*.pgm;*.ppm;*.ras;*.mng;*.jng;*.ska;*.nef;*.crw;*.cr2;*.mrw;*.raf;*.erf;*.3fr;*.dcr;*.raw;*.dng;*.pef;*.x3f;*.arw;*.sr2;*.mef;*.orf;*.mim") },
    { CXIMAGE_FORMAT_BMP, TRUE, TRUE, _T("BMP files"), _T("*.bmp") },
};

/*
.raf (후지필름)
.crw .cr2 .cr3 (캐논)
.srw (삼성)
.tif .k25 .kdc .dcs .dcr .drf (코닥)
.mrw (미놀타)
.nef .nrw (니콘)
.orf (올림푸스)
.dng (어도비)
.ptx .pef (펜탁스)
.arw .srf .sr2 (소니)
.x3f (시그마)
.erf (엡손)
.mef .mos (마미야)
.raw (파나소닉)
.cap .tif .iiq (페이스 원)
.r3d (레드)
.fff (이마콘)
.pxn (로지텍)
.bay (카시오)
*/

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc

IMPLEMENT_DYNCREATE(CDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CDrawDoc, CDocument)
    //{{AFX_MSG_MAP(CDrawDoc)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc construction/destruction

CDrawDoc::CDrawDoc()
{
    //EnableCompoundFile();
    m_pActiveMainView = NULL;

    m_nMapMode = MM_ANISOTROPIC;
    m_paperColor = RGB(255, 255, 255);
    m_nLastROIIndex = 0;
    m_OldZF = m_ZoomFactor=1;
    m_NumSel=0;
    m_etime = 0.0;

//	m_nSelectCh = 0;
    //ComputePageSize();
}

CDrawDoc::~CDrawDoc()
{
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
        delete m_objects.GetNext(pos);
}

BOOL CDrawDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc serialization

void CDrawDoc::Serialize(CArchive& ar)
{
    if ( ar.IsStoring() )
    {
        ar << m_paperColor;
        m_objects.Serialize(ar);
    }
    else
    {
        ar >> m_paperColor;
        m_objects.Serialize(ar);
    }

    // By calling the base class COleDocument, we enable serialization
    //  of the container document's COleClientItem objects automatically.
    CDocument::Serialize(ar);
}


/////////////////////////////////////////////////////////////////////////////
// CDrawDoc implementation

//int CDrawDoc::GetLastNewIndex()
//{
//	int nMax = -1;
//	POSITION pos = m_objects.GetHeadPosition();
//	while (pos != NULL)
//	{
//		CDrawObj* pObj = m_objects.GetNext(pos);
//		if (nMax < pObj->m_pRoiData->nIndex)
//			nMax = pObj->m_pRoiData->nIndex;
//	}
//	return nMax + 1;
//}

void CDrawDoc::Draw(CDC* pDC, CDrawView* pView)
{
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = m_objects.GetNext(pos);

        if (pObj->IsHidden()) // Hidden Flag가 설정된 ROI는 그리지 않는다.
            continue;
        if (m_nSelectCh != pObj->m_pRoiData->m_nCh)
            continue;

        pObj->Draw(pDC);

        if (pView->m_bActive && !pDC->IsPrinting() && pView->IsSelected(pObj))
            pObj->DrawTracker(pDC, CDrawObj::selected); // 선택된영역 Object의 8군데 Tracker를 표시한다.
    }

}

void CDrawDoc::DrawLine(CDC* pDC, CDrawView* pView, int nCh)
{
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = m_objects.GetNext(pos);

        if (pObj->IsHidden()) // Hidden Flag가 설정된 ROI는 그리지 않는다.
            continue;
        if (nCh != pObj->m_pRoiData->m_nCh)
            continue;

        if (pObj->m_nShape == CDrawObj::line) {
            pObj->SetLineWidth(2); // 2016.6.7 - 도희전자 강제로 설정함
            pObj->Draw(pDC);
        }

    }

}

void CDrawDoc::Add(CDrawObj* pObj)
{
/*
    if (pObj->m_pRoiData->nIndex < 0) {
        POSITION pos = m_objects.GetTailPosition();
        if (pos != NULL) {
            if (pObj->m_pParent == NULL) {
                pObj->m_pRoiData->nIndex = m_nLastROIIndex + 1;
                m_nLastROIIndex++;
            }
        }
        else
            m_nLastROIIndex = pObj->m_pRoiData->nIndex = 1;
    } else {
        if (pObj->m_pRoiData->nIndex > m_nLastROIIndex)
            m_nLastROIIndex = pObj->m_pRoiData->nIndex;
    }
*/
    m_objects.AddTail(pObj);
    pObj->m_pDocument = this;
    //SetModifiedFlag();

}
void CDrawDoc::AddBefore(CDrawObj* pObj, CDrawObj* pNewObj)
{
    POSITION pos = m_objects.Find(pObj);
    m_objects.InsertBefore(pos, pNewObj);
    pNewObj->m_pDocument = this;
    //SetModifiedFlag();

}

void CDrawDoc::RemoveAll()
{
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = m_objects.GetNext(pos);
        if (pObj) {
            Remove(pObj);
            pObj->Remove();
        }
    }
    m_objects.RemoveAll();
    g_cRecipeData->m_vecRoiData.clear();
}

void CDrawDoc::Remove(CDrawObj* pObj)
{
    // Find and remove from document
    POSITION pos = m_objects.Find(pObj);
    if (pos != NULL) {
        m_objects.RemoveAt(pos);
    }
    // set document modified flag
    //SetModifiedFlag();

    // call remove for each view so that the view can remove from m_selection
    pos = GetFirstViewPosition();
    while (pos != NULL) {
        CView *pView = GetNextView(pos);
        if (pView->IsKindOf( RUNTIME_CLASS( CDrawView ) ))
            ((CDrawView*)pView)->Remove(pObj);
    }
}

// point is in logical coordinates
CDrawObj* CDrawDoc::ObjectAt(const CPoint& point)
{
    int size = 20; // 20->10 모서리교차는 20으로 하자
    if (GetZoomFactor() < 1.0)
        size = (int)((double)size * (1.0 / GetZoomFactor())); // 축소때만 적용
    CRect rect(CPoint(point.x-size/2, point.y-size/2), CSize(size, size));
    POSITION pos = m_objects.GetTailPosition();
    //while (pos != NULL)
    //{
    //	CDrawObj* pObj = m_objects.GetPrev(pos);
    //	if (pObj->Intersects(rect))
    //		;
    //}
    //pos = m_objects.GetTailPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = m_objects.GetPrev(pos);
        if (m_nSelectCh != pObj->m_pRoiData->m_nCh)
            continue; // 현재 속한 Channel이 아니면..

        //BOOL bInter = pObj->Intersects(rect); // 모서리부분이 교차되는 점이 있으면

        CRect fixed = pObj->m_position;
        fixed.NormalizeRect();
        fixed.InflateRect(size,size);
        if (fixed.PtInRect(point))
        {
            BOOL bInter = pObj->Intersects((float)point.x, (float)point.y, (float)size);	// 마우스로클릭한 반지름 20 이내에 속한 라인이 있으면..
            if (bInter && !pObj->IsHidden()) // Hidden설정이 되어 있지 않은 Obj
                return pObj;
        }
    }
    return NULL;
}

/*
void CDrawDoc::ComputePageSize()
{
    CSize new_size(850, 1100);  // 8.5" x 11" default

    CPrintDialog dlg(FALSE);
    if (AfxGetApp()->GetPrinterDeviceDefaults(&dlg.m_pd))
    {
        // GetPrinterDC returns a HDC so attach it
        CDC dc;
        HDC hDC= dlg.CreatePrinterDC();
        ASSERT(hDC != NULL);
        dc.Attach(hDC);

        // Get the size of the page in loenglish
        new_size.cx = MulDiv(dc.GetDeviceCaps(HORZSIZE), 1000, 254);
        new_size.cy = MulDiv(dc.GetDeviceCaps(VERTSIZE), 1000, 254);
    }

    // if size changed then iterate over views and reset
    if (new_size != m_size)
    {
        m_size = new_size;
        POSITION pos = GetFirstViewPosition();
        while (pos != NULL) {
            if (pView->IsKindOf( RUNTIME_CLASS( CDrawView ) ))
                ((CDrawView*)GetNextView(pos))->SetPageSize(m_size);
        }
    }
}
*/

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc diagnostics

#ifdef _DEBUG
void CDrawDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CDrawDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc commands


//////////////////////////////////////////////////////////////////////////////
// prompt for file name - used for open and save as
// static function called from app
BOOL CDrawDoc::PromptForFileName(CString& fileName, UINT /*nIDSTitle*/,
    DWORD dwFlags, BOOL bOpenFileDialog, int* pType)
{
    CFileDialog dlgFile(bOpenFileDialog);
    CString title;
    if (bOpenFileDialog) title=_T("Open image file"; else title="Save image file");

    dlgFile.m_ofn.Flags |= dwFlags;

	int nDocType = (pType != NULL) ? *pType : 0;// CXIMAGE_FORMAT_BMP;
    //if (nDocType==0) nDocType=1;

    int nIndex = GetIndexFromType(nDocType, bOpenFileDialog);
    if (nIndex == -1) nIndex = 0;

    dlgFile.m_ofn.nFilterIndex = nIndex +1;
    // strDefExt is necessary to hold onto the memory from GetExtFromType
    CString strDefExt = GetExtFromType(nDocType).Mid(2,3);
    dlgFile.m_ofn.lpstrDefExt = strDefExt;

    CString strFilter = GetFileTypes(bOpenFileDialog);
    dlgFile.m_ofn.lpstrFilter = strFilter;
    dlgFile.m_ofn.lpstrTitle = title;
    dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

    BOOL bRet = (dlgFile.DoModal() == IDOK) ? TRUE : FALSE;
    fileName.ReleaseBuffer();
    if (bRet){
        if (pType != NULL){
            int nIndex = (int)dlgFile.m_ofn.nFilterIndex - 1;
            ASSERT(nIndex >= 0);
            *pType = GetTypeFromIndex(nIndex, bOpenFileDialog);
        }
    }
    return bRet;
}

//////////////////////////////////////////////////////////////////////////////
int CDrawDoc::GetIndexFromType(int nDocType, BOOL bOpenFileDialog)
{
    int nCnt = 0;
    for (int i=0;i<CMAX_IMAGE_FORMATS;i++){
        if (bOpenFileDialog ? doctypes[i].bRead : doctypes[i].bWrite){
            if (doctypes[i].nID == nDocType) return nCnt;
            nCnt++;
        }
    }
    return -1;
}
//////////////////////////////////////////////////////////////////////////////
int CDrawDoc::GetTypeFromIndex(int nIndex, BOOL bOpenFileDialog)
{
    int nCnt = 0;
    for (int i=0;i<CMAX_IMAGE_FORMATS;i++){
        if (bOpenFileDialog ? doctypes[i].bRead : doctypes[i].bWrite){
            if (nCnt == nIndex)
//              return i; // PJO - Buglet ?
                return doctypes[i].nID;
            nCnt++;
        }
    }
    ASSERT(FALSE);
    return -1;
}
//////////////////////////////////////////////////////////////////////////////
CString CDrawDoc::GetExtFromType(int nDocType)
{
    for (int i=0;i<CMAX_IMAGE_FORMATS;i++){
        if (doctypes[i].nID == nDocType)
            return doctypes[i].ext;
    }
    return CString(_T(""));
}
//////////////////////////////////////////////////////////////////////////////
CString CDrawDoc::GetDescFromType(int nDocType)
{
    for (int i=0;i<CMAX_IMAGE_FORMATS;i++){
        if (doctypes[i].nID == nDocType)
            return doctypes[i].description;
    }
    return CString(_T(""));
}
//////////////////////////////////////////////////////////////////////////////
BOOL CDrawDoc::GetWritableType(int nDocType)
{
    for (int i=0;i<CMAX_IMAGE_FORMATS;i++){
        if (doctypes[i].nID == nDocType)
            return doctypes[i].bWrite;
    }
    return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
CString CDrawDoc::GetFileTypes(BOOL bOpenFileDialog)
{
    CString str;
    for (int i=0;i<CMAX_IMAGE_FORMATS;i++){
        if (bOpenFileDialog && doctypes[i].bRead){
            str += doctypes[i].description;
            str += (TCHAR)NULL;
            str += doctypes[i].ext;
            str += (TCHAR)NULL;
        } else if (!bOpenFileDialog && doctypes[i].bWrite) {
            str += doctypes[i].description;
            str += (TCHAR)NULL;
            str += doctypes[i].ext;
            str += (TCHAR)NULL;
        }
    }
    return str;
}

//////////////////////////////////////////////////////////////////////////////
void CDrawDoc::Stopwatch(int start0stop1)
{
    if (start0stop1==0)	QueryPerformanceCounter(&m_swStart);
    else {
        QueryPerformanceCounter(&m_swStop);
        if (m_swFreq.LowPart==0 && m_swFreq.HighPart==0) m_etime = -1;
        else {
            m_etime = (float)(m_swStop.LowPart - m_swStart.LowPart);
            if (m_etime < 0) m_etime += 2^32;
            m_etime /= (m_swFreq.LowPart+m_swFreq.HighPart * 2^32);
        }
    }
}
//////////////////////////////////////////////////////////////////////////////

#define EPSILON (0.0000001)

int CDrawDoc::ComputePixel(double x, double y, double &x1, double &y1)
{
  double r, nn;

  if (x==0 && y==0) {
     x1 = x;
     y1 = y;
     return 1;
  }

  nn = sqrt(x*x + y*y);
  r =  (fabs(x) > fabs(y)) ? fabs(nn/x): fabs(nn/y);

  x1 = (double)(r*x);
  y1 = (double)(r*y);

  return 1;
}

//////////////////////////////////////////////////////////////////////////////
void CDrawDoc::OnEditCopy()
{

}

//////////////////////////////////////////////////////////////////////////////
void CDrawDoc::OnEditClear()
{
    CDrawView* pView = m_pActiveMainView;//(CDrawView*)GetNextView(pos);

    // update all the views before the selection goes away
    UpdateAllViews(NULL, HINT_DELETE_SELECTION, &pView->m_selection);
    m_pActiveMainView->OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);

    // now remove the selection from the document
    POSITION pos = pView->m_selection.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = pView->m_selection.GetNext(pos);
        if (pObj->m_pParent == NULL)
        {
            if (pObj->m_nShape == CDrawObj::roi) { // Roi Object를 삭제하면 그에 딸린 Child Object도 삭제한다.
                CDrawRoi *pdrp = (CDrawRoi *)pObj;
                Remove(pdrp->m_pCrossObj);
                pdrp->m_pCrossObj->Remove();
                pdrp->m_pCrossObj = NULL;
            }
            if (pObj->m_nShape == CDrawObj::roipat) { // Pattern Object를 삭제하면 그에 딸린 Child Object도 삭제한다.
                CDrawRoiPat *pdrp = (CDrawRoiPat *)pObj;
                Remove(pdrp->m_pCrossObj);
                pdrp->m_pCrossObj->Remove();
                pdrp->m_pCrossObj = NULL;
                Remove(pdrp->m_pPatObj);
                pdrp->m_pPatObj->Remove();
                pdrp->m_pPatObj = NULL;
            }

            Remove(pObj);
            pObj->Remove();
        }
    }
    pView->m_selection.RemoveAll();

}

