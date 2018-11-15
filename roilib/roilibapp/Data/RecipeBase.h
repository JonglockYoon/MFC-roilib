#pragma once

struct RecipeListInformation
{
	CString sRecipeName;
};

class CRecipeBase
{
public:
	CRecipeBase(void);
	virtual ~CRecipeBase(void);
	CString m_sPathRecipeRoot;
	virtual void ReadRecipeList();
	virtual void WriteRecipeList();

	CString m_sFileName;
	virtual void LoadRecipeData();
	virtual void SaveRecipeData();

};
