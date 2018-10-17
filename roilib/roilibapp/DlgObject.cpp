// DlgObject.cpp : implementation file
//

#include "stdafx.h"
#include "RoilibApp.h"
#include "DlgObject.h"
#include "MainFrm.h"

// CDlgObject dialog

IMPLEMENT_DYNAMIC(CDlgObject, CDialog)

CDlgObject::CDlgObject(CWnd* pParent /*=NULL*/)
    : CDialog(CDlgObject::IDD, pParent)
{
    m_bDeleteFlag = FALSE;
}

CDlgObject::~CDlgObject()
{
}

void CDlgObject::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_INSPLIST, m_CombonInspectType);
    DDX_Control(pDX, IDC_LIST_OBJECTINFO, m_ListObjectInfomation);
}


BEGIN_MESSAGE_MAP(CDlgObject, CDialog)
    ON_BN_CLICKED(IDC_BTN_DELETE_OBJECT, &CDlgObject::OnBnClickedBtnDeleteObject)
    ON_CBN_SELCHANGE(IDC_COMBO_INSPLIST, &CDlgObject::OnCbnSelchangeComboInsplist)
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDOK, &CDlgObject::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgObject message handlers

BOOL CDlgObject::OnInitDialog()
{
    CDialog::OnInitDialog();
    CString str;
    int nCnt = 0;
    int nIndex;

    int nCbIdx = 0;
    switch(m_roiData.m_nObjectType)
    {
        case _ObjType_Patt:
            for(int i=_Inspect_Patt_Start+1; i<_Inspect_Patt_End; i++){
                m_CombonInspectType.InsertString(nCnt++, g_cRecipeData->m_sInspList[i]);
            }
            nCbIdx = m_roiData.m_nInspectType - (_Inspect_Patt_Start + 1);
            break;
        case _ObjType_Roi:
            for(int i=_Inspect_Roi_Start+1; i<_Inspect_Roi_End; i++){
                m_CombonInspectType.InsertString(nCnt++, g_cRecipeData->m_sInspList[i]);
            }
            nCbIdx = m_roiData.m_nInspectType - (_Inspect_Roi_Start + 1);
            break;
        case _ObjType_Point:
            for(int i=_Inspect_Point_Start+1; i<_Inspect_Point_End; i++){
                m_CombonInspectType.InsertString(nCnt++, g_cRecipeData->m_sInspList[i]);
            }
            nCbIdx = m_roiData.m_nInspectType - (_Inspect_Point_Start + 1);
            break;
    }
    SetDlgItemText(IDC_STATIC_OBJNAME, m_roiData.m_sName);
    m_CombonInspectType.SetCurSel(nCbIdx);

    ObjectListSet();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgObject::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN){
        //switch(pMsg->wParam){
        //	case VK_ESCAPE:
        //	case VK_RETURN:
        //		pMsg->wParam = NULL;
        //		pMsg->message = NULL;
        //		break;

        //}
    }

    return CDialog::PreTranslateMessage(pMsg);
}

int	CDlgObject::GetParentIndex(CString sName){
    int nRet = 0;
    for(int i=0; i<(int)m_sParentName.size(); i++){
        if(m_sParentName[i].data() == sName){
            nRet = i;
            break;
        }
    }
    return nRet;
}
void CDlgObject::ObjectListSet(){
    HSECTION hs;
    CString str;
    int nIndex = 0;
    int nValue= 0;

    //std::basic_string<TCHAR> sValue;
    m_ListObjectInfomation.ResetContents();
    m_ListObjectInfomation.SetGutterWidth(200);

    int size = m_roiData.m_vecParams.size();
    //hs = m_ListObjectInfomation.AddSection((_T("Parameter")), false);
    int nOldStep = -1;
    for (int i = 0; i < size; i++)
    {
        CParam *c = &m_roiData.m_vecParams[i];
        if (nOldStep != c->stepType) {
            hs = m_ListObjectInfomation.AddSection(StepTypeValue[c->stepType].GetBuffer(), false);
            nOldStep = c->stepType;
        }
        if (c->valueType == _BoolValue) {
            m_hEditItem[nIndex++] = m_ListObjectInfomation.AddBoolItem(hs, (LPTSTR)c->Name.c_str(), _tstoi(c->Value.c_str()));
        }
        else if (c->valueType == _IntValue) {
            m_hEditItem[nIndex++] = m_ListObjectInfomation.AddIntegerItem(hs, (LPTSTR)c->Name.c_str(), _tstoi(c->Value.c_str()));
        }
        else if (c->valueType == _DoubleValue) {
            m_hEditItem[nIndex++] = m_ListObjectInfomation.AddDoubleItem(hs, (LPTSTR)c->Name.c_str(), _tstof(c->Value.c_str()));
        }
        else if (c->valueType == _StringValue) {
            m_hEditItem[nIndex++] = m_ListObjectInfomation.AddStringItem(hs, (LPTSTR)c->Name.c_str(), c->Value.c_str());
        }
        else if (c->valueType == _ComboValue) { // ,로 구분된 선택 Table처리
            c->m_vecDetail.clear();
            TCHAR seps[] = _T(",");
            TCHAR *token;
            TCHAR szTok[256];
            _tcscpy(szTok, c->Detail.c_str());
            int seq = 0;
            token = _tcstok(szTok, seps);
            while (token != NULL)
            {
                c->m_vecDetail.push_back(token);
                token = _tcstok(NULL, seps);
            }
            m_hEditItem[nIndex++] = m_ListObjectInfomation.AddComboItem(hs, (LPTSTR)(LPTSTR)c->Name.c_str(), c->m_vecDetail, _ttoi(c->Value.c_str()));
        }
    }

    m_ListObjectInfomation.UpdateData();
}

