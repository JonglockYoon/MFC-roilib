#include "stdafx.h"
#include "RoilibApp.h"
#include "ImgProcEngine.h"
#include "MainFrm.h"

using namespace cv;

CImgProcEngine::CImgProcEngine()
{
	CString str;
	str.Format(_T("%s\\[%d]Engine"), theApp.g_sRootPath);

}

void CImgProcEngine::InitEngine(int nCh)
{
	if (g_cRecipeData == NULL)
		return;

}

CImgProcEngine::~CImgProcEngine()
{

}

int CImgProcEngine::InspectOneItem(int nCh, IplImage* img, CRoiData *pData)
{
	if (img == NULL)
		return -1;

	if (pData == NULL)
		return -1;

	CvSize searchSize = cvSize(img->width, img->height);
	IplImage* graySearchImg = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);
	if (img->nChannels == 3)
		cvCvtColor(img, graySearchImg, CV_RGB2GRAY);
	else
		cvCopy(img, graySearchImg);

	IplImage* croppedImage;
	CRect rect = pData->m_RoiArea.GetRect();	// Area로 등록된 ROI
	Point2f left_top = Point2f(rect.left, rect.top);
	cvSetImageROI(graySearchImg, cvRect((int)left_top.x, (int)left_top.y, rect.Width(), rect.Height()));
	croppedImage = cvCreateImage(cvSize(rect.Width(), rect.Height()), graySearchImg->depth, graySearchImg->nChannels);
	cvCopy(graySearchImg, croppedImage);
	cvResetImageROI(graySearchImg);

	switch (pData->m_nInspectType)
	{
	case _Inspect_Roi_Circle:
		SingleROICircle(nCh, croppedImage, pData, rect);
		break;
	}
	cvReleaseImage(&croppedImage);
	cvReleaseImage(&graySearchImg);

	return 0;
}

int CImgProcEngine::SingleROICircle(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect)
{
	int n = FindCircle(pData, croppedImage);
	if (n > 0) {
		pData->m_vecDetectResult[0].pt.x += (float)pData->m_RoiArea.left;
		pData->m_vecDetectResult[0].pt.y += (float)pData->m_RoiArea.top;
		pData->m_vecDetectResult[0].pt1.x += (float)pData->m_RoiArea.left;
		pData->m_vecDetectResult[0].pt1.y += (float)pData->m_RoiArea.top;
	}

	return n;
}

void CImgProcEngine::SaveOutImage(IplImage* pImgOut, int nCh, CString strMsg, BOOL bClear/*=FALSE*/)
{
	CString str = _T("");
	str.Format(_T("%s\\[%d]%s"), m_sDebugPath, nCh, strMsg);
	CT2A ascii(str); cvSaveImage(ascii, pImgOut);
	if (bClear) cvZero(pImgOut);
}

