#pragma once

#include "RoiBase.h"


#define RESULTTYPE_POINT 0x0001
#define RESULTTYPE_RECT  0x0002
#define RESULTTYPE_RECT4P  0x0004
#define RESULTTYPE_POLYGON  0x0008
#define RESULTTYPE_RADIUS  0x0010
#define RESULTTYPE_ANGLE  0x0020
#define RESULTTYPE_MATCHRATE  0x0040
#define RESULTTYPE_STRING  0x0080
#define RESULTTYPE_BOOL  0x0100
#define RESULTTYPE_IMAGE  0x0200
#define RESULTTYPE_LENGTH  0x0400
#define RESULTTYPE_WIDTH  0x0800
#define RESULTTYPE_HEIGHT  0x1000

typedef struct  _tagDetectResult
{
	int resultType; // point, rect, rect4p, ...

	CvPoint2D32f pt; // point
	CvPoint2D32f tl; // rect, rect4p
	CvPoint2D32f tr; // rect4p
	CvPoint2D32f bl; // rect4p
	CvPoint2D32f br; // rect, rect4p
	bool	result; // bool
	std::vector<CvPoint2D32f> vpolygon;

	double dRadius;
	double dAngle;
	double dMatchRate;
	double dValue;
	double dWidth;
	double dHeight;
	std::string str;
	IplImage* img;

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
        CParam* getParam(CString sName, int t = -1);
        BOOL	LoadImageData(CString path);
        BOOL	SaveImageData(CString path);
};


