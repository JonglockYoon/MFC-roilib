#pragma once

#include "Rectdouble.h"


// ���⿡ ���ǵ� ������ �����ϸ� ��� ������ ���Ͽ� ������ �ֹǷ�
// ��������ÿ��� ������ ini ������ nInspectType�� ���� �����ؾ��Ѵ�.
typedef enum _tagInspectType
{
	_Inspect_Patt_Start = 0,
	_Inspect_Patt_End,		// ���ϸ�����

	_Inspect_Roi_Start = 100,
	_Inspect_Roi_Circle,
	_Inspect_BarCode,
	_Inspect_Roi_End,			// ���� ������

	_Inspect_Point_Start = 200,
	_Inspect_Point_End,

	_Inspect_Type_End,


} InspectType;

typedef struct  _Point2i
{
	int x, y;
} Point2i;

typedef struct  _Size2i
{
	int width, height;
} Size2i;

typedef struct  _Size2f
{
	float width, height;
} Size2f;

typedef struct  _RectI
{
	int left, top, right, bottom;
} RectI;

typedef struct  _RectF
{
	float left, top, right, bottom;
} RectF;

enum ObjectTypeList
{
	_ObjType_Patt,
	_ObjType_Roi,
	_ObjType_Point,
	_ObjType_Line,
	_ObjType_Number
};

struct	DOUBLE_POINT{
	double x;
	double y;
	DOUBLE_POINT(){
		x = 0;
		y = 0;
	};
};

enum  ValueType{
	_BoolValue,
	_IntValue,
	_DoubleValue,
	_StringValue,
	_ComboValue,
};

enum  StepType{
	_MaskingValue,
	_ProcessValue1,
	_ProcessValue2,
	_ProcessValue3,
	_FilterValue,
	_PostProcessValue1,
	_PostProcessValue2,
	_DecideValue,
	_PriorityValue,
	_LightValue,
};
extern CString  StepTypeValue[];

class  CParam
{
public:
	enum StepType		stepType;	// ����ũ, ��ó��, Process, ��ó��,...
	int id;	// stepType������ unique�� id
	std::basic_string<TCHAR>	Name;
	enum ValueType		valueType;	// _BoolValue, ...
	std::basic_string<TCHAR>	Value;
	std::basic_string<TCHAR>	Detail; // Combo�ϰ�� Combo �׸���� �޸�(Comma)�� �����ؼ� �־�д�.
	vector<std::basic_string<TCHAR>> m_vecDetail;

public:
	CParam(void){};
	~CParam(void){};
	CParam(enum StepType nStep, const int nId, CString sName, enum ValueType nType, CString sVal, CString sDetail = _T(""))
	{
		//TCHAR buff[64];
		//_stprintf(buff, _T("%.3f"), dVal);

		stepType = nStep;
		id = nId;
		Name = sName.GetString();
		valueType = nType;
		//std::ostringstream strs;
		//strs << dVal;
		//std::string str = strs.str();
		Value = sVal.GetString();
		Detail = sDetail.GetString();
	};
};


class CRoiBase
{
public:
	CRoiBase();
	CRoiBase(CRect nrect);
	virtual ~CRoiBase();
	
	CString sUniqueID;
	int m_nCh;	// ROI�� �����ִ� Channel - ���� ī�޶�� ���еȴ�.
	int m_nObjectType; // ����, ����, ������ ����; ObjectTypeList ����
	TCHAR m_sName[512]; // ROI�̸�
	InspectType m_nInspectType; // Inspection Type, RecipeData.h�� InspectType ����
	DOUBLE_POINT m_dCenterPos;	// Point �߽��� �Ǵ� Rect�� �߽���
	CRectdouble	m_RoiArea; // ROI�� ��ü ������ ��Ÿ����.
	CRectdouble ptnRoi;
	IplImage*	iplTemplate;

	vector<CParam> m_vecParams;	// stepType, Name, valueType, Value, Detail �� ���� vector

	double Width();
	double Height();
	CRect GetRect();
	void SetRect(CRect nrect);
};