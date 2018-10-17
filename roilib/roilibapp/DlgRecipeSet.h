#pragma once

#include "RoilibApp.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "Data/RecipeBase.h"

struct	COPY_RECIPE_INFO
{
    CString sSrcRcpName;
    CString sDstRcpName;

    COPY_RECIPE_INFO(){
        sSrcRcpName.Format(_T(""));
        sDstRcpName.Format(_T(""));
    };
};

// CDlgRecipeSet 대화 상자입니다.

class CDlgRecipeSet : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgRecipeSet)

public:
    CDlgRecipeSet(CWnd* pParent = NULL);   // 표준 생성자입니다.
    virtual ~CDlgRecipeSet();

// 대화 상자 데이터입니다.
    enum { IDD = IDD_DLG_RECIPE_SET };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

    DECLARE_MESSAGE_MAP()
public:
    char m_TmpBuffer[256];

    afx_msg void OnBnClickedBtnCopyRcp();
    virtual BOOL OnInitDialog();
    CListCtrl m_ListRecipeList;
    CComboBox m_ComboSrcRecipe;
    CComboBox m_ComboDstRecipe;
    void RecipeList_AddColum(CListCtrl* pListCtl);
    void DefineRecipeList_Update(CListCtrl* pListCtl, CString sCurName = _T(""));
    void ComboCopyList_Update(void);

    int m_nSelectRecipeIndex;
    HITEM m_ItemRecipeInfo[1024];
    afx_msg void OnBnClickedBtnLoadRecipe();
    afx_msg void OnBnClickedBtnDelRecipe();
    afx_msg void OnBnClickedBtnRcpup();
    afx_msg void OnBnClickedBtnRcpdown();
    afx_msg void OnBnClickedBtnNewRecipe();
    afx_msg void OnNMCustomdrawListRecipeMan(NMHDR *pNMHDR, LRESULT *pResult);
    void RecipeInfoListGet(void);
    afx_msg void OnNMClickListRecipeMan(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
