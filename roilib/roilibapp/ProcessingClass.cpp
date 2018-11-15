#include "StdAfx.h"
#include "RoilibApp.h"
#include "ProcessingClass.h"
#include "data/Config.h"
#include "Utils/Log.h"
#include "IniUtil.h"
#include "MainFrm.h"
#include "RoilibAppDoc.h"


CProcessingClass::CProcessingClass(int nCh)
{
    m_nCh = nCh;

    cimg = NULL;
    iplImage = NULL;

    m_pThread = NULL;
    m_bInspProcessRun = FALSE;

    m_pMainFrame = NULL;
    m_hProcessEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual Reset - ResetEvent를 해주어야함.
}

CProcessingClass::~CProcessingClass(void)
{
    if (m_hProcessEvent != NULL)
        CloseHandle(m_hProcessEvent);
}

//
// Program이 종료될때까지 계속 실행되는 Thread이다.
//
UINT CProcessingClass::ThreadInspProcessing(LPVOID pParam)
{
    CString str;
    CProcessingClass *pProcessingClass = (CProcessingClass *)pParam;

    while (pProcessingClass->m_bInspProcessRun)
    {
        DWORD state = WaitForSingleObject(pProcessingClass->m_hProcessEvent, 50); // 50ms대기.
        //if(state == WAIT_TIMEOUT)            // 이벤트가 발생하지 않은 경우.
        //	continue;

        ::ResetEvent(pProcessingClass->m_hProcessEvent);
        if (pProcessingClass->m_bInspProcessRun)
            pProcessingClass->AreaCamImageGrab();
    }
    return 0;
}

void CProcessingClass::Class_Init(CMainFrame* pFrameWnd, int nGrabFrame, BOOL bDigMode, int nCamSeq)
{

    m_pMainFrame = (CMainFrame*)pFrameWnd;

    m_bInspProcessRun = TRUE;
    m_pThread = AfxBeginThread(ThreadInspProcessing, (PVOID) this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    m_pThread->m_bAutoDelete = FALSE;
    m_pThread->ResumeThread();
}

void CProcessingClass::Class_Destory()
{
    m_bInspProcessRun = FALSE; // ThreadInspProcessing Thread를 종료한다.
    WaitForSingleObject(m_pThread, INFINITE);
    if (m_pThread)
        delete m_pThread;

    if (iplImage) 
	{
        cvReleaseImage(&iplImage);
        iplImage = NULL;
    }
}

void CProcessingClass::GrabImageSave(CString sName)
{
    if (gCfg.m_bSaveGrabImg == FALSE)
        return;

    CTime tCurTime = CTime::GetCurrentTime();
    CString sPath;

    sPath.Format(_T("%s\\Grab\\%04d\\%02d\\%02d\\%s.BMP"), theApp.g_sRootPath, tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), sName);
    checkPath(sPath, true);

    if (iplImage == NULL)
        return;
    CT2A ascii(sPath); cvSaveImage(ascii, cimg);
    g_cLog->AddLog(sPath, _LOG_LIST_INSP);
}

// bGrab==TRUE이면 iplImage buffer로.
// m_bPreviewMode==TRUE이면 previewImage buffer로.
BOOL CProcessingClass::AreaCamImageGrab(int bGrab)
{
    BOOL bRet = FALSE;
    if (m_nCh < 0)
        return FALSE;

    int nCam = gCfg.m_Webcam[m_nCh];
    if (nCam > 15)
        return FALSE;
    if (bGrab == TRUE || theApp.m_bPreviewMode == TRUE) // 프리뷰 Mode일때 카메라 Grab을 계속한다.
    {
        if (!theApp.cap[nCam].isOpened())
            return FALSE;

        cv::Mat mat;

        theApp.cs.Lock();

        theApp.cap[nCam] >> mat;		// 동시 그랩이 안되에 cs 이용.
        theApp.cap[nCam] >> mat;		// 두번을 호출해야 최신 영상이 얻어진다..
        IplImage *s = &IplImage(mat);
        if (s != NULL)
        {
			if (gCfg.m_bCamFlipHoriz && gCfg.m_bCamFlipVert)
				cvFlip(s, NULL, -1);
			else if (gCfg.m_bCamFlipHoriz)
				cvFlip(s, NULL, 1);
			else if (gCfg.m_bCamFlipVert)
				cvFlip(s, NULL, 0);

            int sx = s->width;
            int sy = s->height;

            if (cimg == NULL)	// cimg는 color buffer
                cimg = cvCreateImage(cvSize(sx, sy), IPL_DEPTH_8U, 3);
            else 
			{
                int tx = cimg->width;
                int ty = cimg->height;
                if (sx != tx || sy != ty) 
				{
                    cvReleaseImage(&cimg);
                    cimg = cvCreateImage(cvSize(sx, sy), IPL_DEPTH_8U, 3);
                }
            }
            if (s->nChannels == 1)
                cvCvtColor(s, cimg, CV_GRAY2RGB);
            else
                cvCopy(s, cimg);
            m_pMainFrame->PostMessage(WM_GRAB_UPDATE, m_nCh, (LPARAM)0);

            if (bGrab && s)
            {
                int sx = cimg->width;
                int sy = cimg->height;
                if (iplImage == NULL)
                    iplImage = cvCreateImage(cvSize(s->width, s->height), IPL_DEPTH_8U, 1);
                else 
				{
                    if (sx != iplImage->width || sy != iplImage->height) 
					{
                        cvReleaseImage(&iplImage);
                        iplImage = cvCreateImage(cvSize(sx, sy), IPL_DEPTH_8U, 1);
                    }
                }
                if (s->nChannels == 3)
                    cvCvtColor(s, iplImage, CV_RGB2GRAY);
                else
                    cvCopy(s, iplImage);
            }
            bRet = TRUE;
        }
        theApp.cs.Unlock();
        bRet = TRUE;
    }

    return bRet;
}