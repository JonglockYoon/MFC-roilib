#pragma once
#include "afxcmn.h"
#include "ui/DrawObj.h"
#include "XListCtrl/XListCtrl.h"

// CFrmProperty 대화 상자입니다.

class CFrmProperty : public CDialog
{
    DECLARE_DYNAMIC(CFrmProperty)

public:
    CFrmProperty(CWnd* pParent = NULL);   // 표준 생성자입니다.
    virtual ~CFrmProperty();

// 대화 상자 데이터입니다.
    enum { IDD = IDD_FORM_ROI };

    CDrawObj* m_pSelObj;
    BOOL m_bFromTspInspView;

    CString GetRoiSet(CString sName);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnInitDialog();
    CXListCtrl lstRoi;
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

    afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnCheckbox(WPARAM, LPARAM);
    afx_msg LRESULT OnEditEnd(WPARAM, LPARAM);
    afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);

    void AddListItem(CRoiData* pData, bool save);
    void DeleteListItem(CRoiData* pData);
    void UpdateListItem(CRoiData* pData);
    void SelectProperty(CRoiData* pData);
    void SetWorkListItem(CString sName);
    void DelWorkList(CString name);
    void NewWorkList(CRoiData* pData);
    void NewAllWorkList();
    void DelAllWorkList();
};
