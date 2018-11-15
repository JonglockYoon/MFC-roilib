#include "stdafx.h"
#include "RoilibApp.h"
#include "Data\RecipeData.h"
#include "ImgProcEngine.h"
#include "MainFrm.h"
#include "Utils/String.h"
#include <string>

#if _MSC_VER>1800 // VS2013 보다크면.
#include <tesseract/baseapi.h>
#endif
#include <leptonica/allheaders.h>

using namespace cv;
using namespace std;

CImgProcEngine::CImgProcEngine()
{
	CString str;
	str.Format(_T("%s\\[%d]Engine"), theApp.g_sRootPath);

	setlocale(LC_ALL, "C");
	//setlocale(LC_ALL, NULL);
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
	CRect rect1 = CRect(0, 0, graySearchImg->width, graySearchImg->height);
	rect.NormalizeRect();

	if (rect.left < 0)	rect.left = 0;
	if (rect.top < 0)	rect.top = 0;
	if (rect.right >= graySearchImg->width) rect.right = graySearchImg->width - 1;
	if (rect.bottom >= graySearchImg->height) rect.bottom = graySearchImg->height - 1;

	CRect rcInter;
	rcInter.IntersectRect(rect1, rect);
	if (rcInter.IsRectEmpty())
		return -1;

	cv::Point2f left_top = cv::Point2f(rcInter.left, rcInter.top);
	cvSetImageROI(graySearchImg, cvRect((int)left_top.x, (int)left_top.y, rcInter.Width(), rcInter.Height()));
	croppedImage = cvCreateImage(cvSize(rcInter.Width(), rcInter.Height()), graySearchImg->depth, graySearchImg->nChannels);
	cvCopy(graySearchImg, croppedImage);
	cvResetImageROI(graySearchImg);

	CString str;
	CvFont font;
	switch (pData->m_nInspectType)
	{
	case _Inspect_Patt_Identify:
		SinglePattIdentify(nCh, croppedImage, pData, rect);
		break;
	case _Inspect_Patt_MatchShapes:
		SinglePattMatchShapes(nCh, croppedImage, pData, rect);
		break;
	case _Inspect_Patt_FeatureMatch:
		SinglePattFeatureMatch(nCh, croppedImage, pData, rect);
		break;

	case _Inspect_Roi_Circle:
		SingleROICircle(nCh, croppedImage, pData, rcInter);
		break;
	case _Inspect_BarCode:
		SingleROIBarCode(nCh, croppedImage, pData, rcInter);
		break;
	case _Inspect_Teseract:
		SingleROIOCR(nCh, croppedImage, pData, rcInter);
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
		pData->m_vecDetectResult[0].resultType = RESULTTYPE_POINT;
		pData->m_vecDetectResult[0].pt.x += (float)pData->m_RoiArea.left;
		pData->m_vecDetectResult[0].pt.y += (float)pData->m_RoiArea.top;

	}

	return n;
}

int CImgProcEngine::SingleROIBarCode(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect)
{
#if _MSC_VER>1800 // VS2013 보다크면.

	CString str;
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;

	IplImage *pimg = croppedImage;
	auto binImg = std::make_shared<ZXing::GenericLuminanceSource>((int)pimg->width, (int)pimg->height,
		(unsigned char*)pimg->imageData, pimg->widthStep, 1, 2, 1, 0); // BW Image
	auto bitmap = new ZXing::HybridBinarizer(binImg);
	auto result = pMainFrame->_bcreader->read(*bitmap);
	delete bitmap;
	if (result.isValid()) {
		str = CString(result.text().c_str());
		TRACE(_T("Barcode : %d\n"), str);
		CT2CA pszConvertedAnsiString(str);
		m_DetectResult.resultType = RESULTTYPE_STRING;
		m_DetectResult.str = pszConvertedAnsiString;
		pData->m_vecDetectResult.push_back(m_DetectResult);
		return 0;
	}

#endif

	return -1;
}

int CImgProcEngine::SingleROIOCR(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect)
{
#if _MSC_VER>1800 // VS2013 보다크면.

	CString str;
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;

	if (gCfg.m_bSaveEngineImg) {
		str.Format(_T("%s\\[%d]%03d_src.BMP"), m_sDebugPath, pData->m_nCh, 100);
		CT2A ascii(str); cvSaveImage(ascii, croppedImage);
	}


	static tesseract::TessBaseAPI ocr;
	int init_failed;

	// Initialize tesseract to use English (eng) and the OEM_LSTM_ONLY engine. 
	//init_failed = ocr.Init(".\\tessdata", "eng", tesseract::OEM_DEFAULT);
	init_failed = ocr.Init(".\\tessdata", "eng", tesseract::OEM_LSTM_ONLY);

	// 추천 글자들.
	ocr.SetVariable("tessedit_char_whitelist", "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	// 비추천 글자들.
	ocr.SetVariable("tessedit_char_blacklist", "!@#$%^&*()_+=-[]}{;:'\"\\|~`,./<>?");

	if (init_failed) {
		g_cLog->AddLog(_T("Could not initialize tesseract."), _LOG_LIST_SYS);
		return -1;
	}
	//g_cLog->AddLog(_T("initialize tesseract success."), _LOG_LIST_SYS);

	// Set Page segmentation mode to PSM_AUTO (3)
	ocr.SetPageSegMode(tesseract::PSM_AUTO);

	ThresholdRange(pData, croppedImage, 200);
	
	int iMinY = 0, iMaxY = 100000;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Min Size Y"));
		if (pParam)
			iMinY = (int)(_ttof(pParam->Value.c_str()));
		pParam = pData->getParam(_T("Max Size Y"));
		if (pParam)
			iMaxY = (int)(_ttof(pParam->Value.c_str()));
	}
	FilterBlobBoundingBoxYLength(croppedImage, iMinY, iMaxY);


	NoiseOut(pData, croppedImage, _ProcessValue1, 202);
	Expansion(pData, croppedImage, _ProcessValue1, 204);

	NoiseOut(pData, croppedImage, _ProcessValue2, 206);
	Expansion(pData, croppedImage, _ProcessValue2, 208);

	if (gCfg.m_bSaveEngineImg) {
		str.Format(_T("%s\\[%d]%03d_cvClose.BMP"), m_sDebugPath, pData->m_nCh, 205);
		CT2A ascii(str); cvSaveImage(ascii, croppedImage);
	}

	double dSizeX = 1.0, dSizeY = 1.0;
	if (pData != NULL) {
		CParam *pParam = pData->getParam(_T("Size X(%)"));
		if (pParam)
			dSizeX = _ttof(pParam->Value.c_str()) / 100.0;
		pParam = pData->getParam(_T("Size Y(%)"));
		if (pParam)
			dSizeY = _ttof(pParam->Value.c_str()) / 100.0;
	}

	CvSize sz = CvSize(croppedImage->width * dSizeX, croppedImage->height * dSizeY);
	IplImage* tmp = cvCreateImage(sz, 8, 1);
	cvResize(croppedImage, tmp, CV_INTER_CUBIC);

	Smooth(pData, tmp, 206);
	
	if (gCfg.m_bSaveEngineImg) {
		str.Format(_T("%s\\[%d]%03d_cvTmp.BMP"), m_sDebugPath, pData->m_nCh, 300);
		CT2A ascii(str); cvSaveImage(ascii, tmp);
	}

	// Open input image using OpenCV
	cv::Mat im = cv::cvarrToMat(tmp);
	
	// Set image data
	ocr.SetImage(im.data, im.cols, im.rows, 1, im.step); // BW color
	
	// Run Tesseract OCR on image
	ocr.Recognize(nullptr);

	char* rst = ocr.GetUTF8Text();
	//string outText = string();
	
	ocr.End();
	
	// print recognized text
	//cout << outText << endl; // Destroy used object and release memory ocr->End();
	//CString str;
	str.Format(_T("OCR Text:%s"), CString(rst));
	g_cLog->AddLog(str, _LOG_LIST_SYS);

	m_DetectResult.resultType = RESULTTYPE_STRING;
	m_DetectResult.str = rst;
	pData->m_vecDetectResult.push_back(m_DetectResult);

	cvReleaseImage(&tmp);

#endif	
	return -1;
}









