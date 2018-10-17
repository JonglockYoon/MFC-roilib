// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// 모든 권리 보유.

#pragma once

#include "FrmProperty.h"

class CDrawObj;
class CRoiBase;

class CPropertiesWnd : public CDockablePane
{
// 생성입니다.
public:
    CPropertiesWnd();

    void InitPropList();
    void AddPropertyItem(CDrawObj *pObj);
    void DeletePropertyItem(CDrawObj *pObj);
    void UpdatePropertyItem(CDrawObj *pObj);
    void SetPropListFont();

// 특성입니다.
public:

protected:
    CFont m_fntPropList;
    CFrmProperty m_wndPropList;

// 구현입니다.
public:
    virtual ~CPropertiesWnd();
    void SetSelectProperty(CDrawObj* pObj);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnExpandAllProperties();
    afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);

    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
    afx_msg void OnSetFocus(CWnd* pOldWnd);

    afx_msg void OnRoiHiddenButtonPressed();
    afx_msg void OnRoiShowButtonPressed();
    afx_msg void OnRoiHiddenSelfButtonPressed();
    afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);

    DECLARE_MESSAGE_MAP()
};

