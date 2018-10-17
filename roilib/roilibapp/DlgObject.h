#pragma once
#include "afxwin.h"


// CDlgObject dialog

class CDlgObject : public CDialog
{
    DECLARE_DYNAMIC(CDlgObject)

public:
    CDlgObject(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDlgObject();

// Dialog Data
    enum { IDD = IDD_DLG_OBJECT };
    BOOL	m_bDeleteFlag;//해당 Object에 대해 삭제버튼을 눌렀을 경우 TRUE로 설정한다.

    CRoiData m_roiData;

    /////////////////////////////////////////////////////////////////////////////////
    vector<std::basic_string<TCHAR>> m_sParentName;
    int	GetParentIndex(CString sName);
    /////////////////////////////////////////////////////

    void ObjectListSet();
    void ObjectListGet();
    CPropertyGrid  m_ListObjectInfomation;
    HITEM m_hEditItem[260];

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedBtnDeleteObject();
    CComboBox m_CombonInspectType;
    afx_msg void OnCbnSelchangeComboInsplist();
    virtual void OnOK();

public:
    afx_msg void OnDestroy();

    CStatic m_SrcImagePreView;
    double m_SrcImageRatio;
    double m_ResultImageRatio;
    BOOL m_bImageViewFitSize;
    BOOL m_bResultImageView;

    afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
    virtual void PostNcDestroy();
public:
    afx_msg void OnBnClickedOk();
};
