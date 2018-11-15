#pragma once

class CLineData
{
public:
    CLineData(void);
    ~CLineData(void);

public:
    int		m_iIndex;			// Line의 index
    CPoint	m_ptPosStart;		// Line의 X 좌표.
    CPoint	m_ptPosEnd;			// Line의 Y 좌표.
    double	m_dLength;			// Line의 길이.
    CPoint	m_ptCenterOfLenght;	// Line 길이의 중앙 자표.
};
