#pragma once

class CLineData
{
public:
	CLineData(void);
	~CLineData(void);

public:
	int		m_iIndex;			// Line�� index	
	CPoint	m_ptPosStart;		// Line�� X ��ǥ
	CPoint	m_ptPosEnd;			// Line�� Y ��ǥ
	double	m_dLength;			// Line�� ����
	CPoint	m_ptCenterOfLenght;	// Line ������ �߾� ��ǥ 
};