void CImgProcEngine::SaveOutImage(IplImage* pImgOut, int nCh, CString strMsg, BOOL bClear/*=FALSE*/)
{
	CString str = _T("");
	str.Format(_T("%s\\[%d]%s"), m_sDebugPath, nCh, strMsg);
	CT2A ascii(str); 
	cvSaveImage(ascii, pImgOut);
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

	NoiseOut(pData, graySearchImg, _ProcessValue1, 202);
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

		m_DetectResult.resultType = RESULTTYPE_POINT;
		m_DetectResult.pt.x = circle[0] - pData->GetRect().Width()/2;
		m_DetectResult.pt.y = circle[1] - pData->GetRect().Height()/2;
		m_DetectResult.dRadius = circle[2];
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
		str.Format(_T("210_Circles.BMP"));
		SaveOutImage(graySearchImg, pData->m_nCh, str, FALSE);
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
		str.Format(_T("%03d_cvThreshold.BMP"), nDbg);
		SaveOutImage(grayImg, pData->m_nCh, str, FALSE);
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
		str.Format(_T("%03d_cvThreshold.BMP"), nDbg);
		SaveOutImage(grayImg, pData->m_nCh, str, FALSE);
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

	cvInRangeS(grayImg, cv::Scalar(nThresholdLowValue), cv::Scalar(nThresholdHighValue), grayImg);

	if (bInvert == 1)
		cvNot(grayImg, grayImg);

	if (gCfg.m_bSaveEngineImg){
		str.Format(_T("%03d_cvThresholdRange.BMP"), nDbg);
		SaveOutImage(grayImg, pData->m_nCh, str, FALSE);
	}

	return 0;
}

//
// 모폴리지를 이용하여 잡음제거
//
int CImgProcEngine::NoiseOut(CRoiData *pData, IplImage* grayImg, int t, int nDbg, int h)
{
	CString str;

	if (t < 0)
		t = _ProcessValue1;
	//IplImage* tmp = cvCreateImage(cvGetSize(grayImg), 8, 1);

	// 1. Template이미지의 노이즈 제거
	int filterSize = 3;  // 필터의 크기를 6으로 설정 (Noise out area)
						 //    if (pData != nullptr) {
						 //        CParam *pParam = pData->getParam(("Noise out area"));
						 //        if (pParam)
						 //            filterSize = pParam->Value.toDouble();
						 //    }
	IplConvKernel *element = nullptr;
	if (filterSize <= 0)
		filterSize = 1;
	if (filterSize % 2 == 0)
		filterSize++;
	//element = cvCreateStructuringElementEx(filterSize, filterSize, (filterSize - 1) / 2, (filterSize - 1) / 2, CV_SHAPE_RECT, nullptr);
	element = cvCreateStructuringElementEx(filterSize, filterSize, filterSize / 2, filterSize / 2, CV_SHAPE_RECT, nullptr);

	int nNoiseout = 0;
	if (pData != nullptr) {
		CParam *pParam = pData->getParam(_T("Noise out 1"), t);
		if (pParam)
			nNoiseout = _ttoi(pParam->Value.c_str());
	}
	if (nNoiseout != 0)
	{
		if (nNoiseout < 0)
			cvMorphologyEx(grayImg, grayImg, nullptr, element, CV_MOP_OPEN, -nNoiseout);
		else //if (nNoiseout > 0)
			cvMorphologyEx(grayImg, grayImg, nullptr, element, CV_MOP_CLOSE, nNoiseout);
	}

	nNoiseout = 0;
	if (pData != nullptr) {
		CParam *pParam = pData->getParam(_T("Noise out 2"), t);
		if (pParam)
			nNoiseout = _ttoi(pParam->Value.c_str());
	}
	if (nNoiseout != 0)
	{
		if (nNoiseout < 0)
			cvMorphologyEx(grayImg, grayImg, nullptr, element, CV_MOP_OPEN, -nNoiseout);
		else //if (nNoiseout > 0)
			cvMorphologyEx(grayImg, grayImg, nullptr, element, CV_MOP_CLOSE, nNoiseout);
	}

	nNoiseout = 0;
	if (pData != nullptr) {
		CParam *pParam = pData->getParam(_T("Noise out 3"), t);
		if (pParam)
			nNoiseout = (int)_ttof(pParam->Value.c_str());
	}
	if (nNoiseout != 0)
	{
		if (nNoiseout < 0)
			cvMorphologyEx(grayImg, grayImg, nullptr, element, CV_MOP_OPEN, -nNoiseout);
		else //if (nNoiseout > 0)
			cvMorphologyEx(grayImg, grayImg, nullptr, element, CV_MOP_CLOSE, nNoiseout);
	}

	if (gCfg.m_bSaveEngineImg) {
		if (h >= 0)
			str.Format(_T("%d_%03d_cvClose.jpg"), h, nDbg);
		else str.Format(_T("%03d_cvClose.jpg"), nDbg);
		SaveOutImage(grayImg, pData->m_nCh, str, false);
	}
	//cvReleaseImage(&tmp);

	cvReleaseStructuringElement(&element);
	return 0;
}

