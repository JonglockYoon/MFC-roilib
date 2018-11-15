//
// Copyright (C) 2018
// All rights reserved by jerry1455@gmail.com
//

// RoilibApp.h : RoilibApp 응용 프로그램에 대한 주 헤더 파일.
//
#pragma once

#ifndef __AFXWIN_H__
    #error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "Data/Config.h"
#include <sys/timeb.h>
#include "ui/DrawTool.h"
#include "Utils/Direct_Info.h"
#include "ProcessingClass.h"
#include "Utils/Log.h"
#include "Utils/UI_Object.h"
#include "Engine/ImgProcEngine.h"
#include "Data/RecipeData.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

// CRoilibAppApp:
// 이 클래스의 구현에 대해서는 RoilibApp.cpp을 참조하십시오.
//

#define	WM_USER_NEWIMAGE WM_USER+1
#define WM_USER_PROGRESS WM_USER+2
#define WM_PROGRAM_EXIT_EVENT	WM_USER + 3000
#define WM_GRAB_UPDATE		WM_USER + 3507

#define WM_USER_ACTIVE_RIBBON_TAB_CHANGED	WM_USER+3621
#define WM_USER_ACTIVE_RIBBON_SIZE	WM_USER+3622

#define WM_USER_MOVEVIEWPOSITION WM_USER + 3631
#define WM_USER_LOAD_ROIDATA WM_USER + 3632

#define _S	_fnLoadString
CString _fnLoadString(UINT id, ...);

class CRoilibAppApp : public CSDIWinApp //CWinAppEx
{
public:
    CRoilibAppApp();

public:
    CCriticalSection	cs;
    cv::VideoCapture cap[16]; // 최대 16개의 Webcam Open 지원.

    CString	g_sRootPath;

    BOOL m_bStopProcess;
    BOOL m_bViewMeasureResult;
    BOOL m_bPreviewMode; // 평상시에는  Preview Mode로 동작하고 이미지 분석할때는 FALSE로 설정해서 다른 이미지 처리를 배제한다.
    BOOL m_bSmoothMode;

    CString m_sProcID;		//서버로 부터 검사 시작시 넘겨받을 검사아이뒤.
    BOOL						m_bCamTimeOut;

    //BOOL m_bAutoSelectCh; // TRUE이면 자동검사모드로 진입.
    // m_nSelectCh은 0 ~ m_nCh사이 값이다.
    // Ch이 달라지면 Base이미지가 달라진다. (즉, 다른 그랩이미지).
    int	m_nSelectCh;			// 검사시 그랩 횟수 관련 영역에 대한 설정이 달라짐으로 추가됨. (Ch은 그랩 이미지 구분).

	std::vector<RecipeListInformation>	m_vecRecipeInfo;
	std::vector<CProcessingClass*> m_vecProcessingClass; // 카메라당 한개의 Class가 생성 운용된다.
    CImgProcEngine m_ImgProcEngine;

// 재정의입니다.
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

// 구현입니다.
    UINT  m_nAppLook;
    BOOL  m_bHiColorIcons;

    int Opencv2ximage(IplImage *iplImage, CxImage *ximg);
    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    int AddLog(CString str);

    DECLARE_MESSAGE_MAP()
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CRoilibAppApp theApp;
