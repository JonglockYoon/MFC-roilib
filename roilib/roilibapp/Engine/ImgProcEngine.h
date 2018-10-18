#pragma once

#include "./Data/Config.h"
#include "../Common/Utils/Log.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "blob.h"
#include "BlobResult.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "data/RecipeData.h"
#include "edgessubpix.h"
#include "imgprocbase.h"

using namespace cv;

#define CVX_RED		CV_RGB(255,0,0)
#define CVX_ORANGE	CV_RGB(255,165,0)
#define CVX_YELLOW	CV_RGB(255,255,0)
#define CVX_GREEN	CV_RGB(0,255,0)
#define CVX_BLUE	CV_RGB(0,0,255)
#define CVX_PINK	CV_RGB(255,0,255)
#define CVX_BLICK	CV_RGB(0,0,0)
#define CVX_WHITE	CV_RGB(255,255,255)

class CImgProcEngine : public CImgProcBase
{
public:
	CWnd* m_pMainWnd;

	CString m_sDebugPath;

	DetectResult m_DetectResult;

protected:
	int Threshold(CRoiData *pData, IplImage* grayImg, int nDbg = 100);
	double ThresholdOTSU(CRoiData *pData, IplImage* grayImg, int nDbg = 100);
	int ThresholdRange(CRoiData *pData, IplImage* grayImg, int nDbg = 100);
	int NoiseOut(CRoiData *pData, IplImage* grayImg, int nDbg = 100);
	int Expansion(CRoiData *pData, IplImage* grayImg, int nDbg = 150);
	void Smooth(CRoiData *pData, IplImage* ImgIn, int iImgID);

public:
	CImgProcEngine();
	~CImgProcEngine();

	int FindCircle(CRoiData *pData, IplImage* graySearchImgIn);
	void SaveOutImage(IplImage* pImgOut, int nCh, CString strMsg, BOOL bClear = FALSE);

	void InitEngine(int nCh);
	int InspectOneItem(int nCh, IplImage* img, CRoiData *pData);
	int SingleROICircle(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);
	CString SingleROIBarCode(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);
};