//
// Dialate / Erode
//
int CImgProcEngine::Expansion(CRoiData *pData, IplImage* grayImg, int t, int nDbg, int h)
{
	CString str;

	if (t < 0)
		t = _ProcessValue1;
	int nExpansion1 = 0;
	if (pData != nullptr) {
		CParam *pParam = pData->getParam(_T("Expansion 1"), t);
		if (pParam)
			nExpansion1 = _ttoi(pParam->Value.c_str());
	}
	int nExpansion2 = 0;
	if (pData != nullptr) {
		CParam *pParam = pData->getParam(_T("Expansion 2"), t);
		if (pParam)
			nExpansion2 = _ttoi(pParam->Value.c_str());
	}
	if (nExpansion1 == 0 && nExpansion2 == 0)
		return 0;

	//IplImage* tmp = cvCreateImage(cvGetSize(grayImg), 8, 1);
	if (nExpansion1 < 0)
		cvErode(grayImg, grayImg, nullptr, -nExpansion1);
	else  //if (nExpansion > 0)
		cvDilate(grayImg, grayImg, nullptr, nExpansion1);

	if (nExpansion2 < 0)
		cvErode(grayImg, grayImg, nullptr, -nExpansion2);
	else //if (nExpansion > 0)
		cvDilate(grayImg, grayImg, nullptr, nExpansion2);

	if (gCfg.m_bSaveEngineImg) {
		str.Format(_T("%03d_cvExpansion.jpg"), nDbg);
		SaveOutImage(grayImg, pData->m_nCh, str, false);
	}
	//cvReleaseImage(&tmp);
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

int CImgProcEngine::SinglePattIdentify(int nCh, IplImage* grayImage, CRoiData *pData, CRect rect)
{

    if (pData == nullptr)
        return -1;
    if (pData->iplTemplate == nullptr)
        return -1;
    CString str;

    CvSize searchSize = cvSize(grayImage->width, grayImage->height);
    IplImage* graySearchImg = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);
    if (grayImage->nChannels == 3)
        cvCvtColor(grayImage, graySearchImg, CV_RGB2GRAY);
    else
        cvCopy(grayImage, graySearchImg);

    //static CvPoint2D32f cog = { 0, 0 };

    CvSize templateSize = cvSize(pData->iplTemplate->width, pData->iplTemplate->height);
    IplImage* grayTemplateImg = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);
    if (pData->iplTemplate->nChannels == 3)
        cvCvtColor(pData->iplTemplate, grayTemplateImg, CV_RGB2GRAY);
    else
        cvCopy(pData->iplTemplate, grayTemplateImg);


    double dMatchShapes = 0;
    double MatchRate = 0, LimitMatchRate = 40;

    CParam *pParam = pData->getParam(_T("Pattern matching rate"));
    if (pParam) LimitMatchRate = (int)_ttof(pParam->Value.c_str());

    if (gCfg.m_bSaveEngineImg)
    {
        CString str; str.Format(_T("%d_TemplateImage0.jpg"), 140);
        SaveOutImage(pData->iplTemplate, pData->m_nCh, str);
    }

    double dAngle = 0.0;
    double dAngleStep = 0.0;
    CvPoint left_top = { 0, 0 };
    pParam = pData->getParam(_T("Rotate angle"));
    if (pParam) dAngle = (double)_ttof(pParam->Value.c_str());
    pParam = pData->getParam(_T("Angle step"));
    if (pParam) dAngleStep = (int)_ttof(pParam->Value.c_str());

    if (dAngle > 0.0) // computing power가 낮은 시스템은 사용하지말자.
    {
		CRect srect = pData->m_RoiArea.GetRect();	// Area로 등록된 ROI
        CvSize size = cvSize(srect.Width() - grayTemplateImg->width + 1, srect.Height() - grayTemplateImg->height + 1);
        IplImage* C = cvCreateImage(size, IPL_DEPTH_32F, 1); // 상관계수를 구할 이미지(C)
        double min, max;
        double rate = LimitMatchRate / 100.0;
        std::vector<std::pair<double, double>> pairs;
        IplImage *clone = cvCloneImage(grayTemplateImg);
        int cnt = 0;
        for (double a = -dAngle; a < dAngle; a=a+dAngleStep) // 패턴을 -30 에서 30도까지 돌려가면서 매칭율이 가장좋은 이미지를 찾는다.
        {
            cvCopy(grayTemplateImg, clone);
            RotateImage(clone, a);

            if (gCfg.m_bSaveEngineImg)
            {
                cnt++;
                CString str; str.Format(_T("%d_Template%d.jpg"), 149, cnt);
                SaveOutImage(clone, pData->m_nCh, str);
            }

            cvMatchTemplate(graySearchImg, clone, C, CV_TM_CCOEFF_NORMED); // 제곱차 매칭
            cvMinMaxLoc(C, &min, &max, nullptr, &left_top); // 상관계수가 최대값을 값는 위치 찾기
            if (max > rate) {
                pairs.push_back(std::pair<double, double>(a, max));
            }
        }
        cvReleaseImage(&C);

        std::stable_sort(pairs.begin(), pairs.end(), [=](const std::pair<double, double>& a, const std::pair<double, double>& b)
        {
            return a.second > b.second; // descending
        });

        if (pairs.size() > 0) {
            std::pair<double, double> a = pairs[0];
            cvCopy(grayTemplateImg, clone);
            RotateImage(clone, a.first);
            cvCopy(clone, grayTemplateImg);
            CString str;
            str.Format(_T("big match : %f %f"), a.first, a.second);
            //qDebug() << "big match : " << a.first << a.second;
        	g_cLog->AddLog(str, _LOG_LIST_SYS);
		}
        cvReleaseImage(&clone);
    }

    if (gCfg.m_bSaveEngineImg)
    {
        CString str; str.Format(_T("%d_TemplateImage1.jpg"), 150);
        SaveOutImage(grayTemplateImg, pData->m_nCh, str);
    }

    // Opencv Template Matching을 이용해서 Template 을 찾는다.
    // 이미지 Template Matching이기때문에 부정확한것은 cvMatchShapes()로 보완한다.
    //CString strMsg;
    MatchRate = TemplateMatch(pData, graySearchImg, grayTemplateImg, left_top, dMatchShapes);
    if (LimitMatchRate <= MatchRate)
    {
        //strMsg.Format(("TemplateMatch Result Success ===> : %.2f%%"), MatchRate);
        //MSystem.DevLogSave(("%s"), strMsg);

        if (m_DetectResult.result == true)
        {
			m_DetectResult.resultType = RESULTTYPE_RECT;
            m_DetectResult.pt = CvPoint2D32f(left_top.x, left_top.y);
            m_DetectResult.tl = CvPoint2D32f(left_top.x, left_top.y);
            m_DetectResult.br = CvPoint2D32f(left_top.x + pData->iplTemplate->width, left_top.y + pData->iplTemplate->height);
            m_DetectResult.dRadius = 0;
            m_DetectResult.dAngle = 0;
            pData->m_vecDetectResult.push_back(m_DetectResult);
        }
    }
    else {
        left_top = { 0, 0 };
    }

    str.Format(_T("SinglePattIdentify MatchRate:%.1f MatchShapes:%.1f (%s)"),
                MatchRate, dMatchShapes, m_DetectResult.result == true ? _T("OK") : _T("NG"));
    //qDebug() << str;
    //MSystem.m_pFormBottom->SetBottomMessage(str);
	g_cLog->AddLog(str, _LOG_LIST_SYS);

    return 0;
}



