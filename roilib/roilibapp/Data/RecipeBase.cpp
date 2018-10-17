#include "stdafx.h"
#include "RecipeBase.h"
#include "IniUtil.h"
#include "Utils/Direct_Info.h"
#include "MainFrm.h"

CRecipeBase::CRecipeBase(void)
{
	m_sFileName = "";
}

CRecipeBase::~CRecipeBase(void)
{

}

void CRecipeBase::LoadRecipeData()
{
	CIniUtil* ini = new CIniUtil(m_sFileName);
	CString str;
	str.Format(_T("RECIPE_INFO"));

	delete ini;
}

void CRecipeBase::SaveRecipeData()
{
	CIniUtil* ini = new CIniUtil(m_sFileName);
	CString str;
	str.Format(_T("RECIPE_INFO"));

	ini->WriteString(str, _T("RecipeName"), gCfg.m_sLastRecipeName);

	delete ini;
}


void CRecipeBase::ReadRecipeList()
{
	CString sFileName;
	sFileName.Format(_T("%s\\RecipeList.ini"), m_sPathRecipeRoot);
	CIniUtil* ini = new CIniUtil(sFileName);
	CString str;
	CString dumy;
	int nNum;
	extern CDirect_Info * g_pDirectory;

	RecipeListInformation tRcpData;

	theApp.m_vecRecipeInfo.clear();

	nNum = ini->ReadInt(_T("RecipeList"), _T("Recipe_Number"), 0);
	for(int i=0; i<nNum; i++){
		dumy.Format(_T("Recipe_%02d"), i);

		tRcpData.sRecipeName = ini->ReadString(dumy, _T("Name"), _T(""));

		if(tRcpData.sRecipeName.GetLength() > 0){
			str.Format(_T("%s\\%s"), m_sPathRecipeRoot, tRcpData.sRecipeName);
			if(g_pDirectory->Search_FileName(str)){
				theApp.m_vecRecipeInfo.push_back(tRcpData);
			}
		}
	}

	delete ini;
}
void CRecipeBase::WriteRecipeList()
{
	CString sFileName;
	sFileName.Format(_T("%s\\RecipeList.ini"), m_sPathRecipeRoot);
	CIniUtil* ini = new CIniUtil(sFileName);

	CString str;
	CString dumy;

	ini->WriteInt(_T("RecipeList"), _T("Recipe_Number"), (int)theApp.m_vecRecipeInfo.size());
	for(int i=0; i<(int)theApp.m_vecRecipeInfo.size(); i++){
		dumy.Format(_T("Recipe_%02d"), i);
		ini->WriteString(dumy, _T("Name"), theApp.m_vecRecipeInfo[i].sRecipeName);
	}

	delete ini;
}
