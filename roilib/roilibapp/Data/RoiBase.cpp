#include "stdafx.h"
#include "RoiBase.h"
#include "Utils\Utils.h"

CString  StepTypeValue[] = {
	_T("MaskingValue"),
	_T("ProcessValue1"),
	_T("ProcessValue2"),
	_T("ProcessValue3"),
	_T("FilterValue"),
	_T("DecideValue"),
	_T("PostProcessValue1"),
	_T("PostProcessValue2"),
	_T("PriorityValue(0-High)"),
	_T("LightValue"),
};


CRoiBase::CRoiBase(void)
{
	m_nCh = 0;
	m_nObjectType = 0;
	memset(m_sName, 0, sizeof(m_sName));
	m_nInspectType = (InspectType)0;
	m_RoiArea= CRect(100,100,300,300);
	sUniqueID =  MakeUuidString(NULL);

	iplTemplate = NULL;
	_tcscpy(m_sName, GetTimeString());

}
CRoiBase::~CRoiBase(void)
{
}
CRoiBase::CRoiBase(CRect nrect)
{
	m_nCh = 0;
	m_nObjectType = 0;
	memset(m_sName, 0, sizeof(m_sName));
	m_nInspectType = (InspectType)0;
	m_RoiArea.left = (double)nrect.left;
	m_RoiArea.right = (double)nrect.right;
	m_RoiArea.bottom = (double)nrect.bottom;
	m_RoiArea.top = (double)nrect.top;

	m_dCenterPos.x = m_RoiArea.left + Width()/2;
	m_dCenterPos.y = m_RoiArea.top + Height() / 2;
}

double CRoiBase::Width()
{
	return m_RoiArea.Width();
}
double 	CRoiBase::Height()
{
	return m_RoiArea.Height();
}
CRect CRoiBase::GetRect()
{
	return CRect((int)m_RoiArea.left,(int)m_RoiArea.top,(int)m_RoiArea.right,(int)m_RoiArea.bottom);
}
void CRoiBase::SetRect(CRect nrect)
{
	m_RoiArea.left = (double)nrect.left;
	m_RoiArea.right = (double)nrect.right;
	m_RoiArea.bottom = (double)nrect.bottom;
	m_RoiArea.top = (double)nrect.top;

	m_dCenterPos.x = m_RoiArea.left + Width()/2;
	m_dCenterPos.y = m_RoiArea.top + Height()/2;
}