int CImgProcEngine::SinglePattMatchShapes(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rectIn)
{
    if (pData->iplTemplate == nullptr)
        return -1;

    CString str;
    int retry = 0;
    CParam *pParam;
    IplImage* grayImg = nullptr;
    //int nThresholdLowValue;
    int nThresholdHighValue;

    pData->m_vecDetectResult.clear();
    if (grayImg != nullptr)
        cvReleaseImage(&grayImg);
    grayImg = cvCloneImage(croppedImage);

    //nThresholdLowValue = 0;
    //pParam = pData->getParam(("Low Threshold"), _ProcessValue1+retry);
    //if (pParam)
    //    nThresholdLowValue = pParam->Value.toDouble();

    nThresholdHighValue = 255;
    pParam = pData->getParam(_T("High Threshold"), _ProcessValue1+retry);
    if (pParam)
        nThresholdHighValue = (int)_ttof(pParam->Value.c_str());

    float dMatchShapesingRate = 0.7f;
    if (pData != nullptr) {
        CParam *pParam = pData->getParam(_T("Shape matching rate"));
        if (pParam)
            dMatchShapesingRate = (double)_ttof(pParam->Value.c_str()) / 100.0f;
    }

    if (gCfg.m_bSaveEngineImg)
    {
        CString str; str.Format(_T("%d_Src.jpg"), 200);
        SaveOutImage(grayImg, pData->m_nCh, str);
    }

    if (nThresholdHighValue == 0)
        ThresholdOTSU(pData, grayImg, 211);
    else
        ThresholdRange(pData, grayImg, 211);

    if (gCfg.m_bSaveEngineImg)
    {
        CString str; str.Format(_T("%d_Threshold.jpg"), 203);
        SaveOutImage(grayImg, pData->m_nCh, str);
    }

    NoiseOut(pData, grayImg, _ProcessValue1, 212);
    Expansion(pData, grayImg, _ProcessValue1, 213);
    //IplImage *grayImg1 = cvCloneImage(grayImg);

    ///////////////////////////////////////////////////////////

    CvSize templateSize = cvSize(pData->iplTemplate->width, pData->iplTemplate->height);
    IplImage* grayTemplateImg = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);
    if (pData->iplTemplate->nChannels == 3)
        cvCvtColor(pData->iplTemplate, grayTemplateImg, CV_RGB2GRAY);
    else
        cvCopy(pData->iplTemplate, grayTemplateImg);

     if (gCfg.m_bSaveEngineImg)
    {
        str.Format(_T("%d_grayTemplateImg.jpg"), 250);
        SaveOutImage(grayTemplateImg, pData->m_nCh, str);
    }


    IplImage* g2 = cvCreateImage(cvSize(grayTemplateImg->width, grayTemplateImg->height), IPL_DEPTH_8U, 1);
    cvCopy(pData->iplTemplate, g2);

    if (nThresholdHighValue == 0)
        ThresholdOTSU(pData, g2, 255);
    else
        ThresholdRange(pData, g2, 255);
    NoiseOut(pData, g2, _ProcessValue1, 260);
    Expansion(pData, g2, _ProcessValue1, 261);
	FilterIncludeLargeBlob(g2);

    cvCanny(g2, g2, 100, 300, 3);
    if (gCfg.m_bSaveEngineImg){
        SaveOutImage(g2, pData->m_nCh, _T("265_TemplateImageCany.jpg"));
    }

    CvMemStorage *s2 = cvCreateMemStorage(0); //storage area for all contours 모든 형상들을 위한 저장공간.
    CvSeq* c2 = 0;         // 경계 계수를 저장할 변수
    cvFindContours(g2, s2, &c2, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    if (c2 == nullptr || c2->total <= 0) {
        if (grayTemplateImg) cvReleaseImage(&grayTemplateImg);
        if (c2) cvClearSeq(c2);
        if (s2) cvReleaseMemStorage(&s2);
        return 0;
    }
    CvSeq* result2 = cvApproxPoly(c2, sizeof(CvContour), s2, CV_POLY_APPROX_DP, cvContourPerimeter(c2)*0.001, 1);

    CvRect boundbox2 = cvBoundingRect(result2);
    for (int i = 0; i < result2->total; ++i)
    {
        CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, result2, i);
        p->x = p->x - boundbox2.x;
        p->y = p->y - boundbox2.y;
    }

    if (gCfg.m_bSaveEngineImg)
    {
        IplImage* drawImage = cvCreateImage(cvSize(g2->width, g2->height), grayImg->depth, grayImg->nChannels);
        cvZero(drawImage);
        cvDrawContours(drawImage, result2, CVX_WHITE, CVX_WHITE, 1, 1, 8); // 외곽선 근사화가 잘되었는지 테스트
        str.Format(_T("266_cvApproxPoly_Template.jpg"));
        SaveOutImage(drawImage, pData->m_nCh, str, false);
        if (drawImage) cvReleaseImage(&drawImage);
    }

    ///////////////////////////////////////////////////////////

    CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours 모든 형상들을 위한 저장공간.
    CvSeq* contours = 0;         // 경계 계수를 저장할 변수

    int seq = 0;
    cvCanny(grayImg, grayImg, 100, 300, 3);
    cvFindContours(grayImg, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int total = contours->total;
	if (gCfg.m_bSaveEngineImg)
	{
		IplImage* drawImage = cvCreateImage(cvSize(grayImg->width, grayImg->height), grayImg->depth, grayImg->nChannels);
		cvZero(drawImage);
		cvDrawContours(drawImage, contours, CVX_WHITE, CVX_WHITE, 1, 1, 8); // 외곽선 근사화가 잘되었는지 테스트
		str.Format(_T("269_cvApproxPoly.jpg"));
		SaveOutImage(drawImage, pData->m_nCh, str, false);
		if (drawImage) cvReleaseImage(&drawImage);
	}

    //iterating through each contour
    while(contours)
    {
        //obtain a sequence of points of contour, pointed by the variable 'contour' 형상의 점들의 시퀀스 가져오기, 인자 ‘contour’에 의해 지정된
        CvSeq* result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.001, 1);
        CvRect boundbox = cvBoundingRect(contours);
        //if (boundbox.x+boundbox.width < grayImg->width && boundbox.y+boundbox.height < grayImg->height)
        {
            if (abs(result2->total - result->total) <= 10)
            {
				CvSeq* ptseq = cvCreateSeq((CV_SEQ_KIND_CURVE | CV_SEQ_ELTYPE_POINT | CV_SEQ_FLAG_CLOSED) | CV_32SC2, sizeof(CvContour), sizeof(CvPoint), storage);
				for (int i = 0; i < result->total; ++i)
				{
					CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, result, i);
					p->x = p->x - boundbox.x;
					p->y = p->y - boundbox.y;
					cvSeqPush(ptseq, p);
				}

                if (gCfg.m_bSaveEngineImg)
                {
					//IplImage* drawImage1 = cvCreateImage(cvSize(grayImg->width, grayImg->height), grayImg->depth, grayImg->nChannels);
					//cvZero(drawImage1);
					//cvDrawContours(drawImage1, contours, CVX_WHITE, CVX_WHITE, 1, 1, 8); // 외곽선 근사화가 잘되었는지 테스트
					//str.Format(_T("270_cvApproxPoly1_%d.jpg"), seq);
					//SaveOutImage(drawImage1, pData->m_nCh, str, false);
					//if (drawImage1) cvReleaseImage(&drawImage1);

                    IplImage* drawImage = cvCreateImage(cvSize(g2->width, g2->height), grayImg->depth, grayImg->nChannels);
                    cvZero(drawImage);
                    cvDrawContours(drawImage, ptseq, CVX_WHITE, CVX_WHITE, 1, 1, 8); // 외곽선 근사화가 잘되었는지 테스트
                    str.Format(_T("270_cvApproxPoly2_%d.jpg"), seq);
                    SaveOutImage(drawImage, pData->m_nCh, str, false);
                    if (drawImage) cvReleaseImage(&drawImage);
                }
				cvClearSeq(ptseq);

				// result - search image
				// result2 - template image
                double matching = cvMatchShapes(result, result2, CV_CONTOURS_MATCH_I1); // 작은 값 일수록 두 이미지의 형상이 가까운 (0은 같은 모양)라는 것이된다.
                //double matching2 = cvMatchShapes(result, result2, CV_CONTOURS_MATCH_I2);
                //double matching3 = cvMatchShapes(result, result2, CV_CONTOURS_MATCH_I3);
                //qDebug() << "matching" << matching << matching2 << matching3;
                if (matching > 1.0)
                    matching = 1.0;
                double dMatchShapes = (1.0-matching) * 100.0;
                if (matching <= (1.0 - dMatchShapesingRate))
                {
                    //str.Format(_T("Template Shape Match(%d) ===> : %.2f%%  %d:%d"), seq, dMatchShapes, result2->total, result->total);
					//g_cLog->AddLog(str, _LOG_LIST_SYS);

                    CvSeq* ptseq = cvCreateSeq((CV_SEQ_KIND_CURVE | CV_SEQ_ELTYPE_POINT | CV_SEQ_FLAG_CLOSED) | CV_32SC2, sizeof(CvContour), sizeof(CvPoint), storage);
                    for (int i = 0; i < result->total; ++i)
                    {
                        CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, result, i);
                        cvSeqPush(ptseq, p);
                    }
                    Point2f centerPoint = CenterOfMoment(ptseq);
                    cvClearSeq(ptseq);

                    //qDebug() << "centerPoint1 " << centerPoint.x << centerPoint.y ;

                    centerPoint.x += boundbox.x;
                    centerPoint.y += boundbox.y;

                    //qDebug() << "centerPoint2 " << centerPoint.x << centerPoint.y ;

					m_DetectResult.resultType = RESULTTYPE_POINT;
                    m_DetectResult.dMatchRate = dMatchShapes;
                    m_DetectResult.pt = centerPoint;
                    pData->m_vecDetectResult.push_back(m_DetectResult);
                }
            }
        }

        //obtain the next contour 다음 형상 가져오기
        contours = contours->h_next;
        seq++;
    }

    int size = pData->m_vecDetectResult.size();
    if (size >= 1) {
        std::stable_sort(pData->m_vecDetectResult.begin(), pData->m_vecDetectResult.end(), [](const DetectResult lhs, const DetectResult rhs)->bool {
            if (lhs.dMatchRate > rhs.dMatchRate) // descending
                return true;
            return false;
        });
        m_DetectResult = pData->m_vecDetectResult[0];
        pData->m_vecDetectResult.clear();
        pData->m_vecDetectResult.push_back(m_DetectResult);
        str.Format(_T("Selected template Shape Match ===> : %.2f%%"), m_DetectResult.dMatchRate);
    	g_cLog->AddLog(str, _LOG_LIST_SYS);
	}

    if (contours) cvClearSeq(contours);
    if (storage) cvReleaseMemStorage(&storage);
    if (c2) cvClearSeq(c2);
    if (s2) cvReleaseMemStorage(&s2);


    if (grayImg) cvReleaseImage(&grayImg);
    //if (grayImg1) cvReleaseImage(&grayImg1);
    if (grayTemplateImg) cvReleaseImage(&grayTemplateImg);


    return 0;
}

