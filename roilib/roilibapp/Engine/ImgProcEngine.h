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
#include "data/recipedata.h"
#include "voronoithinner.h"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "edgessubpix.h"
#include "imgprocbase.h"

#define CVX_RED		CV_RGB(255,0,0)
#define CVX_ORANGE	CV_RGB(255,165,0)
#define CVX_YELLOW	CV_RGB(255,255,0)
#define CVX_GREEN	CV_RGB(0,255,0)
#define CVX_BLUE	CV_RGB(0,0,255)
#define CVX_PINK	CV_RGB(255,0,255)
#define CVX_BLICK	CV_RGB(0,0,0)
#define CVX_WHITE	CV_RGB(255,255,255)

namespace tesseract {
	class TessBaseAPI;
}

typedef struct {
    cv::Point2d center;
    double radius;
    double cPerc;
} RANSACIRCLE;


struct SURFDetector
{
    cv::Ptr<cv::Feature2D> surf;
    SURFDetector(double hessian = 800.0)
    {
        surf = cv::xfeatures2d::SURF::create(hessian);
    }
    template<class T>
    void operator()(const T& in, const T& mask, std::vector<cv::KeyPoint>& pts, T& descriptors, bool useProvided = false)
    {
        surf->detectAndCompute(in, mask, pts, descriptors, useProvided);
    }
};
struct SIFTDetector
{
    cv::Ptr<cv::Feature2D> sift;
    SIFTDetector(int nFeatures = 0)
    {
        sift = cv::xfeatures2d::SIFT::create(nFeatures);
    }
    template<class T>
    void operator()(const T& in, const T& mask, std::vector<cv::KeyPoint>& pts, T& descriptors, bool useProvided = false)
    {
        sift->detectAndCompute(in, mask, pts, descriptors, useProvided);
    }
};
struct ORBDetector
{
    cv::Ptr<cv::Feature2D> orb;
    ORBDetector(int nFeatures = 500)
    {
        orb = cv::ORB::create(nFeatures);
    }
    template<class T>
    void operator()(const T& in, const T& mask, std::vector<cv::KeyPoint>& pts, T& descriptors, bool useProvided = false)
    {
        orb->detectAndCompute(in, mask, pts, descriptors, useProvided);
    }
};
class CImgProcEngine : public CImgProcBase
{
public:
	CWnd* m_pMainWnd;

	CString m_sDebugPath;

	DetectResult m_DetectResult;

protected:
	double TemplateMatch(CRoiData *pData, IplImage* graySearchImgIn, IplImage* grayTemplateImg, CvPoint &left_top, double &dMatchShapes);
	int Threshold(CRoiData *pData, IplImage* grayImg, int nDbg = 100);
	double ThresholdOTSU(CRoiData *pData, IplImage* grayImg, int nDbg = 100);
	int NoiseOut(CRoiData *pData, IplImage* grayImg, int t = -1, int nDbg = 100, int h = -1);
	int Expansion(CRoiData *pData, IplImage* grayImg, int t = -1, int nDbg = 150, int h = -1);
	int ThresholdRange(CRoiData *pData, IplImage* grayImg, int nDbg = 100);
	void Smooth(CRoiData *pData, IplImage* ImgIn, int iImgID);

public:
	CImgProcEngine();
	~CImgProcEngine();

    cv::Mat drawGoodMatches(
        const cv::Mat& img1,
        const cv::Mat& img2,
        const std::vector<cv::KeyPoint>& keypoints1,
        const std::vector<cv::KeyPoint>& keypoints2,
        std::vector<cv::DMatch>& matches,
        std::vector<cv::Point2f>& scene_corners_
    );
	
	int FindCircle(CRoiData *pData, IplImage* graySearchImgIn);
	void SaveOutImage(IplImage* pImgOut, int nCh, CString strMsg, BOOL bClear = FALSE);

	void InitEngine(int nCh);
	int InspectOneItem(int nCh, IplImage* img, CRoiData *pData);

    int SinglePattIdentify(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);
    int SinglePattMatchShapes(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);
    int SinglePattFeatureMatch(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);
    int SingleROIFindShape(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);
	int SingleROICircle(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);
	int SingleROIBarCode(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);
	int SingleROIOCR(int nCh, IplImage* croppedImage, CRoiData *pData, CRect rect);

};

