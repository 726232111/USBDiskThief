#pragma once

#include <windows.h>


BOOL InitLogFile();

BOOL RecordLog(DWORD LogType, TCHAR* LogInfo);

BOOL ShowLog(DWORD LogType, TCHAR* LogInfo);

