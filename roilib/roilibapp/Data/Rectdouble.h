#pragma once
#include <math.h>

class CRectdouble
{
public:
	CRectdouble();
	CRectdouble(CRect nrect);

	double left;
	double right;
	double bottom;
	double top;
	double Width();
	double Height();
	CRect GetRect();
	void SetRect(CRect nrect);
	BOOL InterRect(double dX, double dY, int offset = 0);
	BOOL CornerPoint(double dX, double dY);
};