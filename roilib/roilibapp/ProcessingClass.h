#pragma once

#include "Data/Config.h"

struct T_RESERVED_PROCESS{
    CString sID;
    int		nCh;
    int		nStatus;

    T_RESERVED_PROCESS(){
        sID.Format(_T(""));
        nCh = 0;
        nStatus = 0;
    };
};

//
// 한개 카메라당 한개의 CProcessingClass가 운용된다.
// theApp.m_vecProcessingClass에 등록되어 운용.
//

class CMainFrame;
class CProcessingClass
{
public:
    CProcessingClass(int nCh);
    ~CProcessingClass(void);

    int m_nCh;

    IplImage* cimg;	// preview 용 color buffer
    IplImage *iplImage; // OPENCV를 이용하기위한 gray base이미지

    CMainFrame* m_pMainFrame;

    static UINT ThreadInspProcessing(LPVOID pParam);
    CWinThread *m_pThread;

    HANDLE m_hProcessEvent;
    T_RESERVED_PROCESS m_ProcessState;

    void Class_Init(CMainFrame* pFrameWnd, int nGrabFrame = 0, BOOL bDigMode = true, int nCamSeq = 0);
    void Class_Destory();

    BOOL m_bInspProcessRun;
    BOOL AreaCamImageGrab(int bGrab = FALSE);
    void GrabImageSave(CString sName);

};