//
// HoughCircle을 이용한 Circle을 찾는다
//
int CImgProcEngine::FindCircle(CRoiData *pData, IplImage* graySearchImgIn)
{
	int ret = 0;
	CString str;
	CvMemStorage* storage = NULL;
	storage = cvCreateMemStorage(0);

	IplImage* graySearchImg = cvCloneImage(graySearchImgIn);

	NoiseOut(pData, graySearchImg, 202);
	Smooth(pData, graySearchImg, 204);

	int nMinCircleRadius = 30;
	int nMaxCircleRadius = 100;
	int nMaximumThresholdCanny = 100;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Minimum circle radius"));
		if (pParam)
			nMinCircleRadius = (int)_ttof(pParam->Value.c_str());
		pParam = pData->getParam(_T("Maximum circle radius"));
		if (pParam)
			nMaxCircleRadius = (int)_ttof(pParam->Value.c_str());
		pParam = pData->getParam(_T("Maximum Threshold Canny"));
		if (pParam)
			nMaximumThresholdCanny = (int)_ttof(pParam->Value.c_str());

	}

	double dp = 3; // Accumulator resolution
	//double min_dist = 100; // Minimum distance between the centers of the detected circles.
	double min_dist = graySearchImg->height / 3;
	CvSeq* Circles = cvHoughCircles(graySearchImg, storage, CV_HOUGH_GRADIENT,
		dp,  // // 누적기 해상도(영상크기/2) //예를 들어 2로 지정하면 영상의 절반 크기인 누산기를 만듬.
		min_dist, // 두 원 간의 최소 거리
		nMaximumThresholdCanny, // 200 // 캐니 최대 경계값, 200정도면 확실한 원을 찾고, 100정도이면 근사치의 원을 모두 찾아준다.
		100, // 25 // 투표 최소 개수 //낮은 경계값은  높은 경계값  절반으로 설정.
		nMinCircleRadius,   // Minimum circle radius
		nMaxCircleRadius); // Maximum circle radius

	float* circle;
	if (Circles->total >= 1)
	{
		int cx, cy, radius;
		circle = (float*)cvGetSeqElem(Circles, 0);

		m_DetectResult.pt.x = circle[0] - pData->GetRect().Width()/2;
		m_DetectResult.pt.y = circle[1] - pData->GetRect().Height()/2;
		m_DetectResult.pt1.x = circle[0] + pData->GetRect().Width()/2;
		m_DetectResult.pt1.y = circle[1] + pData->GetRect().Height()/2;
		m_DetectResult.dRadius = circle[2];
		m_DetectResult.dAngle = 0;
		m_DetectResult.nCh = pData->m_nCh;
		pData->m_vecDetectResult.push_back(m_DetectResult);
		ret = 1;
	}

	if (gCfg.m_bSaveEngineImg){
		for (int k = 0; k<Circles->total; k++)
		{
			//m_DetectResult.dRadius -= 10; //test
			circle = (float*)cvGetSeqElem(Circles, k);
			cvCircle(graySearchImg, cvPoint(cvRound(circle[0]), cvRound(circle[1])), cvRound(m_DetectResult.dRadius), CV_RGB(128, 128, 128), 3);
			break;
		}
		str.Format(_T("%s\\[%d]210_Circles.BMP"), theApp.m_ImgProcEngine.m_sDebugPath, pData->m_nCh);
		CT2A ascii(str); cvSaveImage(ascii, graySearchImg);
	}

	cvReleaseImage(&graySearchImg);
	cvReleaseMemStorage(&storage);

	return ret;
}

//
// 사용자가 설정한 값으로 Threshold를 실행
//
int CImgProcEngine::Threshold(CRoiData *pData, IplImage* grayImg, int nDbg)
{
	CString str;

	int nThresholdValue = 70;
	int nThresholdMaxVal = 255;
	int bInvert = 0;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Brightness Threshold"));
		if (pParam)
			nThresholdValue = _ttoi(pParam->Value.c_str());
		pParam = pData->getParam(_T("Brightness Max"));
		if (pParam)
			nThresholdMaxVal = _ttoi(pParam->Value.c_str());
		pParam = pData->getParam(_T("Invert?"));
		if (pParam)
			bInvert = (int)_ttof(pParam->Value.c_str());
	}
	if (nThresholdValue == 0 && nThresholdMaxVal == 0)
		return -1;

	int nInvert = CV_THRESH_BINARY;
	if (bInvert == 1)
		nInvert = CV_THRESH_BINARY_INV;
	cvThreshold(grayImg, grayImg, nThresholdValue, nThresholdMaxVal, nInvert);
	if (gCfg.m_bSaveEngineImg){
		str.Format(_T("%s\\[%d]%03d_cvThreshold.BMP"), m_sDebugPath, pData->m_nCh, nDbg);
		CT2A ascii(str); cvSaveImage(ascii, grayImg);
	}

	return 0;
}

