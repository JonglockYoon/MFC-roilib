#pragma once

#define COMMAND_MSG_LMOUSE_DOWN		10000
#define COMMAND_MSG_LMOUSE_UP		10100

// CNaButton

class CNaButton : public CMFCButton
{
	DECLARE_DYNAMIC(CNaButton)

public:
	CNaButton();
	virtual ~CNaButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


