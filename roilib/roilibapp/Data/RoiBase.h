#pragma once

class CRectdouble;

// 여기에 정의된 순서를 변경하면 모든 레시피 파일에 영향을 주므로.
// 순서변경시에는 레시피 ini 파일의 nInspectType도 같이 변경해야한다.
typedef enum _tagInspectType
{
        _Inspect_Patt_Start = 0,
		_Inspect_Patt_Identify,
		_Inspect_Patt_MatchShapes,
		_Inspect_Patt_FeatureMatch,
		_Inspect_Patt_End,		// 패턴마지막.

        _Inspect_Roi_Start = 100,
        _Inspect_Roi_Circle,
        _Inspect_BarCode,
		_Inspect_Teseract,
        _Inspect_Roi_End,			// 영역 마지막.

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

struct	DOUBLE_POINT
{
        double x;
        double y;
        DOUBLE_POINT()
		{
                x = 0;
                y = 0;
        };
};

enum  ValueType
{
        _BoolValue,
        _IntValue,
        _DoubleValue,
        _StringValue,
        _ComboValue,
};

enum  StepType
{
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
        enum StepType		stepType;	// 마스크, 전처리, Process, 후처리,...
        std::basic_string<TCHAR>	Name;
        enum ValueType		valueType;	// _BoolValue, ...
        std::basic_string<TCHAR>	Value;
        std::basic_string<TCHAR>	Detail; // Combo일경우 Combo 항목들을 콤마(Comma)로 구분해서 넣어둔다.
		std::vector<std::basic_string<TCHAR>> m_vecDetail;

public:
        CParam(void){};
        ~CParam(void){};
        CParam(enum StepType nStep, CString sName, enum ValueType nType, CString sVal, CString sDetail = _T(""))
        {
                stepType = nStep;
                Name = sName.GetString();
                valueType = nType;
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
        int m_nCh;	// ROI가 속해있는 Channel - 보통 카메라로 구분된다.//
        int m_nObjectType; // 패턴, 영역, 포인터 구분; ObjectTypeList 참조.//
        TCHAR m_sName[512]; // ROI이름.//
        InspectType m_nInspectType; // Inspection Type, RecipeData.h의 InspectType 참조.//
        DOUBLE_POINT m_dCenterPos;	// Point 중심점 또는 Rect의 중심점.//
        CRectdouble m_RoiArea; // ROI의 전체 영역을 나타낸다.//
        CRectdouble ptnRoi;
        IplImage* iplTemplate;

		std::vector<CParam> m_vecParams;	// stepType, Name, valueType, Value, Detail 를 담은 vector

        double Width();
        double Height();
        CRect GetRect();
        void SetRect(CRect nrect);
};
