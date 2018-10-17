#pragma once
#include "RoiData.h"

#define MAX_CH_NUM			2
#define MAX_WORKLIST_NUM	128

#include "RoiBase.h"
#include "RoiData.h"
#include "IniUtil.h"
#include "RecipeBase.h"

//#define PATTERN_MATCHING_RATE _T("Pattern matching rate")


enum  BOUNDARY_POSITION //_T("Boundary position") - �ܰ� �˻� ��ġ����
{
	_BoundaryPos_Horz,
	_BoundaryPos_Vert,
	_BoundaryPos_Round,
	_BoundaryPos_Half, // ROI ������ LEFT �� RIGHT �������� �������
	_BoundaryPos_Full,	// ROI������ �̹��� ��ü�� ������� - GRAB�̹��� ǰ���� ������ FULL�� ������ �ɰ� ����.
	_BoundaryPos_Number
};


enum  PreMode //�˻�Ÿ��
{
	PREMODE_BINARY,	//"Fixed Binary Process"
	PREMODE_ADAPTIVE_BINARY,
	PREMODE_EDGE,	//"Edge Process"
};

struct CEPData
{
	CPoint	m_ptPosStart;		// Line�� X ��ǥ
	CPoint	m_ptPosEnd;			// Line�� Y ��ǥ
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
	vector<CRoiData*>	m_vecRoiData;	// �� �𵨺� ��� ROI���� ��ϵǾ� �ִ� vector
	vector<vector<CRoiData*>>	m_vecWorkRoiData;  // �𵨺� ���� �˻��� ROI�� ��� �ִ� vector

	void InitParamData(void);
	void ClearImageBuff();
	virtual void LoadRecipeData(); //�ش� �Լ��� ȣ���ϱ����� Object������ ���� �ҷ��־�� �ȴ�.
	void UpdateOneRecipeData(CRoiData *pRoiData, int nSeq);
	virtual void SaveRecipeData();

	//int GetLastNewIndex();
	//CString GetInspName(CString sName);
	int GetObjectInspIndex(CString sName);

	CString getInspName(int nInspectType);
	int getSeq(CRoiData* pRoiData);

};

extern CRecipeData * g_cRecipeData;