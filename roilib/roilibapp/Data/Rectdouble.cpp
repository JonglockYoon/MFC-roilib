#include "StdAfx.h"
#include "Rectdouble.h"


CRectdouble::CRectdouble(){
}

CRectdouble::CRectdouble(CRect nrect)
{
	left = (double)nrect.left;
	right = (double)nrect.right;
	bottom = (double)nrect.bottom;
	top = (double)nrect.top;
}

double CRectdouble::Width()
{
	return fabs(right - left);
}

double CRectdouble::Height()
{
	return fabs(bottom - top);
}

CRect CRectdouble::GetRect()
{
	return CRect((int)left, (int)top, (int)right, (int)bottom);
}

void CRectdouble::SetRect(CRect nrect)
{
	left = (double)nrect.left;
	right = (double)nrect.right;
	bottom = (double)nrect.bottom;
	top = (double)nrect.top;
}

BOOL CRectdouble::InterRect(double dX, double dY, int offset/*=0*/)
{
	CRect rect = GetRect();

	rect.left += offset;
	rect.right -= offset;
	rect.top += offset;
	rect.bottom -= offset;

	if ((rect.left < dX && rect.right > dX)
		&& (rect.top < dY && rect.bottom > dY)) return TRUE;
	return FALSE;
}

BOOL CRectdouble::CornerPoint(double dX, double dY)
{
	//LeftTop Corner
	if (left == dX && top == dY) return TRUE;
	//RightTop Corner
	if (right == dX && top == dY) return TRUE;
	//LeftBottom Corner
	if (left == dX && bottom == dY) return TRUE;
	//RightBottom Corner
	if (right == dX && bottom == dY) return TRUE;
	return FALSE;
}