int CImgProcEngine::SinglePattFeatureMatch(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rectIn)
{
    //if (pData->iplTemplate == nullptr)
    //    return -1;

    CParam *pParam;
    int nMethod = 0;
    pParam = pData->getParam(_T("Method"));
    if (pParam)
        nMethod = (int)_ttof(pParam->Value.c_str());
    double dParam1 = 0;
    pParam = pData->getParam(_T("Param1"));
    if (pParam)
        dParam1 = (double)_ttof(pParam->Value.c_str());

    double kDistanceCoef = 0;
    pParam = pData->getParam(_T("kDistanceCoef"));
    if (pParam)
        kDistanceCoef = (double)_ttof(pParam->Value.c_str());
    double kMaxMatchingSize = 0;
    pParam = pData->getParam(_T("kMaxMatchingSize"));
    if (pParam)
        kMaxMatchingSize = (double)_ttof(pParam->Value.c_str());


    //std::string detectorName;
    //detectorName = ui->comboBoxDetector->currentText().toLatin1().data(); // "SURF";
    std::vector<cv::KeyPoint> keypoints_object, keypoints_scene;
    cv::Mat descriptors_object, descriptors_scene;
    vector<vector<cv::DMatch>> m_knnMatches;
    std::vector< cv::DMatch > good_matches;

    cv::Mat img_object = cv::cvarrToMat(pData->iplTemplate);
    cv::Mat img_scene =  cv::cvarrToMat(croppedImage);

    if (nMethod == 1) { // SIFT distance:10, MaxSize:200
        SIFTDetector sift(dParam1); //  nParam1=0
        sift(img_object, cv::Mat(), keypoints_object, descriptors_object);
        sift(img_scene, cv::Mat(), keypoints_scene, descriptors_scene);

        cv::BFMatcher matcher;
        matcher.knnMatch(descriptors_object, descriptors_scene, m_knnMatches, 2);
    }
    else if (nMethod == 0) { // SURF distance:5, MaxSize:200
        SURFDetector surf(dParam1); //  nParam1=800
        surf(img_object, cv::Mat(), keypoints_object, descriptors_object);
        surf(img_scene, cv::Mat(), keypoints_scene, descriptors_scene);

        cv::BFMatcher matcher;
        matcher.knnMatch(descriptors_object, descriptors_scene, m_knnMatches, 2);
    }
    else if (nMethod == 2) { // ORB distance:5, MaxSize:200
        ORBDetector orb(dParam1); // nParam1=2000
        orb(img_object, cv::Mat(), keypoints_object, descriptors_object);
        orb(img_scene, cv::Mat(), keypoints_scene, descriptors_scene);

        cv::BFMatcher matcher(cv::NORM_HAMMING); // use cv::NORM_HAMMING2 for ORB descriptor with WTA_K == 3 or 4 (see ORB constructor)
        matcher.knnMatch(descriptors_object, descriptors_scene, m_knnMatches, 2);
    }
    else return -1;

    for(int i = 0; i < min(img_scene.rows-1,(int) m_knnMatches.size()); i++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
    {
        const cv::DMatch& bestMatch = m_knnMatches[i][0];
        if((int)m_knnMatches[i].size()<=2 && (int)m_knnMatches[i].size()>0)
        {
            good_matches.push_back(bestMatch);
        }
    }

    if (good_matches.size() == 0)
        return -1;

    std::sort(good_matches.begin(), good_matches.end());
    while (good_matches.front().distance * kDistanceCoef < good_matches.back().distance) {
        good_matches.pop_back();
    }
    while (good_matches.size() > kMaxMatchingSize) {
        good_matches.pop_back();
    }

    std::vector<cv::Point2f> corner;
    cv::Mat img_matches = drawGoodMatches(img_object, img_scene,
                                keypoints_object, keypoints_scene, good_matches, corner);

    //-- Show detected matches
    if (img_matches.rows > 0)
    {
        cv::namedWindow("knnMatch", 0);
        cv::imshow("knnMatch", img_matches);
    }

    if (corner.size() > 0)
    {
        cv::Point pt1 = corner[0];
        cv::Point pt2 = corner[1];
        cv::Point pt3 = corner[2];
        cv::Point pt4 = corner[3];

        m_DetectResult.resultType = RESULTTYPE_RECT4P;
        m_DetectResult.tl = CvPoint2D32f(pt1);
        m_DetectResult.tr = CvPoint2D32f(pt2);
        m_DetectResult.br = CvPoint2D32f(pt3);
        m_DetectResult.bl = CvPoint2D32f(pt4);
        pData->m_vecDetectResult.push_back(m_DetectResult);

//        cvLine(outImg, pt1, pt2, cv::Scalar(128, 128, 128), 2, cv::LINE_AA);
//        cvLine(outImg, pt2, pt3, cv::Scalar(128, 128, 128), 2, cv::LINE_AA);
//        cvLine(outImg, pt3, pt4, cv::Scalar(128, 128, 128), 2, cv::LINE_AA);
//        cvLine(outImg, pt4, pt1, cv::Scalar(128, 128, 128), 2, cv::LINE_AA);
//        m_DetectResult.rect = QRectF(QPointF(0,0),QPointF(0,0));
//        pData->m_vecDetectResult.push_back(m_DetectResult);

//        cvRectangle(iplImg2, pt1, pt3, cvScalar(255, 255, 255), CV_FILLED); // test
//        theMainWindow->outWidget("test1", iplImg2);

    }

    return 0;
}

cv::Mat CImgProcEngine::drawGoodMatches(
    const cv::Mat& img1,
    const cv::Mat& img2,
    const std::vector<cv::KeyPoint>& keypoints1,
    const std::vector<cv::KeyPoint>& keypoints2,
    std::vector<cv::DMatch>& good_matches,
    std::vector<cv::Point2f>& scene_corners_
)
{
    cv::Mat img_matches;
    if (good_matches.size() == 0)
        return img_matches;

    cv::drawMatches(img1, keypoints1, img2, keypoints2,
        good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
        std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //-- Localize the object
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

    for (size_t i = 0; i < good_matches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        obj.push_back(keypoints1[good_matches[i].queryIdx].pt);
        scene.push_back(keypoints2[good_matches[i].trainIdx].pt);
    }
    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<cv::Point2f> obj_corners(4);
    obj_corners[0] = cv::Point(0, 0);
    obj_corners[1] = cv::Point(img1.cols, 0);
    obj_corners[2] = cv::Point(img1.cols, img1.rows);
    obj_corners[3] = cv::Point(0, img1.rows);
    std::vector<cv::Point2f> scene_corners(4);

    cv::Mat H;
    try {
        H = cv::findHomography(obj, scene, cv::RANSAC);
        perspectiveTransform(obj_corners, scene_corners, H);
    } catch (...) {
        //qDebug() << _T("Error <unknown> findHomography()");
        return img_matches;
    }

    scene_corners_ = scene_corners;

    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
//    cv::Point pt1 = scene_corners[0] + cv::Point2f((float)img1.cols, 0);
//    cv::Point pt2 = scene_corners[1] + cv::Point2f((float)img1.cols, 0);
//    cv::Point pt3 = scene_corners[2] + cv::Point2f((float)img1.cols, 0);
//    cv::Point pt4 = scene_corners[3] + cv::Point2f((float)img1.cols, 0);
//    line(img_matches, pt1, pt2, cv::Scalar(128, 128, 128), 2, cv::LINE_AA);
//    line(img_matches, pt2, pt3, cv::Scalar(128, 128, 128), 2, cv::LINE_AA);
//    line(img_matches, pt3, pt4, cv::Scalar(128, 128, 128), 2, cv::LINE_AA);
//    line(img_matches, pt4, pt1, cv::Scalar(128, 128, 128), 2, cv::LINE_AA);

    return img_matches;
}

int CImgProcEngine::SingleROIFindShape(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rectIn)
{
    CString str;
    int retry = 0;
    //RANSACIRCLE *pCircle;
    CParam *pParam;
    IplImage* grayImg = nullptr;
    int nThresholdLowValue;
    int nThresholdHighValue;
    int nMinArea;
    //int nMaxArea;
    //IplImage* grayImg1;

    pData->m_vecDetectResult.clear();
    if (grayImg != nullptr)
        cvReleaseImage(&grayImg);
    grayImg = cvCloneImage(croppedImage);

    nThresholdLowValue = 0;
    pParam = pData->getParam(_T("Low Threshold"), _ProcessValue1+retry);
    if (pParam)
        nThresholdLowValue = (int)_ttof(pParam->Value.c_str());

    nThresholdHighValue = 255;
    pParam = pData->getParam(_T("High Threshold"), _ProcessValue1+retry);
    if (pParam)
        nThresholdHighValue = (int)_ttof(pParam->Value.c_str());

    int nType = -1;
    nMinArea = 0;
    //nMaxArea = 100000;
    pParam = pData->getParam(_T("Minimum area"));
    if (pParam)
        nMinArea = (int)_ttof(pParam->Value.c_str());
    //pParam = pData->getParam("Maximum area");
    //if (pParam)
    //    nMaxArea = (int)pParam->Value.toDouble();
    pParam = pData->getParam(_T("Type"));
    if (pParam)
        nType = (int)_ttof(pParam->Value.c_str());

    if (gCfg.m_bSaveEngineImg)
    {
        CString str; str.Format(_T("%d_Src.jpg"), 200);
        SaveOutImage(grayImg, pData->m_nCh, str);
    }

    cvInRangeS(grayImg, Scalar(nThresholdLowValue), Scalar(nThresholdHighValue), grayImg);

    if (gCfg.m_bSaveEngineImg)
    {
        CString str; str.Format(_T("%d_Threshold.jpg"), 203);
        SaveOutImage(grayImg, pData->m_nCh, str);
    }

    NoiseOut(pData, grayImg, _ProcessValue1, 212);
    Expansion(pData, grayImg, _ProcessValue1, 213);

    CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours 모든 형상들을 위한 저장공간.
    CvSeq* contours = 0;         // 경계 계수를 저장할 변수
    CvSeq* result;   //hold sequence of points of a contour 형상의 포인터의 시퀀스 잡기.

    cvFindContours(grayImg, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    //iterating through each contour 각형상별로 반복
    while(contours)
    {
        //obtain a sequence of points of contour, pointed by the variable 'contour' 형상의 점들의 시퀀스 가져오기, 인자 ‘contour’에 의해 지정된
        result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.01, 0);
        double area = cvContourArea(contours);
        if (area > nMinArea)  //면적이 일정크기 이상이어야 한다.
        {
            if (gCfg.m_bSaveEngineImg)
            {
                IplImage* drawImage = cvCreateImage(cvSize(grayImg->width, grayImg->height), grayImg->depth, 3);
                cvZero(drawImage);
                cvDrawContours(drawImage, result, CVX_WHITE, CVX_WHITE, 1, 1, 8); // 외곽선 근사화가 잘되었는지 테스트
                str.Format(_T("220_cvApproxPoly.jpg"));
                SaveOutImage(drawImage, pData->m_nCh, str, false);
                if (drawImage) cvReleaseImage(&drawImage);
            }

            vector<Point> approx;
            int size = result->total;
            CvSeq* c = result;
            //for (CvSeq* c = result; c != nullptr; c = c->h_next)  // 엣지의 링크드리스트를 순회하면서 각 엣지들에대해서 출력한다.
            for (int i = 0; i < size; i++)
            {
                // CvSeq로부터좌표를얻어낸다.
                CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, c, i);
                approx.push_back(*p);
            }
            //모든 코너의 각도를 구한다.
            vector<int> angle;
            //cout << "===" << size << endl;
            for (int k = 0; k < size; k++) {
                int ang = GetAngleABC(approx[k], approx[(k + 1) % size], approx[(k + 2)%size]);
                //cout << k<< k+1<< k+2<<"@@"<< ang << endl;
                angle.push_back(ang);
            }

            std::sort(angle.begin(), angle.end());
            int minAngle = angle.front();
            int maxAngle = angle.back();
            int threshold = 8;

            int type = -1;
            //도형을 판정한다.
            if ( size == 3 ) // triangle
                type = 0;
            else if ( size == 4 && maxAngle < 120 ) // && minAngle >= 90- threshold && maxAngle <= 90+ threshold) // rectangle
            {
                IplImage* img = cvCreateImage(cvSize(20, 20), IPL_DEPTH_8U, 1);
                cvZero(img);
                cvRectangle(img, cvPoint(5, 5), cvPoint(15,15), CV_RGB(255, 225, 255), CV_FILLED); // filled rectangle.

                CvMemStorage *s2 = cvCreateMemStorage(0); //storage area for all contours 모든 형상들을 위한 저장공간.
                CvSeq* c2 = 0;         // 경계 계수를 저장할 변수

                cvFindContours(img, s2, &c2, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
                double matching = cvMatchShapes(c2, c, CV_CONTOURS_MATCH_I2); // 작은 값 일수록 두 이미지의 형상이 가까운 (0은 같은 모양)라는 것이된다.
                if (matching> 0.5)
                   str = _T("Parallelogram");
               else if (0.3 < matching && matching < 0.5)
                   str = _T("rectangle");
               else if (0.02 < matching && matching < 0.3)
                   str = _T("Rhombus");
               else
                   str = _T("Square");
			   g_cLog->AddLog(str, _LOG_LIST_SYS);

               if (img) cvReleaseImage(&img);
               if (c2) cvClearSeq(c2);
               if (s2) cvReleaseMemStorage(&s2);

               type = 1;
            } else if (size == 5 && minAngle >= 108- threshold && maxAngle <= 108+ threshold) // pentagon
            {
               str = _T("pentagon");
			   g_cLog->AddLog(str, _LOG_LIST_SYS);
			   type = 2;
            } else if (size == 6 && minAngle >= 120 - threshold && maxAngle <= 140 + threshold) // hexagon
            {
               str = _T("hexagon");
			   g_cLog->AddLog(str, _LOG_LIST_SYS);
			   type = 3;
            } else {
                // if found convex area is ~ PI*r² then it is probably a circle
               float radius;
               CvPoint2D32f center;
               cvMinEnclosingCircle(c, &center, &radius);
               double tmp = 1.0 - (area / (PI * pow(radius, 2)));
               if (tmp < 0.5) {
                   str = _T("circle");
				   g_cLog->AddLog(str, _LOG_LIST_SYS);
				   type = 4;
               }
            }

            if (type >= 0 && nType == type)
            {
                CvSeq* ptseq = cvCreateSeq((CV_SEQ_KIND_CURVE | CV_SEQ_ELTYPE_POINT | CV_SEQ_FLAG_CLOSED) | CV_32SC2, sizeof(CvContour), sizeof(CvPoint), storage);
                for (int i = 0; i < size; ++i)
                {
                    CvPoint* p = CV_GET_SEQ_ELEM(CvPoint, c, i);
                    cvSeqPush(ptseq, p);
                }
                Point2f centerPoint = CenterOfMoment(ptseq);
                cvClearSeq(ptseq);

				m_DetectResult.resultType == RESULTTYPE_POINT;
                m_DetectResult.pt = centerPoint;
                pData->m_vecDetectResult.push_back(m_DetectResult);
                break;
            }
        }
        //obtain the next contour 다음 형상 가져오기
        contours = contours->h_next;
    }

    if (contours) cvClearSeq(contours);
    if (storage) cvReleaseMemStorage(&storage);


    if (grayImg) cvReleaseImage(&grayImg);

    return 0;
}

double CImgProcEngine::TemplateMatch(CRoiData *pData, IplImage* graySearchImgIn, IplImage* grayTemplateImg, CvPoint &left_top, double &dMatchShapes)
{
    CString str;
	CRect srect = pData->m_RoiArea.GetRect();	// Area로 등록된 ROI
    IplImage* graySearchImg = cvCloneImage(graySearchImgIn);

	float dMatchingRate = 0.5f;
	float dMatchShapesingRate = 0;
	if (pData != nullptr) {
		CParam *pParam = pData->getParam(_T("Pattern matching rate"));
		if (pParam)
			dMatchingRate = (double)_ttof(pParam->Value.c_str()) / 100.0f;
	}
	if (pData != nullptr) {
		CParam *pParam = pData->getParam(_T("Shape matching rate"));
		if (pParam)
			dMatchShapesingRate = (double)_ttof(pParam->Value.c_str()) / 100.0f;
	}
	int nThresholdValue = 0;
	CParam *pParam = pData->getParam(_T("High Threshold"));
	if (pParam)
		nThresholdValue = (int)_ttof(pParam->Value.c_str());
	int nFilterBlob = 0;
	pParam = pData->getParam(_T("Filter blob"));
	if (pParam)
		nFilterBlob = (int)_ttof(pParam->Value.c_str());


    if (gCfg.m_bSaveEngineImg)
    {
        //str.Format(("%d_graySearchImg.jpg"), 200);
        //SaveOutImage(graySearchImg, pData->m_nCh, str);

        str.Format(_T("%d_grayTemplateImg.jpg"), 201);
        SaveOutImage(grayTemplateImg, pData->m_nCh, str);
    }

    CvSize size = cvSize(srect.Width() - grayTemplateImg->width + 1, srect.Height() - grayTemplateImg->height + 1);
    IplImage* C = cvCreateImage(size, IPL_DEPTH_32F, 1); // 상관계수를 구할 이미지(C)
    double min, max;

    IplImage* g2 = cvCreateImage(cvSize(grayTemplateImg->width, grayTemplateImg->height), IPL_DEPTH_8U, 1);
    cvCopy(pData->iplTemplate, g2);


	if (dMatchShapesingRate > 0)
	{
		if (nThresholdValue == 0)
			ThresholdOTSU(pData, g2, 211);
		else
			ThresholdRange(pData, g2, 211);

		NoiseOut(pData, g2, _ProcessValue2, 212);
		Expansion(pData, g2, _ProcessValue2, 213);

		// 가장큼 or 긴 blob만 남긴다.
		if (nFilterBlob == 1)
			FilterLargeArea(g2);
		else if (nFilterBlob == 2)
			FilterLargeDiameter(g2);
		if (gCfg.m_bSaveEngineImg) {
			SaveOutImage(g2, pData->m_nCh, _T("226_FilterBlob.jpg"));
		}

		NoiseOut(pData, g2, _ProcessValue2, 231);
		Expansion(pData, g2, _ProcessValue2, 232);
		cvCanny(g2, g2, 100, 300, 3);
		if (gCfg.m_bSaveEngineImg) {
			SaveOutImage(g2, pData->m_nCh, _T("227_TemplateImageCany.jpg"));
		}
	}
    CvMemStorage* storage = cvCreateMemStorage(0);

    dMatchShapes = 0.0;
    int nLoop = 0;
    double maxRate = 0;
    while (1)
    {
        cvMatchTemplate(graySearchImg, grayTemplateImg, C, CV_TM_CCOEFF_NORMED); // 제곱차 매칭
        cvMinMaxLoc(C, &min, &max, nullptr, &left_top); // 상관계수가 최대값을 값는 위치 찾기
        //str.Format(("MatchTemplate : %.3f"), max);
        //g_cLog->AddLog(_LOG_LIST_SYS, str);
        if (maxRate < max)
            maxRate = max;

        if (max >= dMatchingRate)	// OpenCV의 Template Matching 함수를 이용해서 유사한 패턴을 찾은다음, Shape 기능을 이용하여 한번더 검사한다.
        {
            str.Format(_T("TemplateMatch step%d : %.2f%%"), nLoop, max*100);
            //g_cLog->AddLog(_LOG_LIST_SYS, str);

            cvSetImageROI(graySearchImg, cvRect(left_top.x, left_top.y, grayTemplateImg->width, grayTemplateImg->height));
            if (gCfg.m_bSaveEngineImg)
            {
                str.Format(_T("242_MatchImage%d.jpg"), nLoop);
                SaveOutImage(graySearchImg, pData->m_nCh, str);
            }

            if (dMatchShapesingRate > 0)
            {
                IplImage* g1 = cvCreateImage(cvSize(grayTemplateImg->width, grayTemplateImg->height), IPL_DEPTH_8U, 1);

                cvCopy(graySearchImg, g1); // cvSetImageROI() 만큼 Copy
                if (nThresholdValue == 0)
                    ThresholdOTSU(pData, g1, 243);
                else
                    ThresholdRange(pData, g1, 243);

                NoiseOut(pData, g1, _ProcessValue2, 244); // 노이즈 제거
                Expansion(pData, g1, _ProcessValue2, 245);

                // 가장큼 or 긴 blob만 남긴다.
                if (nFilterBlob == 1)
                    FilterLargeArea(g1);
                else if (nFilterBlob == 2)
                    FilterLargeDiameter(g1);
                if (gCfg.m_bSaveEngineImg){
                    SaveOutImage(g1, pData->m_nCh, _T("248_FilterBlob.jpg"));
                }

                NoiseOut(pData, g1, _ProcessValue2, 251);
                Expansion(pData, g1, _ProcessValue2, 252);
                cvCanny(g1, g1, 100, 300, 3);
                if (gCfg.m_bSaveEngineImg){
                    str.Format(_T("255_TemplateImageCany%d.jpg"), nLoop);
                    SaveOutImage(g1, pData->m_nCh, str);
                }

                // g1이 전부 0로 채워져있으면 cvMatchShapes()에서 0로 리턴되어서 zero image filtering
                //IplImage* c1 = cvCloneImage(g1);
                //CvSeq* contours = 0;
                //cvFindContours(c1, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL);
                //cvReleaseImage(&c1);

                double matching = 1.0;
                //if (contours && contours->total > 0)
                    matching = cvMatchShapes(g1, g2, CV_CONTOURS_MATCH_I1); // 작은 값 일수록 두 이미지의 형상이 가까운 (0은 같은 모양)라는 것이된다.
                cvReleaseImage(&g1);

                str.Format(_T("MatchTemplate cvMatchShapes : %.3f"), matching);
				g_cLog->AddLog(str, _LOG_LIST_SYS);

                Point2f pt2 = Point2f((float)grayTemplateImg->width, (float)grayTemplateImg->height);
                cvRectangle(graySearchImg, cvPoint(0, 0), pt2, CV_RGB(128, 128, 128), CV_FILLED); // filled rectangle.

                cvResetImageROI(graySearchImg);

                if (matching > 1.0)
                    matching = 1.0;
                dMatchShapes = (1.0-matching) * 100.0;
                if (matching <= (1.0 - dMatchShapesingRate)) // || max > 0.9)
                {
                    str.Format(_T("Template Shape Match(succ) ===> : %.2f%%"), dMatchShapes);
					g_cLog->AddLog(str, _LOG_LIST_SYS);

//                    cvReleaseImage(&graySearchImg);
//                    cvReleaseImage(&C);
//                    cvReleaseImage(&g2);
//                    cvReleaseMemStorage(&storage);

					m_DetectResult.resultType = RESULTTYPE_RECT;
					m_DetectResult.tl = CvPoint2D32f(left_top.x + srect.left, left_top.y + srect.top);
					m_DetectResult.br = CvPoint2D32f(m_DetectResult.tl.x+grayTemplateImg->width, m_DetectResult.tl.y+grayTemplateImg->height);
					m_DetectResult.result = true; // OK

                    //left_top.x += srect.left;
                    //left_top.y += srect.top;
                    //return (max*100);
                    break;
                }
                else {
                    str.Format(_T("Template Shape Match(Fail) ===> : %.2f%%"), dMatchShapes);
					g_cLog->AddLog(str, _LOG_LIST_SYS);
					break;
                }
                nLoop++;
                //if (nLoop > 10) {
                //	str.Format(("MatchShape failed"));
                //	//g_cLog->AddLog(_LOG_LIST_SYS, str);
                //	break;
                //}
            } else {
                str.Format(_T("TemplateMatch Match(succ) ===> : %.2f%%"), maxRate * 100);
				g_cLog->AddLog(str, _LOG_LIST_SYS);
				m_DetectResult.resultType = RESULTTYPE_RECT;
				m_DetectResult.tl = CvPoint2D32f(left_top.x + srect.left, left_top.y + srect.top);
				m_DetectResult.br = CvPoint2D32f(m_DetectResult.tl.x + grayTemplateImg->width, m_DetectResult.tl.y + grayTemplateImg->height);
				m_DetectResult.result = true; // OK
                break;
            }
        }
        else
        {
            nLoop++;
            //if (nLoop > 10) {
                //dMatchShapes = maxRate * 100;
                str.Format(_T("TemplateMatch Result Fail ===> : %.2f%%"), maxRate * 100);
				g_cLog->AddLog(str, _LOG_LIST_SYS);
				max = maxRate;
                m_DetectResult.result = false; // NG
                break;
            //}
        }
    }

    if(graySearchImg) cvReleaseImage(&graySearchImg);
    if(C) cvReleaseImage(&C);
    if(g2) cvReleaseImage(&g2);
    if(storage) cvReleaseMemStorage(&storage);

    return max*100.0;// -1;
}
