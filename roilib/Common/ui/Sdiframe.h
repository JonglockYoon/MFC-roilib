// SDIFRAME.h : interface of the CSDIFrameWnd class
//
/////////////////////////////////////////////////////////////////////////////

class CSplitInfo
{
public:
  int m_MaxRow, m_MaxCol;
  CSplitterWnd m_wndSplitter;
  CSplitInfo::CSplitInfo();
  CSplitInfo &operator=(const CSplitInfo &SplitInfo);
};

class CSDIFrameWnd : public CFrameWndEx
{
protected: // create from serialization only
	CSDIFrameWnd();
	DECLARE_DYNCREATE(CSDIFrameWnd)

// Attributes
protected:
	CMap <int, int, CSplitInfo, CSplitInfo&> m_SplitInfo;
public:

// Operations
public:
	void ActivateView(CRuntimeClass *pViewClass);
	BOOL IsViewActive(CRuntimeClass *pViewClass);
	void UpdateResource(CSDIDocTemplate* pNewTemplate);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIFrameWnd)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSDIFrameWnd();

// Generated message map functions
protected:
	//{{AFX_MSG(CSDIFrameWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
