#pragma once

// CDlgConfig dialog

class CDlgConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgConfig)

public:
	CDlgConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgConfig();

// Dialog Data
	enum { IDD = IDD_DLG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedConfigOk();
	afx_msg void OnBnClickedConfigCancel();
	BOOL m_bSaveEngineImage;
	BOOL m_bSaveGrabImage;
	BOOL m_bCamFlipVert;
	BOOL m_bCamFlipHoriz;
};