//
// OTSU 알고리즘을 이용하여 히스토그램 Threshold를 실행
//
double CImgProcEngine::ThresholdOTSU(CRoiData *pData, IplImage* grayImg, int nDbg)
{
	CString str;

	int nThresholdMaxVal = 255;
	if (pData != NULL) {
		CParam *pParam;
	}

	int nInvert = CV_THRESH_BINARY | CV_THRESH_OTSU;
	double otsuThreshold = cvThreshold(grayImg, grayImg, 0, nThresholdMaxVal, nInvert);
	if (gCfg.m_bSaveEngineImg){
		str.Format(_T("%s\\[%d]%03d_cvThreshold.BMP"), m_sDebugPath, pData->m_nCh, nDbg);
		CT2A ascii(str); cvSaveImage(ascii, grayImg);
	}

	return otsuThreshold;
}

//
// 사용자가 설정한 값으로 Threshold를 실행
//
int CImgProcEngine::ThresholdRange(CRoiData *pData, IplImage* grayImg, int nDbg)
{
	CString str;

	int nThresholdLowValue = 70;
	int nThresholdHighValue = 255;
	int bInvert = 0;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Low Threshold"));
		if (pParam)
			nThresholdLowValue = _ttoi(pParam->Value.c_str());
		pParam = pData->getParam(_T("High Threshold"));
		if (pParam)
			nThresholdHighValue = _ttoi(pParam->Value.c_str());
		pParam = pData->getParam(_T("Invert?"));
		if (pParam)
			bInvert = (int)_ttof(pParam->Value.c_str());
	}
	if (nThresholdHighValue == 0 && nThresholdLowValue == 0)
		return -1;

	cvInRangeS(grayImg, Scalar(nThresholdLowValue), Scalar(nThresholdHighValue), grayImg);

	if (bInvert == 1)
		cvNot(grayImg, grayImg);

	if (gCfg.m_bSaveEngineImg){
		str.Format(_T("%s\\[%d]%03d_cvThresholdRange.BMP"), m_sDebugPath, pData->m_nCh, nDbg);
		CT2A ascii(str); cvSaveImage(ascii, grayImg);
	}

	return 0;
}

//
// 모폴리지를 이용하여 잡음제거
//
int CImgProcEngine::NoiseOut(CRoiData *pData, IplImage* grayImg, int nDbg)
{
	CString str;

	IplImage* tmp = cvCreateImage(cvGetSize(grayImg), 8, 1);

	// 1. Template이미지의 노이즈 제거
	int filterSize = 6;  // 필터의 크기를 6으로 설정 (Noise out area)
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Noise out area"));
		if (pParam)
			filterSize = _ttoi(pParam->Value.c_str());
	}
	IplConvKernel *element = NULL;
	if (filterSize <= 0)
		filterSize = 1;
	if (filterSize % 2 == 0)
		filterSize++;
	element = cvCreateStructuringElementEx(filterSize, filterSize, filterSize / 2, filterSize / 2, CV_SHAPE_RECT, NULL);
	int nNoiseout = 1;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Noise out 1"));
		if (pParam)
			nNoiseout = _ttoi(pParam->Value.c_str());
	}
	if (nNoiseout < 0)
		cvMorphologyEx(grayImg, tmp, NULL, element, CV_MOP_OPEN, -nNoiseout);
	else //if (nNoiseout > 0)
		cvMorphologyEx(grayImg, tmp, NULL, element, CV_MOP_CLOSE, nNoiseout);

	nNoiseout = 1;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Noise out 2"));
		if (pParam)
			nNoiseout = _ttoi(pParam->Value.c_str());
	}
	if (nNoiseout < 0)
		cvMorphologyEx(tmp, grayImg, NULL, element, CV_MOP_OPEN, -nNoiseout);
	else //if (nNoiseout > 0)
		cvMorphologyEx(tmp, grayImg, NULL, element, CV_MOP_CLOSE, nNoiseout);

	if (gCfg.m_bSaveEngineImg){
		str.Format(_T("%s\\[%d]%03d_cvClose.BMP"), m_sDebugPath, pData->m_nCh, nDbg);
		CT2A ascii(str); cvSaveImage(ascii, grayImg);
	}
	cvReleaseImage(&tmp);

	cvReleaseStructuringElement(&element);
	return 0;
}

