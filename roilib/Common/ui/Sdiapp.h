// SdiApp.h : main header file for the CSDIWinApp class
//

/////////////////////////////////////////////////////////////////////////////
// CSDIDocTemplate class

class CSDIDocTemplate : public CSingleDocTemplate
{
	DECLARE_DYNAMIC(CSDIDocTemplate)
// Constructors
public:
  CSDIDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
    CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass,
      int nSplitterID = -1, int nRow = 0, int nCol = 0);

// Attributes
public:
  int m_nSplitterID;		// -1, no splitter else splitter ID (0, 1,...)
  int m_nRow;			// if splitter, row number (0, 1,...)
  int m_nCol;			// if splitter, col number (0, 1,...)

  CView* m_pView;		// view pointer

  // these overrides to retreive the view class and resource ID
  CRuntimeClass* GetViewClass()
    { return m_pViewClass; }
  UINT GetResourceID()
	{ return m_nIDResource; }
};

/////////////////////////////////////////////////////////////////////////////
// CSDIWinApp class

class CSDIWinApp : public CWinAppEx
{
	DECLARE_DYNAMIC(CSDIWinApp)
public:
	CSDIWinApp();
	CSDIDocTemplate* GetTemplate(CView* pView);
	CSDIDocTemplate* GetTemplate(CRuntimeClass *pViewClass);
	CSDIDocTemplate* GetFirstTemplate();
};

void AFXAPI DDX_MyRadio(CDataExchange* pDX, int nIDC, int& value); // must be first in a group of auto radio buttons