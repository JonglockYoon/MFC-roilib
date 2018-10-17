#pragma once

#include "RoiBase.h"


typedef struct  _tagDetectResult
{
	int nCh;			// 0 ~ 15
	//Result Error ID
	int ErrorID;
	CvPoint2D32f pt;
	CvPoint2D32f pt1;

	//���Ʈ ����
	int momentSize;
	//�˻��� ���� ������
	double dRadius;
	double dAngle;
	//�˻� �ð�
	double dTime;
	//��Ī��
	double dMatchRate;
} DetectResult;

enum NG_TYPE
{
	_NG_ALIGN,
	_NG_NUMBER
};

typedef struct _SpecInfomation
{
	BOOL	bEnable;

	_SpecInfomation()
	{
		bEnable = FALSE;
	};

}SpecInfo;


class CRoiData : public CRoiBase
{
public:
	CRoiData();
	virtual ~CRoiData();

public:
	SpecInfo	m_tSpec[_NG_NUMBER];
	vector<DetectResult>	m_vecDetectResult;

	int ReplaceParam(CParam param);
	int AddParam(CParam param);
	BOOL IsRegWorkList(CString sName);
	CParam* getParam(int nId);
	CParam* getParam(CString sName);

	int nParam(int nId)
	{
		int n = 0;
		CParam* pParam = getParam(nId);
		if (pParam)	n = _ttoi(pParam->Value.c_str());
		return n;
	}
	float fParam(int nId)
	{
		float n = 0;
		CParam* pParam = getParam(nId);
		if (pParam)	n = _ttof(pParam->Value.c_str());
		return n;
	}
	BOOL	LoadImageData(CString path);
	BOOL	SaveImageData(CString path);
};


