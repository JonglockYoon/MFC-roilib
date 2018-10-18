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

        CString m_sFileName;	//설정파일에 대한 경로.
        void ReadConfig();		//설정파일 불러오는 함수.
        void WriteConfig();		//설정파일 저장하는 함수.

        int    m_Webcam[MAX_CH_NUM];
        bool	m_bSaveEngineImg;
        bool	m_bSaveGrabImg;

        double m_dRulerMaxY;
        double m_dRulerMaxX;

        CString m_Root;

        CString m_sSaveRootDir;
        CString m_sLastRecipeName;

};
extern CConfig gCfg;//외부(다른 클래스)에서 접근하기 위한 변수.
