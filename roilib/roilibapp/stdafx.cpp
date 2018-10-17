// Copyright (C) jerry1455@gmail.com
// 모든 권리 보유.

// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// RoilibApp.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include <sys/timeb.h>

int		m_bExitFlag = 0;

time_t TimeFromSystemTime(const SYSTEMTIME * pTime)
{
    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    tm.tm_year = pTime -> wYear - 1900; // 주의 :년도는 1900년부터 시작
    tm.tm_mon = pTime -> wMonth - 1; // 주의 :월은 0부터 시작
    tm.tm_mday = pTime -> wDay;

    tm.tm_hour = pTime -> wHour;
    tm.tm_min = pTime -> wMinute;
    tm.tm_sec = pTime -> wSecond;
    tm.tm_isdst = 0;  // 썸머 타임 사용 안함

    return mktime(&tm);
}

void DoEvents(void)
{
    MSG msg;
    if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

DWORD DiffSystemTime(SYSTEMTIME* t1, SYSTEMTIME* t2)
{
    __int64 ltd, lt1, lt2;
    FILETIME ft1, ft2;
    SystemTimeToFileTime(t1, &ft1);
    SystemTimeToFileTime(t2, &ft2);

    lt1 = ft1.dwHighDateTime;
    lt1 = lt1 << 32;
    lt1 |= ft1.dwLowDateTime;
    lt1 /= 10000; // Convert from 100 nano-sec periods to milli-seconds.

    lt2 = ft2.dwHighDateTime;
    lt2 = lt2 << 32;
    lt2 |= ft2.dwLowDateTime;
    lt2 /= 10000; // Convert from 100 nano-sec periods to milli-seconds.

    ltd = lt1 - lt2;
    return (DWORD)labs((DWORD)ltd);
}

void WaitTimeMillisec(DWORD waitTimeMillisec)
{
    MSG				msg;
    SYSTEMTIME			ctime, ltime;

    GetLocalTime(&ctime);
    while (1)
    {
        if (m_bExitFlag != 0) break;
        if (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        GetLocalTime(&ltime);
        if (DiffSystemTime(&ltime, &ctime) > waitTimeMillisec) break;
    }
}

