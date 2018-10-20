#pragma once

#include "RoiBase.h"


typedef struct  _tagDetectResult
{
        int nCh;
        int ErrorID;
        CvPoint2D32f pt;
        CvPoint2D32f pt1;

        //모멘트 개수.
        int momentSize;
        //검색된 원의 반지름.
        double dRadius;
        double dAngle;
        //검색 시간.
        double dTime;
        //매칭률.
        double dMatchRate;

        CString strResult;
} DetectResult;

class CRoiData : public CRoiBase
{
public:
        CRoiData();
        virtual ~CRoiData();

public:
		std::vector<DetectResult>	m_vecDetectResult;

		int ReplaceParam(CParam param);
		BOOL IsRegWorkList(CString sName);
        CParam* getParam(CString sName);
        BOOL	LoadImageData(CString path);
        BOOL	SaveImageData(CString path);
};


