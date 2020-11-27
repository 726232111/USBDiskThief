#include "pch.h"
#include "Log.h"



TCHAR* LogFileNma = L"USBThiefLog.txt";
TCHAR logPath[MAX_PATH+1] = { 0 };

BOOL InitLogFile()
{
	if (GetTempPath(MAX_PATH + 1, logPath))
	{
		if (!wcscat_s(logPath, MAX_PATH + 1, LogFileNma)) 
		{
			if (!PathFileExists(logPath))
			{
				HANDLE handle = (LogFileNma, GENERIC_READ| GENERIC_WRITE,NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
				if (handle == NULL)
					return FALSE;
			}
			return TRUE;
		}
	}

	return FALSE;
}

BOOL RecordLog(DWORD LogType,TCHAR* LogInfo) 
{
	


}


BOOL ShowLog(DWORD LogType, TCHAR* LogInfo)
{



}