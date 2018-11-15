#pragma once

#include "Rectdouble.h"
#include "RoiBase.h"
#include "RoiData.h"
#include "IniUtil.h"
#include "RecipeBase.h"

#define MAX_CH_NUM			2
#define MAX_WORKLIST_NUM	128

typedef struct _tagParamTable 
{
        InspectType nInspectType;
        CParam param;
} ParamTable;

class CRecipeData : public CRecipeBase
{
public:
        CRecipeData(void);
        virtual ~CRecipeData(void);

        CString	m_sInspList[_Inspect_Type_End]; // ROI Type Table
		std::vector<CRoiData*>	m_vecRoiData;	// 각 모델별 모든 ROI들이 등록되어 있는 vector
		std::vector<std::vector<CRoiData*>>	m_vecWorkRoiData;  // 모델별 실제 검사할 ROI만 들어 있는 vector

        void InitParamData(void);
        void ClearImageBuff();
        virtual void LoadRecipeData(); //해당 함수를 호출하기전에 Object파일을 먼저 불러주어야 된다.
        void UpdateOneRecipeData(CRoiData *pRoiData, int nSeq);
        virtual void SaveRecipeData();

        int GetObjectInspIndex(CString sName);

        CString getInspName(int nInspectType);
        int getSeq(CRoiData* pRoiData);

};

extern CRecipeData * g_cRecipeData;