void CDlgObject::ObjectListGet(){
    std::basic_string<TCHAR> temp_str;
    CString str;
    int nIndex = 0;
    int nValue;
    double dValue;
    bool bValue;

    int size = m_roiData.m_vecParams.size();
    for (int i = 0; i < size; i++)
    {
        CParam *c = &m_roiData.m_vecParams[i];
        if (c->valueType == _BoolValue) {
            m_ListObjectInfomation.GetItemValue(m_hEditItem[nIndex++], bValue);
            str.Format(_T("%d"), bValue);
        }
        else if (c->valueType == _IntValue) {
            m_ListObjectInfomation.GetItemValue(m_hEditItem[nIndex++], nValue);
            str.Format(_T("%d"), nValue);
        }
        else if (c->valueType == _DoubleValue) {
            m_ListObjectInfomation.GetItemValue(m_hEditItem[nIndex++], dValue);
            str.Format(_T("%.3f"), dValue);
        }
        else if (c->valueType == _StringValue) {
            m_ListObjectInfomation.GetItemValue(m_hEditItem[nIndex++], temp_str);
            str = temp_str.c_str();
        }
        else if (c->valueType == _ComboValue) { // ,로 구분된 선택 Table처리
            m_ListObjectInfomation.GetItemValue(m_hEditItem[nIndex++], nValue);
            str.Format(_T("%d"), nValue);
        }
        m_roiData.m_vecParams[i].Value = str.GetBuffer();
    }
}

void CDlgObject::OnBnClickedBtnDeleteObject()
{
    m_bDeleteFlag = TRUE;
    OnOK();
}

void CDlgObject::OnCbnSelchangeComboInsplist()
{
    int nIndex;
    nIndex = m_CombonInspectType.GetCurSel();
    if(nIndex < 0) return;

    InspectType		nInspectType = (InspectType)0;

    switch(m_roiData.m_nObjectType){
        case _ObjType_Patt:
            nInspectType = (InspectType)(m_CombonInspectType.GetCurSel() + (_Inspect_Patt_Start + 1));
            break;
        case _ObjType_Roi:
            nInspectType = (InspectType)(m_CombonInspectType.GetCurSel() + (_Inspect_Roi_Start + 1));
            break;
        case _ObjType_Point:
            nInspectType = (InspectType)(m_CombonInspectType.GetCurSel() + (_Inspect_Point_Start + 1));
            break;
    }

    if (nInspectType >= 0){
        m_roiData.m_nInspectType = nInspectType;
        m_roiData.m_vecParams.clear();

        extern ParamTable paramTable[];
        ParamTable* pParamTab = paramTable;
        for (int n = 0;; n++)
        {
            if (pParamTab[n].nInspectType == _Inspect_Type_End)
                break;
            if (pParamTab[n].nInspectType == nInspectType)
                m_roiData.m_vecParams.push_back(pParamTab[n].param);
        }

    }

    ObjectListSet();
}

void CDlgObject::OnOK()
{
    CDialog::OnOK();
}

void CDlgObject::OnDestroy()
{
    CDialog::OnDestroy();
}

void CDlgObject::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
}

void CDlgObject::PostNcDestroy()
{
    CDialog::PostNcDestroy();
}


void CDlgObject::OnBnClickedOk()
{
    CDialog::OnOK();
}
