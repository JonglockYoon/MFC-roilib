// SDIFrame.cpp : implementation of the CSDIFrameWnd class
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitInfo class

CSplitInfo::CSplitInfo()
{
	m_MaxRow = m_MaxCol = 0;
}

CSplitInfo &CSplitInfo::operator=(const CSplitInfo &SplitInfo)
{
	// function only for CMap
	return *this;
}
  
/////////////////////////////////////////////////////////////////////////////
// CSDIFrameWnd

IMPLEMENT_DYNCREATE(CSDIFrameWnd, CFrameWndEx)

BEGIN_MESSAGE_MAP(CSDIFrameWnd, CFrameWndEx)
	//{{AFX_MSG_MAP(CSDIFrameWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDIFrameWnd construction/destruction

CSDIFrameWnd::CSDIFrameWnd()
{
}

CSDIFrameWnd::~CSDIFrameWnd()
{
}

BOOL CSDIFrameWnd::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CView* pView;
	CSDIWinApp* pApp = (CSDIWinApp*)AfxGetApp();
	ASSERT_KINDOF(CSDIWinApp, pApp);

	// hold informations from DocTemplates to create splitter
	POSITION TmpPos = pApp->GetFirstDocTemplatePosition();
	while (TmpPos)
	{
		CSDIDocTemplate* pTemplate = (CSDIDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
		ASSERT_KINDOF(CSDIDocTemplate, pTemplate);
		if (pTemplate->m_nSplitterID != -1)
		{
			// find or create the splitter ID in the splitter map
			int nID = pTemplate->m_nSplitterID;
			m_SplitInfo[nID].m_MaxRow = max(m_SplitInfo[nID].m_MaxRow, pTemplate->m_nRow + 1);
			m_SplitInfo[nID].m_MaxCol = max(m_SplitInfo[nID].m_MaxCol, pTemplate->m_nCol + 1);
		}
	}
  
	// create the splitter and the views in the splitter
	POSITION MapPos = m_SplitInfo.GetStartPosition();
	while (MapPos)
	{
		int nID; CSplitInfo SplitInfo;
		m_SplitInfo.GetNextAssoc(MapPos, nID, SplitInfo);
		if (m_SplitInfo[nID].m_MaxRow && m_SplitInfo[nID].m_MaxCol)
		{
			CRect rect; GetClientRect(&rect);
			int cx = rect.Width(); int cy = rect.Height();
			
			// normaly always ToolBar & StatusBar
			cy = cy - 56; if (cy < 56) cy = 0;

			if (m_SplitInfo[nID].m_MaxCol) cx /= m_SplitInfo[nID].m_MaxCol;
			if (m_SplitInfo[nID].m_MaxRow) cy /= m_SplitInfo[nID].m_MaxRow;
			CSize sizeDefault(cx, cy);

			m_SplitInfo[nID].m_wndSplitter.CreateStatic(this, m_SplitInfo[nID].m_MaxRow, m_SplitInfo[nID].m_MaxCol);
			// disable view ID to use for next splitter
			m_SplitInfo[nID].m_wndSplitter.SetDlgCtrlID(0);
	
			TmpPos = pApp->GetFirstDocTemplatePosition();
			while (TmpPos)
			{
				CSDIDocTemplate* pTemplate = (CSDIDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
				if (pTemplate->m_nSplitterID == nID)
				{
					// create view
					int nRow = pTemplate->m_nRow, nCol = pTemplate->m_nCol;
					pContext->m_pNewDocTemplate = pTemplate;
					pContext->m_pNewViewClass = pTemplate->GetViewClass();
					if (!m_SplitInfo[nID].m_wndSplitter.CreateView(nRow, nCol,
						pContext->m_pNewViewClass, sizeDefault, pContext)) return FALSE;
					pTemplate->m_pView = (CView*)m_SplitInfo[nID].m_wndSplitter.GetPane(nRow, nCol);
				}
			}
			// disable view ID to use for next splitter view
			TmpPos = pApp->GetFirstDocTemplatePosition();
			while (TmpPos)
			{
				CSDIDocTemplate* pTemplate = (CSDIDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
				if (pTemplate->m_nSplitterID == nID)
					pTemplate->m_pView->SetDlgCtrlID(0);
			}
		}
	}

	// create the other views (no splitter)
	TmpPos = pApp->GetFirstDocTemplatePosition();
	while (TmpPos)
	{
		CSDIDocTemplate* pTemplate = (CSDIDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
		if (pTemplate->m_nSplitterID == -1)
		{
			pContext->m_pNewViewClass = pTemplate->GetViewClass();
			pView = (CView*)CreateView(pContext, AFX_IDW_PANE_FIRST);
			ASSERT(pView != NULL);
			pTemplate->m_pView = pView;
			// disable view ID to use for next view
			pView->SetDlgCtrlID(0);
		}
	}
	SetActiveView(NULL);

	CSDIDocTemplate *pTemplate;

	// litle problem ! if a CRichEditView is in the view list
	// we must activate it first to update the layout correctly
	pTemplate = pApp->GetTemplate(RUNTIME_CLASS(CFormView));
	if (pTemplate) ActivateView(pTemplate->GetViewClass());

	// activate the first doctemplate view
	pTemplate = pApp->GetFirstTemplate();
	ActivateView(pTemplate->GetViewClass());

	return TRUE;
}

void CSDIFrameWnd::ActivateView(CRuntimeClass* pViewClass)
{
	CSDIWinApp* pApp = (CSDIWinApp*)AfxGetApp();
	CView* pActiveView = GetActiveView();
	CSDIDocTemplate* pNewTemplate = pApp->GetTemplate(pViewClass);
	CSDIDocTemplate* pActiveTemplate = pApp->GetTemplate(pActiveView);

	// the view class doesn't exist
	if (pNewTemplate == NULL) return;
	
	// the new template is the active template, do nothing
	if (pNewTemplate == pActiveTemplate) return;

	// the active and new template are in the same splitter, change the active view
	if (pActiveTemplate != NULL)
	{
		if (pActiveTemplate->m_nSplitterID != -1 &&
		pActiveTemplate->m_nSplitterID == pNewTemplate->m_nSplitterID)
		{
			UpdateResource(pNewTemplate);
			SetActiveView(pNewTemplate->m_pView);
			return;
		}

		// the active view is in a splitter, hide all views in the splitter and the splitter
		if (pActiveTemplate->m_nSplitterID != -1)
		{
			POSITION TmpPos;
			CSDIDocTemplate* pTemplate;
			int nID = pActiveTemplate->m_nSplitterID;
			TmpPos = pApp->GetFirstDocTemplatePosition();
			while (TmpPos)
			{
				pTemplate = (CSDIDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
				if (pTemplate->m_nSplitterID == nID)
				{
					pTemplate->m_pView->ShowWindow(SW_HIDE);
					pTemplate->m_pView->SetDlgCtrlID(0);
				}
			}
			// hide the splitter
			m_SplitInfo[nID].m_wndSplitter.SetDlgCtrlID(0);
			m_SplitInfo[nID].m_wndSplitter.ShowWindow(SW_HIDE);
		}
		// the active view is not in a splitter, hide this view
		else
		{
			pActiveTemplate->m_pView->SetDlgCtrlID(0);
			pActiveTemplate->m_pView->ShowWindow(SW_HIDE);
		}
	}
  
	// the new view is in a splitter, show all splitter views and the splitter
	if (pNewTemplate->m_nSplitterID != -1)
	{
		POSITION TmpPos;
		CSDIDocTemplate* pTemplate;
		int nID = pNewTemplate->m_nSplitterID;
		TmpPos = pApp->GetFirstDocTemplatePosition();
		while (TmpPos)
		{
			pTemplate = (CSDIDocTemplate*)pApp->GetNextDocTemplate(TmpPos);
			if (pTemplate->m_nSplitterID == nID)
			{
				int nRow = pTemplate->m_nRow;
				int nCol = pTemplate->m_nCol;
				int nDlgID = m_SplitInfo[nID].m_wndSplitter.IdFromRowCol(nRow, nCol);
				pTemplate->m_pView->SetDlgCtrlID(nDlgID);
				pTemplate->m_pView->ShowWindow(SW_SHOW);
			}
		}
		// show the splitter
		m_SplitInfo[nID].m_wndSplitter.SetDlgCtrlID(AFX_IDW_PANE_FIRST);
		m_SplitInfo[nID].m_wndSplitter.ShowWindow(SW_SHOW);
	}
	// the new view is not in a splitter, active the new view
	else
	{
		pNewTemplate->m_pView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
		pNewTemplate->m_pView->ShowWindow(SW_SHOW);
	}

	UpdateResource(pNewTemplate);

	// display and update the new view
	SetActiveView(pNewTemplate->m_pView);
	RecalcLayout();
}

BOOL CSDIFrameWnd::IsViewActive(CRuntimeClass* pViewClass)
{
	CView* pActiveView = GetActiveView();
	if (pActiveView == NULL) return FALSE;
	return (pActiveView->GetRuntimeClass() == pViewClass);
}

void CSDIFrameWnd::UpdateResource(CSDIDocTemplate *pNewTemplate)
{
	CSDIWinApp* pApp = (CSDIWinApp*)AfxGetApp();

	// support for view context menu, accelerator and help ID like MDI
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(pNewTemplate->GetResourceID()), RT_MENU);

	// if menu is not the default menu, destroy it
	HMENU hMenu = ::GetMenu(m_hWnd);
	if (hMenu != m_hMenuDefault) ::DestroyMenu(hMenu);
	// load menu from doctemplate
	hMenu = ::LoadMenu(hInst, MAKEINTRESOURCE(pNewTemplate->GetResourceID()));
	// if no menu, get default
	if (hMenu == NULL) hMenu = m_hMenuDefault;
	// load menu
	// ::SetMenu(m_hWnd, hMenu);

	// load new accelerator table
	HACCEL hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(pNewTemplate->GetResourceID()));
	// if no table for this template, load default
	if (hAccel == NULL) ::LoadAccelerators(hInst, MAKEINTRESOURCE(pApp->GetFirstTemplate()->GetResourceID()));
	
	// chage help ID for this view
	m_nIDHelp = pNewTemplate->GetResourceID();

	// change the title of the document
	pNewTemplate->GetDocString(m_strTitle, CDocTemplate::windowTitle);
	OnUpdateFrameTitle(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CSDIFrameWnd message handlers
