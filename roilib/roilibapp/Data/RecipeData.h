#pragma once
#include "RoiData.h"

#define MAX_CH_NUM			2
#define MAX_WORKLIST_NUM	128

#include "RoiBase.h"
#include "RoiData.h"
#include "IniUtil.h"
#include "RecipeBase.h"

//#define PATTERN_MATCHING_RATE _T("Pattern matching rate")


enum  BOUNDARY_POSITION //_T("Boundary position") - 외곽 검사 위치지정
{
	_BoundaryPos_Horz,
	_BoundaryPos_Vert,
	_BoundaryPos_Round,
	_BoundaryPos_Half, // ROI 영역을 LEFT 및 RIGHT 반쪽으로 잡았을때
	_BoundaryPos_Full,	// ROI영역을 이미지 전체로 잡았을때 - GRAB이미지 품질이 좋으면 FULL로 잡으면 될것 같다.
	_BoundaryPos_Number
};


enum  PreMode //검사타입
{
	PREMODE_BINARY,	//"Fixed Binary Process"
	PREMODE_ADAPTIVE_BINARY,
	PREMODE_EDGE,	//"Edge Process"
};

struct CEPData
{
	CPoint	m_ptPosStart;		// Line의 X 좌표
	CPoint	m_ptPosEnd;			// Line의 Y 좌표
};

typedef struct _tagParamTable {
	InspectType nInspectType;
	CParam param;
} ParamTable;


struct ErasePatternInformation {
	int nCellSize;
	vector<CEPData> m_LineList[64][64]; // X,Y Cell
};

class CRecipeData : public CRecipeBase
{
public:
	CRecipeData(void);
	virtual ~CRecipeData(void);

	SpecInfo		m_tGeneralSpec[_NG_NUMBER];

	CString	m_sInspList[_Inspect_Type_End]; // ROI Type Table
	vector<CRoiData*>	m_vecRoiData;	// 각 모델별 모든 ROI들이 등록되어 있는 vector
	vector<vector<CRoiData*>>	m_vecWorkRoiData;  // 모델별 실제 검사할 ROI만 들어 있는 vector

	void InitParamData(void);
	void ClearImageBuff();
	virtual void LoadRecipeData(); //해당 함수를 호출하기전에 Object파일을 먼저 불러주어야 된다.
	void UpdateOneRecipeData(CRoiData *pRoiData, int nSeq);
	virtual void SaveRecipeData();

	//int GetLastNewIndex();
	//CString GetInspName(CString sName);
	int GetObjectInspIndex(CString sName);

	CString getInspName(int nInspectType);
	int getSeq(CRoiData* pRoiData);

};

extern CRecipeData * g_cRecipeData;