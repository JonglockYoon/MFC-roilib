// DlgConfig.cpp : implementation file
//

#include "stdafx.h"
#include "RoilibApp.h"
#include "DlgConfig.h"

// CDlgConfig dialog

IMPLEMENT_DYNAMIC(CDlgConfig, CDialog)

CDlgConfig::CDlgConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConfig::IDD, pParent)
{
}

CDlgConfig::~CDlgConfig()
{
}

void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ENGINE_IMAGE, m_bSaveEngineImage);
	DDX_Check(pDX, IDC_CHECK_GRAB_IMAGE, m_bSaveGrabImage);
	DDX_Check(pDX, IDC_CHECK_CAM_FLIP_HORIZ, m_bCamFlipHoriz);
	DDX_Check(pDX, IDC_CHECK_CAM_FLIP_VERT, m_bCamFlipVert);
}


BEGIN_MESSAGE_MAP(CDlgConfig, CDialog)
	ON_BN_CLICKED(IDC_CONFIG_OK, &CDlgConfig::OnBnClickedConfigOk)
	ON_BN_CLICKED(IDC_CONFIG_CANCEL, &CDlgConfig::OnBnClickedConfigCancel)
END_MESSAGE_MAP()


// CDlgConfig message handlers

BOOL CDlgConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bSaveEngineImage = gCfg.m_bSaveEngineImg;
	m_bSaveGrabImage = gCfg.m_bSaveGrabImg;
	m_bCamFlipHoriz = gCfg.m_bCamFlipHoriz;
	m_bCamFlipVert = gCfg.m_bCamFlipVert;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfig::OnBnClickedConfigOk()
{
	CString str;

	UpdateData(TRUE);
	gCfg.m_bSaveEngineImg = m_bSaveEngineImage;
	gCfg.m_bSaveGrabImg = m_bSaveGrabImage;
	gCfg.m_bCamFlipHoriz = m_bCamFlipHoriz;
	gCfg.m_bCamFlipVert = m_bCamFlipVert;

	gCfg.WriteConfig();

	CDialog::OnOK();
}

void CDlgConfig::OnBnClickedConfigCancel()
{
	CDialog::OnCancel();
}
