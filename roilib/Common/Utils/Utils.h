
extern DWORD WaitForUserMultipleObjects(DWORD nCount, CONST HANDLE *lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);

extern CStringList * GetList_Dir( CString SourceDir, CString Exe);
extern CStringList * GetList_File( CString SourceDir, CString Exe,CString Filter);
extern CStringList * GetList_File( CString SourceDir, CString Exe,CString Filter1,CString Filter2);
extern int GetList_File( CString SourceDir, CString Exe,CString Filter, CStringList * List);
extern int GetList_File_With_FileName( CString SourceDir, CString FileNamem, CString Exe,CString Filter, CStringList * List);

extern CString ExtractFileDir();

extern CString GetDateString(void);
extern CString GetDateString(CString cStr);
extern CString GetTimeString(void);
extern CString GetTimeString(CString cStr);

extern CString GetDateSingle(CString index);//index="Y""M""D"
int checkPath(const CString& strAbsFullPath, bool bCreate);
extern bool ExistDirOrFile( CString sDir,CString sSearchStr);
extern bool FileExists( CString sFile);
extern int Delete_DirOrFile(CString sFile, HWND hWnd);

extern CString GetAllTrim(CString sString);


extern CString Encord_String(CString sString);
extern CString Decord_String(CString sString);

extern void GetList_String( CString String,CString Filter,CStringList *slReturn);
extern void GetList_String( CString String,CString Filter,CString Skip1,CString Skip2,CStringList *slReturn);
extern void GetList_String( CString String,CString Filter,CString Filter2 ,CString Skip1, CString Skip2,CStringList *slReturn);

extern int Convert_StrHexaToInt(CString String,int nPort);
extern CString Convert_StrToStrHexa(CString String);

extern int ForceDirectories (CString Dir);
extern void DirectroyCopy(CString SrcFolder, CString DesFolder, HWND hWnd);
extern void DirectroyCopy(TCHAR *SrcFolder, TCHAR *DesFolder, HWND hWnd);

extern CString ChangeFileExt(CString sFile,CString sExe);
extern CString ExtractFileName(CString sFile);
extern BOOL DirectoryExists(LPCTSTR Name);

extern CString IntToStr(int iValue);
extern CString FloatToStr(double fValue);

extern int	Get_FileSize( CString FilePath );
extern TCHAR GetCharIndex( int Index ); // 0, 1, 2, ~ , 9, A, B, C, ~,
extern int  GetIntIndex( TCHAR Char );
extern CString ExtractFileName( CString sFile, CString sExe );
extern CString  BlankFillter( CString sSource );
extern __int64 GetClockCount();
extern int Rotate(double OriginX, double OriginY, double *TargetX, double *TargetY, double Angle);
extern int ProcessMessages(void);
extern int Delay( int ms );
extern int SubFolderSearchFile(LPCTSTR FolderName, LPCTSTR FileName);
extern TCHAR *getLastErrorText(int nErrorCode);
extern CString MakeUuidString(UUID* pUUID/*=NULL*/);