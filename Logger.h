#pragma once
#include  <windows.h>
#include <tchar.h>
#include  <Shlwapi.h>
#pragma comment(lib,"Shlwapi")

//使用者给定路径+文件名
BOOL InitLoggerByPath(LPCWCH pLogPath);
//默认路径为当前用户临时目录
BOOL InitLoggerByFile(LPCWCH pLogFileNma);
//记录日志
BOOL RecordLog(const TCHAR* LogInfo);
BOOL RecordTimeAndLog(const TCHAR* LogInfo);
//获取文件路径
BOOL GetLogFilePath(TCHAR* LogPath, DWORD SizeOfPath);
//关闭记录器
BOOL CloseLogger();