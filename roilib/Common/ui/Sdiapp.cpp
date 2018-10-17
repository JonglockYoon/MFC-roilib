// SDIAPP.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDIDocTemplate

IMPLEMENT_DYNAMIC(CSDIDocTemplate, CSingleDocTemplate)

CSDIDocTemplate::CSDIDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
    CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass,
		int nSplitterID, int nRow, int nCol) :
  CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
	m_nSplitterID = nSplitterID;
	m_nRow = nRow;
	m_nCol = nCol;
	m_pView = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CSDIWinApp construction

IMPLEMENT_DYNAMIC(CSDIWinApp, CWinAppEx)

CSDIWinApp::CSDIWinApp()
{
}

CSDIDocTemplate* CSDIWinApp::GetTemplate(CView* pView)
{
	POSITION pos;
	CSDIDocTemplate* pTemplate = NULL;

	pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		pTemplate = (CSDIDocTemplate*)GetNextDocTemplate(pos);
		if (pTemplate->m_pView == pView) return pTemplate;
	}
	return NULL;
}

CSDIDocTemplate* CSDIWinApp::GetTemplate(CRuntimeClass *pViewClass)
{
	POSITION pos;
	CSDIDocTemplate* pTemplate = NULL;

	pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		pTemplate = (CSDIDocTemplate*)GetNextDocTemplate(pos);
		if (pTemplate->m_pView->IsKindOf(pViewClass)) return pTemplate;
	}
	return NULL;
}

CSDIDocTemplate* CSDIWinApp::GetFirstTemplate()
{
	POSITION pos;
	CSDIDocTemplate* pTemplate = NULL;
	pos = GetFirstDocTemplatePosition();
	pTemplate = (CSDIDocTemplate*)GetNextDocTemplate(pos);
	ASSERT(pTemplate != NULL);
	return pTemplate;
}

//  DDX_MyRadio(), which is a modified DDX_Radio().
// 
void AFXAPI DDX_MyRadio(CDataExchange* pDX, int nIDC, int& value) // must be first in a group of auto radio buttons
{
   HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
   ASSERT(::GetWindowLong(hWndCtrl, GWL_STYLE) & WS_GROUP);
   ASSERT(::SendMessage(hWndCtrl, WM_GETDLGCODE, 0, 0L) & DLGC_RADIOBUTTON);
   if( pDX->m_bSaveAndValidate ) value = -1;     // value if none found
   // walk all children in group
   int iButton = 0;
   do
   {
      if (::SendMessage(hWndCtrl, WM_GETDLGCODE, 0, 0L) & DLGC_RADIOBUTTON)
      {
		// control in group is a radio button
         if( pDX->m_bSaveAndValidate )
         {
            if( ::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L) != 0 )
            {
               ASSERT(value == -1);    // only set once
               value = iButton;
            }
         }
         else
         {
			// select button
            ::SendMessage( hWndCtrl, BM_SETCHECK, (iButton == value), 0L);
         }
         iButton++;
      }
      else
      {
         TRACE( _T("Warning: skipping non-radio button in group.\n") );
      }
      hWndCtrl = ::GetWindow( hWndCtrl, GW_HWNDNEXT );
   } while(hWndCtrl!=NULL &&!(GetWindowLong(hWndCtrl,GWL_STYLE)&WS_GROUP));
}