//
// Dialate / Erode
//
int CImgProcEngine::Expansion(CRoiData *pData, IplImage* grayImg, int nDbg)
{
	CString str;

	IplImage* tmp = cvCreateImage(cvGetSize(grayImg), 8, 1);

	int nExpansion = 1;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Expansion 1"));
		if (pParam)
			nExpansion = _ttoi(pParam->Value.c_str());
	}
	if (nExpansion < 0)
		cvErode(grayImg, tmp, NULL, -nExpansion);
	else  //if (nExpansion > 0)
		cvDilate(grayImg, tmp, NULL, nExpansion);

	nExpansion = 1;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Expansion 2"));
		if (pParam)
			nExpansion = _ttoi(pParam->Value.c_str());
	}
	if (nExpansion < 0)
		cvErode(tmp, grayImg, NULL, -nExpansion);
	else //if (nExpansion > 0)
		cvDilate(tmp, grayImg, NULL, nExpansion);

	if (gCfg.m_bSaveEngineImg){
		str.Format(_T("%s\\[%d]%03d_cvExpansion.BMP"), m_sDebugPath, pData->m_nCh, nDbg);
		CT2A ascii(str); cvSaveImage(ascii, grayImg);
	}
	cvReleaseImage(&tmp);
	return 0;
}


void CImgProcEngine::Smooth(CRoiData *pData, IplImage* ImgIn, int iImgID)
{
	int param1 = 3;
	int param2 = 0;
	double param3 = 0, param4 = 0;
	int smoothMethod = CV_GAUSSIAN;
	int smoothSize = 3;
	IplImage* image = NULL;
	BOOL bUse = TRUE;

	if (pData != NULL)
	{
		CParam *pParam = pData->getParam(_T("Smooth method"));
		if (pParam) smoothMethod = _ttoi(pParam->Value.c_str());
		pParam = pData->getParam(_T("Smooth size"));
		if (pParam) smoothSize = _ttoi(pParam->Value.c_str());
		// 5x5 행렬
		if (smoothSize == 1) smoothSize = 5;
		// 7x7 행렬
		else if (smoothSize == 2) smoothSize = 7;
		// 9x9 행렬
		else if (smoothSize == 3) smoothSize = 9;
		// 11x11 행렬
		else if (smoothSize == 4) smoothSize = 11;
		// 13x13 행렬
		else if (smoothSize == 5) smoothSize = 13;
		// 15x15 행렬
		else if (smoothSize == 6) smoothSize = 15;
		// 3x3 행렬
		else smoothSize = 3;

		pParam = pData->getParam(_T("Smooth Use"));
		if (pParam) bUse = (BOOL)_ttoi(pParam->Value.c_str());
	}

	if (!bUse) return;

	// This is done so as to prevent a lot of false circles from being detected
	// 
	// 1. const CvArr* src
	// 2. CvArr* dst
	// 3. int smooththype=CV_GAUSSIAN
	// 4. int param1=3
	// 5. int param2=0
	// 6. double param3=0
	// 7. double param4=0
	switch (smoothMethod)
	{
	case CV_BLUR_NO_SCALE:
		param1 = smoothSize;
		break;
	case CV_BLUR:
		param1 = smoothSize;
		break;
	case CV_MEDIAN:
		param1 = smoothSize;
		param2 = smoothSize;
		break;
	case CV_BILATERAL:
		param1 = smoothSize;
		param2 = smoothSize;

		if (smoothSize == 11)
		{
			param3 = 5;
			param4 = 5;
		}
		else if (smoothSize == 5)
		{
			param3 = 3;
			param4 = 3;
		}
		else
		{
			param3 = 2;
			param4 = 2;
		}

		break;
	case CV_GAUSSIAN:
	default:
		param1 = smoothSize;
		param2 = smoothSize;
		break;
	}

	cvSmooth(ImgIn, ImgIn, smoothMethod, param1, param2, param3, param4);

	if (gCfg.m_bSaveEngineImg)
	{
		CString str;
		str.Format(_T("%d_Smooth.BMP"), iImgID);
		SaveOutImage(ImgIn, pData->m_nCh, str);
	}
}
