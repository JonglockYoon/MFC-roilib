#include "stdafx.h"
#include "IniUtil.h"
#include "Utils/Tokenizer.h"
#include "Data/RecipeData.h"
#include "Utils/Utils.h"
#include "Utils/Direct_Info.h"
#include "Config.h"
#include "RoilibApp.h"
#include "MainFrm.h"
#include "RoiBase.h"

CRecipeData * g_cRecipeData = NULL;

ParamTable paramTable[] = {


		_Inspect_Patt_Identify,  CParam(_ProcessValue1, _T("Pattern matching rate"), _DoubleValue, _T("60")),
		_Inspect_Patt_Identify,  CParam(_ProcessValue1, _T("Rotate angle"), _IntValue, _T("0")),
		_Inspect_Patt_Identify,  CParam(_ProcessValue1, _T("Angle step"), _IntValue, _T("2")),
		_Inspect_Patt_Identify,  CParam(_ProcessValue2, _T("Shape matching rate"), _DoubleValue, _T("70")),
		_Inspect_Patt_Identify,  CParam(_ProcessValue2, _T("Low Threshold"), _IntValue, _T("0")),
		_Inspect_Patt_Identify,  CParam(_ProcessValue2, _T("High Threshold"), _IntValue, _T("100")),
		_Inspect_Patt_Identify,  CParam(_ProcessValue2, _T("Noise out 1"), _IntValue, _T("1")),	// -1 : Open - 작은 White blob 들을 없앤다.
		_Inspect_Patt_Identify,  CParam(_ProcessValue2, _T("Noise out 2"), _IntValue, _T("-1")),	// 1 : Close - White blob 들을 묶는다..
		_Inspect_Patt_Identify,  CParam(_ProcessValue2, _T("Filter blob"), _ComboValue, _T("2"), _T("None,LargeArea,LargeDiameter")),

		_Inspect_Patt_MatchShapes,  CParam(_ProcessValue1, _T("Low Threshold"), _IntValue, _T("0")),
		_Inspect_Patt_MatchShapes,  CParam(_ProcessValue1, _T("High Threshold"), _IntValue, _T("100")),
		_Inspect_Patt_MatchShapes,  CParam(_ProcessValue1, _T("Noise out 1"), _IntValue, _T("1")),	// -1 : Open - 작은 White blob 들을 없앤다
		_Inspect_Patt_MatchShapes,  CParam(_ProcessValue1, _T("Noise out 2"), _IntValue, _T("-1")),	// 1 : Close - White blob 들을 묶는다.
		_Inspect_Patt_MatchShapes,  CParam(_ProcessValue1, _T("Shape matching rate"), _DoubleValue, _T("85")),
		//_Inspect_Patt_MatchShapes,  CParam(_PriorityValue, _T("Priority"), _IntValue, _T("5")),

		_Inspect_Patt_FeatureMatch, CParam(_ProcessValue1, _T("Method"), _ComboValue, _T("2"), _T("SURF,SIFT,ORB")),
		_Inspect_Patt_FeatureMatch, CParam(_ProcessValue1, _T("Param1"), _IntValue, _T("2000")),
		_Inspect_Patt_FeatureMatch, CParam(_ProcessValue1, _T("kDistanceCoef"), _IntValue, _T("5")),
		_Inspect_Patt_FeatureMatch, CParam(_ProcessValue1, _T("kMaxMatchingSize"), _IntValue, _T("200")),


		_Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Noise out area"), _IntValue, _T("3")), // pixel size
        _Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Noise out 1"), _IntValue, _T("-2")),	// -1 : Open - 작은 White blob 들을 없앤다.
        _Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Noise out 2"), _IntValue, _T("2")),	// 1 : Close - White blob 들을 묶는다.
        _Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Smooth Use"), _ComboValue, _T("0"), _T("No,Yes")),
        _Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Smooth method"), _ComboValue, _T("2"), _T("BLUR_NO_SCALE,BLUR,GAUSSIAN,CV_MEDIAN,BILATERAL ")),
        _Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Smooth size"), _IntValue, _T("7")),
        _Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Minimum circle radius"), _DoubleValue, _T("70")),
        _Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Maximum circle radius"), _DoubleValue, _T("150")),
        _Inspect_Roi_Circle, CParam(_ProcessValue1, _T("Maximum Threshold Canny"), _IntValue, _T("100")),

        _Inspect_BarCode, CParam(_ProcessValue1, _T("Type"), _ComboValue, _T("English")),

		//_Inspect_Teseract, CParam(_ProcessValue1, _T("Type"), _ComboValue, _T("Multiformat")),
		_Inspect_Teseract, CParam(_ProcessValue1, _T("Low Threshold"), _IntValue, _T("100")),
		_Inspect_Teseract, CParam(_ProcessValue1, _T("High Threshold"), _IntValue, _T("255")),
		_Inspect_Teseract, CParam(_ProcessValue1, _T("Min Size Y"), _IntValue, _T("50")),
		_Inspect_Teseract, CParam(_ProcessValue1, _T("Max Size Y"), _IntValue, _T("10000")),
		_Inspect_Teseract, CParam(_ProcessValue1, _T("Noise out 1"), _IntValue, _T("-2")),	// -1 : Open - 작은 White blob 들을 없앤다.
		_Inspect_Teseract, CParam(_ProcessValue1, _T("Noise out 2"), _IntValue, _T("2")),	// 1 : Close - White blob 들을 묶는다.
		_Inspect_Teseract, CParam(_ProcessValue1, _T("Expansion 1"), _IntValue, _T("-2")),	// -1 : Erode - 	
		_Inspect_Teseract, CParam(_ProcessValue1, _T("Expansion 2"), _IntValue, _T("0")),	// 1 : Dilate - 팽창연산
		_Inspect_Teseract, CParam(_ProcessValue2, _T("Noise out 3"), _IntValue, _T("0")),	// -1 : Open - 작은 White blob 들을 없앤다.
		_Inspect_Teseract, CParam(_ProcessValue2, _T("Noise out 4"), _IntValue, _T("0")),	// 1 : Close - White blob 들을 묶는다.
		_Inspect_Teseract, CParam(_ProcessValue2, _T("Expansion 3"), _IntValue, _T("0")),	// -1 : Erode - 	
		_Inspect_Teseract, CParam(_ProcessValue2, _T("Expansion 4"), _IntValue, _T("0")),	// 1 : Dilate - 팽창연산
		_Inspect_Teseract, CParam(_ProcessValue3, _T("Size X(%)"), _IntValue, _T("100")),
		_Inspect_Teseract, CParam(_ProcessValue3, _T("Size Y(%)"), _IntValue, _T("100")),
		_Inspect_Teseract, CParam(_ProcessValue3, _T("Smooth Use"), _ComboValue, _T("0"), _T("No,Yes")),
		_Inspect_Teseract, CParam(_ProcessValue3, _T("Smooth method"), _ComboValue, _T("2"), _T("BLUR_NO_SCALE,BLUR,GAUSSIAN,CV_MEDIAN,BILATERAL ")),
		_Inspect_Teseract, CParam(_ProcessValue3, _T("Smooth size"), _IntValue, _T("7")),

        _Inspect_Type_End, CParam(_FilterValue, _T(""), _IntValue, _T("")), // 반드시 있어야한다.
};

