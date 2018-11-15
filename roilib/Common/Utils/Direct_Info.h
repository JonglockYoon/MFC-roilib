#pragma once

class CDirect_Info
{
public:
    CDirect_Info(void);
    ~CDirect_Info(void);

    CString Get_Root_Path();//실행파일이 위치한 경로를 구하기 위한 함수.
    void Create_Directory(CString dir);//디렉토리 생성.
    bool CreateOrNoTuncate_File(CString file_name);//비어있는 텍스트파일 생성.
    bool Search_FileName(CString file_name);//같은파일 이름있는지 여부 확인 함수.

    CString Get_Day(bool only_number);
    CString Get_Time(bool only_number);

    int Delete_DirOrFile(CString sFile, HWND hWnd);
    void DirectroyCopy(CString SrcFolder, CString DesFolder, HWND hWnd);
    void DirectroyCopy(TCHAR *SrcFolder, TCHAR *DesFolder, HWND hWnd);

    void SetPath(CString path);

};
extern CDirect_Info * g_pDirectory;
