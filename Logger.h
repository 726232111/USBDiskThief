#pragma once
#include  <windows.h>
#include <tchar.h>
#include  <Shlwapi.h>
#pragma comment(lib,"Shlwapi")

//ʹ���߸���·��+�ļ���
BOOL InitLoggerByPath(LPCWCH pLogPath);
//Ĭ��·��Ϊ��ǰ�û���ʱĿ¼
BOOL InitLoggerByFile(LPCWCH pLogFileNma);
//��¼��־
BOOL RecordLog(const TCHAR* LogInfo);
BOOL RecordTimeAndLog(const TCHAR* LogInfo);
//��ȡ�ļ�·��
BOOL GetLogFilePath(TCHAR* LogPath, DWORD SizeOfPath);
//�رռ�¼��
BOOL CloseLogger();