CRecipeData::CRecipeData(void)
        : CRecipeBase()
{
        m_sFileName = _T("");
        for (int i = 0; i<_Inspect_Point_End; i++){
                m_sInspList[i].Format(_T(""));
        }
        std::vector<CRoiData*> element1;
        for (int i = 0; i < MAX_CH_NUM; i++)
                m_vecWorkRoiData.push_back(element1);

        InitParamData();
}

CRecipeData::~CRecipeData(void)
{
        ClearImageBuff();
}

void CRecipeData::InitParamData()
{
	m_sInspList[_Inspect_Patt_Identify].Format(_T("Pattern Identify"));
	m_sInspList[_Inspect_Patt_MatchShapes].Format(_T("Pattern Shape Match"));
	m_sInspList[_Inspect_Patt_FeatureMatch].Format(_T("Feature Match"));


        m_sInspList[_Inspect_Roi_Circle].Format(_T("Find Circle"));
        m_sInspList[_Inspect_BarCode].Format(_T("BarCode"));
		m_sInspList[_Inspect_Teseract].Format(_T("OCR"));
}

void CRecipeData::ClearImageBuff()
{
        for (int i = 0; i<m_vecRoiData.size(); i++)
        {
                CRoiData* pData = m_vecRoiData[i];

                if (pData->iplTemplate){
                        cvReleaseImage(&pData->iplTemplate);
                        pData->iplTemplate = NULL;
                }
        }
}
void CRecipeData::LoadRecipeData()
{
        CIniUtil* ini = new CIniUtil(m_sFileName);
        CString str, sub, strParam, sPatternPath;
        CString parentName;

        CRecipeBase::LoadRecipeData();

        CParam param;
        sPatternPath.Format(_T("%s\\Recipe\\%s"), theApp.g_sRootPath, gCfg.m_sLastRecipeName);

        if (theApp.m_vecProcessingClass.size() <= theApp.m_nSelectCh)
                return;
        CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[theApp.m_nSelectCh];

        int size = ini->ReadInt(_T("RECIPE_INFO"), _T("TOTAL_ROI_NUMBER"), 0);
        for (int nSeq = 0; nSeq < size; nSeq++)
        {
                CRoiData* pRoiData = new CRoiData();
                pRoiData->iplTemplate = NULL;
                str.Format(_T("ROI%02d"), nSeq);
                pRoiData->m_nCh = ini->ReadInt(str, _T("CH"), 0);;
                _tcscpy(pRoiData->m_sName, ini->ReadString(str, _T("NAME"), _T("")));
                pRoiData->m_nObjectType = ini->ReadInt(str, _T("OBJECT_TYPE"), 0); // 패턴, 영역, 포인터
                pRoiData->m_nInspectType = (InspectType)ini->ReadInt(str, _T("INSPECT_TYPE"), 0); // InspectType 참조

                switch (pRoiData->m_nObjectType) {
                case _ObjType_Patt:
                        pRoiData->m_RoiArea.left = ini->ReadInt(str, _T("ROILEFT"), 0);
                        pRoiData->m_RoiArea.top = ini->ReadInt(str, _T("ROITOP"), 0);
                        pRoiData->m_RoiArea.right = ini->ReadInt(str, _T("ROIRIGHT"), 0);
                        pRoiData->m_RoiArea.bottom = ini->ReadInt(str, _T("ROIBOTTOM"), 0);
                        pRoiData->ptnRoi.left = ini->ReadInt(str, _T("PATLEFT"), 0);
                        pRoiData->ptnRoi.top = ini->ReadInt(str, _T("PATTOP"), 0);
                        pRoiData->ptnRoi.right = ini->ReadInt(str, _T("PATRIGHT"), 0);
                        pRoiData->ptnRoi.bottom = ini->ReadInt(str, _T("PATBOTTOM"), 0);
                        pRoiData->m_dCenterPos.x = ini->ReadInt(str, _T("PATCENTERX"), 0);
                        pRoiData->m_dCenterPos.y = ini->ReadInt(str, _T("PATCENTERY"), 0);

						if (pRoiData->iplTemplate) {
							cvReleaseImage(&pRoiData->iplTemplate);
							pRoiData->iplTemplate = NULL;
						}

						sub.Format(_T("%s\\Recipe\\%s\\Pat%02d\\%s.bmp"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, pRoiData->m_nCh, pRoiData->m_sName);
						if (::PathFileExists(sub))
						{
							CT2A ascii(sub);
							pRoiData->iplTemplate = cvLoadImage(ascii, 0);
						}

                        break;
                case _ObjType_Roi:
                        pRoiData->m_RoiArea.left = ini->ReadInt(str, _T("ROILEFT"), 0);
                        pRoiData->m_RoiArea.top = ini->ReadInt(str, _T("ROITOP"), 0);
                        pRoiData->m_RoiArea.right = ini->ReadInt(str, _T("ROIRIGHT"), 0);
                        pRoiData->m_RoiArea.bottom = ini->ReadInt(str, _T("ROIBOTTOM"), 0);

                        pRoiData->m_dCenterPos.x = ini->ReadInt(str, _T("POSFIVE-X"), 0);
                        pRoiData->m_dCenterPos.y = ini->ReadInt(str, _T("POSFIVE-Y"), 0);
                        //pRoiData->nIndex = indexCount;
                        if (pRoiData->m_dCenterPos.x < pRoiData->m_RoiArea.left + 5)
                                pRoiData->m_dCenterPos.x = pRoiData->m_RoiArea.left + 5;
                        else if (pRoiData->m_dCenterPos.x > pRoiData->m_RoiArea.right - 5)
                                pRoiData->m_dCenterPos.x = pRoiData->m_RoiArea.right - 5;

                        if (pRoiData->m_dCenterPos.y < pRoiData->m_RoiArea.top + 5)
                                pRoiData->m_dCenterPos.y = pRoiData->m_RoiArea.top + 5;
                        else if (pRoiData->m_dCenterPos.y > pRoiData->m_RoiArea.bottom - 5)
                                pRoiData->m_dCenterPos.y = pRoiData->m_RoiArea.bottom - 5;
                        break;
                case _ObjType_Point:
                        pRoiData->m_dCenterPos.x = ini->ReadInt(str, _T("POS_X"), 0);
                        pRoiData->m_dCenterPos.y = ini->ReadInt(str, _T("POS_Y"), 0);
                        break;
                case _ObjType_Line:
                        pRoiData->m_RoiArea.left = ini->ReadInt(str, _T("ROILEFT"), 0);
                        pRoiData->m_RoiArea.top = ini->ReadInt(str, _T("ROITOP"), 0);
                        pRoiData->m_RoiArea.right = ini->ReadInt(str, _T("ROIRIGHT"), 0);
                        pRoiData->m_RoiArea.bottom = ini->ReadInt(str, _T("ROIBOTTOM"), 0);

                        pRoiData->m_dCenterPos.x = ini->ReadInt(str, _T("POSFIVE-X"), 0);
                        pRoiData->m_dCenterPos.y = ini->ReadInt(str, _T("POSFIVE-Y"), 0);

                        if (pRoiData->m_dCenterPos.x < pRoiData->m_RoiArea.left + 5)
                                pRoiData->m_dCenterPos.x = pRoiData->m_RoiArea.left + 5;
                        else if (pRoiData->m_dCenterPos.x > pRoiData->m_RoiArea.right - 5)
                                pRoiData->m_dCenterPos.x = pRoiData->m_RoiArea.right - 5;

                        if (pRoiData->m_dCenterPos.y < pRoiData->m_RoiArea.top + 5)
                                pRoiData->m_dCenterPos.y = pRoiData->m_RoiArea.top + 5;
                        else if (pRoiData->m_dCenterPos.y > pRoiData->m_RoiArea.bottom - 5)
                                pRoiData->m_dCenterPos.y = pRoiData->m_RoiArea.bottom - 5;
                        break;
                        break;
                }

                //m_roiData.m_nInspectType = nInspType;
                pRoiData->m_vecParams.clear();

                extern ParamTable paramTable[];
                ParamTable* pParamTab = paramTable;
                for (int n = 0;; n++)
                {
                        if (pParamTab[n].nInspectType == _Inspect_Type_End)
                                break;
                        if (pParamTab[n].nInspectType == pRoiData->m_nInspectType)
                                pRoiData->m_vecParams.push_back(pParamTab[n].param);
                }


                TCHAR seps[] = _T(";");
                TCHAR *token;
                TCHAR szTok[512];
                int nParamSize = ini->ReadInt(str, _T("PARAM_NUMBER"), 0);
                for (int j = 0; j < nParamSize; j++) {
                        sub.Format(_T("PARAM%02d"), j);
                        strParam = ini->ReadString(str, sub, _T(""));
                        _tcscpy(szTok, (LPCTSTR)strParam.GetBuffer(0));

                        int seq = 0;
                        token = _tcstok(szTok, seps);
                        while (token != NULL)
                        {
                                if (seq == 0)	param.stepType = (enum StepType)_ttoi(token);
                                else if (seq == 1)	param.Name = token;
                                else if (seq == 2)	param.valueType = (enum ValueType)_ttoi(token);
                                else if (seq == 3)	{
                                        CString str(token);
                                        str.TrimRight(_T(" "));	// 파일에서 ;;를 없애기위해 ; ; 처럼 공백을 추가했는데 없애기위하
                                        param.Value = str;
                                }
                                else if (seq == 4)	param.Detail = token;
                                seq++;
                                token = _tcstok(NULL, seps);
                        }
                        //strParam.ReleaseBuffer();
                        pRoiData->ReplaceParam(param);
                }

                m_vecRoiData.push_back(pRoiData);
        }

        for (int nCh = 0; nCh<MAX_CH_NUM; nCh++) // Channel
        {
                m_vecWorkRoiData[nCh].clear();	// Channel마다 WorkLit를 관리.
                str.Format(_T("WORK_LIST_%02d"), nCh);
                int nSize = ini->ReadInt(str, _T("NUMBER"), 0);
                for (int j = 0; j < nSize; j++)
                {
                        sub.Format(_T("NAME_%02d"), j);
                        CString sName = ini->ReadString(str, sub, _T(""));
                        int size = m_vecRoiData.size();
                        for (int k = 0; k < size; k++) //ROI Pointer를 찾아 vector에 등록
                        {
                                CRoiData *pRoiData = m_vecRoiData[k];
                                if (nCh == pRoiData->m_nCh && pRoiData->m_sName == sName)
                                {
                                        m_vecWorkRoiData[nCh].push_back(pRoiData);
                                        //break;
                                }
                        }
                }
        }

        gCfg.WriteConfig();

        delete ini;
}

void CRecipeData::UpdateOneRecipeData(CRoiData *pRoiData, int nSeq)
{
        CMainFrame *pMainFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[theApp.m_nSelectCh];
		//pProcessingClass->m_pMainFrame;

		CRect roirect = pRoiData->m_RoiArea.GetRect();
		roirect.NormalizeRect();
		if (roirect.left < 0)	roirect.left = 0;
		if (roirect.top < 0)	roirect.top = 0;
		if (roirect.right >= pProcessingClass->iplImage->width) roirect.right = pProcessingClass->iplImage->width - 1;
		if (roirect.bottom >= pProcessingClass->iplImage->height) roirect.bottom = pProcessingClass->iplImage->height - 1;
		CRect ptnrect = pRoiData->ptnRoi.GetRect();
		ptnrect.NormalizeRect();
		if (ptnrect.left < 0)	ptnrect.left = 0;
		if (ptnrect.top < 0)	ptnrect.top = 0;
		if (ptnrect.right >= pProcessingClass->iplImage->width) ptnrect.right = pProcessingClass->iplImage->width - 1;
		if (ptnrect.bottom >= pProcessingClass->iplImage->height) ptnrect.bottom = pProcessingClass->iplImage->height - 1;

        CIniUtil* ini = new CIniUtil(m_sFileName);
        CString str, sub, strParam;
        CString sPatternPath;
        CString parentName;

        str.Format(_T("ROI%02d"), nSeq);
        ini->WriteString(str, _T("NAME"), pRoiData->m_sName);
        ini->WriteInt(str, _T("CH"), pRoiData->m_nCh);
        ini->WriteInt(str, _T("OBJECT_TYPE"), pRoiData->m_nObjectType); // 패턴, 영역, 포인터
        ini->WriteInt(str, _T("INSPECT_TYPE"), pRoiData->m_nInspectType); // InspectType 참조, _Inspect_Patt_VerticalAlign,.. 등

        sPatternPath.Format(_T("%s\\Recipe\\%s"), theApp.g_sRootPath, gCfg.m_sLastRecipeName);

        switch (pRoiData->m_nObjectType) {
        case _ObjType_Patt:
                ini->WriteInt(str, _T("ROILEFT"), roirect.left);
                ini->WriteInt(str, _T("ROITOP"), roirect.top);
                ini->WriteInt(str, _T("ROIRIGHT"), roirect.right);
                ini->WriteInt(str, _T("ROIBOTTOM"), roirect.bottom);
                ini->WriteInt(str, _T("PATLEFT"), ptnrect.left);
                ini->WriteInt(str, _T("PATTOP"), ptnrect.top);
                ini->WriteInt(str, _T("PATRIGHT"), ptnrect.right);
                ini->WriteInt(str, _T("PATBOTTOM"), ptnrect.bottom);
                ini->WriteInt(str, _T("PATCENTERX"), pRoiData->m_dCenterPos.x);
                ini->WriteInt(str, _T("PATCENTERY"), pRoiData->m_dCenterPos.y);
                sub.Format(_T("%s\\Pat%02d\\%s.bmp"), sPatternPath, pRoiData->m_nCh, pRoiData->m_sName);

                if (pProcessingClass->iplImage != NULL)
                {
                        if (pRoiData->iplTemplate){
                                cvReleaseImage(&pRoiData->iplTemplate);
                                pRoiData->iplTemplate = NULL;
                        }
                        pRoiData->iplTemplate = cvCreateImage(cvSize(ptnrect.Width(), ptnrect.Height()), IPL_DEPTH_8U, 1);
                        cvSetImageROI(pProcessingClass->iplImage, cvRect(ptnrect.left, ptnrect.top, ptnrect.Width(), ptnrect.Height()));
                        cvCopy(pProcessingClass->iplImage, pRoiData->iplTemplate);
                        cvResetImageROI(pProcessingClass->iplImage);
                }
                pRoiData->SaveImageData(sub);

                break;
        case _ObjType_Roi:
                ini->WriteInt(str, _T("ROILEFT"), pRoiData->m_RoiArea.left);
                ini->WriteInt(str, _T("ROITOP"), pRoiData->m_RoiArea.top);
                ini->WriteInt(str, _T("ROIRIGHT"), pRoiData->m_RoiArea.right);
                ini->WriteInt(str, _T("ROIBOTTOM"), pRoiData->m_RoiArea.bottom);

                ini->WriteInt(str, _T("POSFIVE-X"), pRoiData->m_dCenterPos.x);
                ini->WriteInt(str, _T("POSFIVE-Y"), pRoiData->m_dCenterPos.y);

                break;
        case _ObjType_Point:
                ini->WriteDouble(str, _T("POS_X"), pRoiData->m_dCenterPos.x);
                ini->WriteDouble(str, _T("POS_Y"), pRoiData->m_dCenterPos.y);
                break;
        case _ObjType_Line:
                ini->WriteInt(str, _T("ROILEFT"), pRoiData->m_RoiArea.left);
                ini->WriteInt(str, _T("ROITOP"), pRoiData->m_RoiArea.top);
                ini->WriteInt(str, _T("ROIRIGHT"), pRoiData->m_RoiArea.right);
                ini->WriteInt(str, _T("ROIBOTTOM"), pRoiData->m_RoiArea.bottom);

                ini->WriteInt(str, _T("POSFIVE-X"), pRoiData->m_dCenterPos.x);
                ini->WriteInt(str, _T("POSFIVE-Y"), pRoiData->m_dCenterPos.y);
                break;
        }


        int nParamSize = pRoiData->m_vecParams.size();
        ini->WriteInt(str, _T("PARAM_NUMBER"), nParamSize);
        for (int j = 0; j < nParamSize; j++) {
                sub.Format(_T("PARAM%02d"), j);
                CParam *pParam = &pRoiData->m_vecParams[j];
                strParam.Format(_T("%d;%s;%d;%s ;%s;"), pParam->stepType, pParam->Name.c_str(), pParam->valueType, pParam->Value.c_str(), pParam->Detail.c_str()); //Value 뒤에 공백을 한개 두자. _strtok()이 ;;로 붙어 있느니까 마지막으로 처라한다.
                ini->WriteString(str, sub, strParam);
        }

}

void CRecipeData::SaveRecipeData()
{
        CIniUtil* ini = new CIniUtil(m_sFileName);
        CString str, sub, strParam;

        CRecipeBase::SaveRecipeData();

        int size = m_vecRoiData.size();
        ini->WriteInt(_T("RECIPE_INFO"), _T("TOTAL_ROI_NUMBER"), size);
        for (int i = 0; i < size; i++) {
                CRoiData *pRoiData = m_vecRoiData[i];
                UpdateOneRecipeData(pRoiData, i);
        }

        for (int nCh = 0; nCh<MAX_CH_NUM; nCh++) // Channel
        {
                str.Format(_T("WORK_LIST_%02d"), nCh);
                int nSize = m_vecWorkRoiData[nCh].size();
                ini->WriteInt(str, _T("NUMBER"), nSize);
                for (int j = 0; j < nSize; j++)
                {
                        CRoiData *pData = m_vecWorkRoiData[nCh][j];
                        sub.Format(_T("NAME_%02d"), j);
                        ini->WriteString(str, sub, pData->m_sName);
                }
        }

        delete ini;
}



CString CRecipeData::getInspName(int nInspectType)
{
        return m_sInspList[nInspectType];
}


int CRecipeData::GetObjectInspIndex(CString sName){
        int nRet = -1;

        for (int i = 0; i<_Inspect_Point_End; i++){
                if (m_sInspList[i].GetLength() > 0 && sName == m_sInspList[i]){//m_sInspName.Find(m_sInspList[i], 0) != -1){
                        nRet = i;
                        break;
                }
        }
        return nRet;
}

int CRecipeData::getSeq(CRoiData* pRoiData)
{
        int size = m_vecRoiData.size();
        for (int k = 0; k < size; k++) //ROI Pointer를 찾아 vector에 등록
        {
                if (pRoiData == m_vecRoiData[k])
                        return k;
        }
        return -1;
}
