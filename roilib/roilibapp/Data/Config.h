#pragma once

#include "data/RecipeData.h"

#define DEFAULT_CAM_WIDTH	1600//1280
#define DEFAULT_CMA_HEIGHT	1200//1024

typedef struct
{
	CString strFriendlyName;
	CString		strDevPath;
} CAMINFO;

class CButtonST;
class CConfig
{
public:
	CConfig();
	CConfig(TCHAR *pszPath);
	virtual ~CConfig(void);

	CString m_sFileName;	//�������Ͽ� ���� ���
	void ReadConfig();		//�������� �ҷ����� �Լ�
	void WriteConfig();		//�������� �����ϴ� �Լ�

	int    m_Webcam[MAX_CH_NUM];
	bool	m_bSaveEngineImg;
	bool	m_bSaveGrabImg;

	double m_dRulerMaxY;
	double m_dRulerMaxX;

	CString m_Root;

	CString m_sSaveRootDir;
	CString m_sLastRecipeName;

};
extern CConfig gCfg;//�ܺ�(�ٸ� Ŭ����)���� �����ϱ� ���� ����