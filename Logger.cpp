#include "pch.h"
#include "Logger.h"


TCHAR paLogPath[MAX_PATH + 1] = { 0 };    //不让用户操作,修改了以后每次获取的路径都是有问题的
BOOL bInitFlag = FALSE;    //初始化标识
HANDLE hLogFile = INVALID_HANDLE_VALUE;    //文件句柄


//默认路径为当前用户临时目录
BOOL InitLoggerByFile(LPCWCH pLogFileNma)
{
	DWORD ErrorCode = 0;

	if (pLogFileNma == NULL || wcslen(pLogFileNma) == 0)    //判断文件名
		return FALSE;

	if (bInitFlag)    //不重复初始化
		return TRUE;


	if (GetTempPath(MAX_PATH + 1, paLogPath))    //获取用户临时目录
	{
		if (!wcscat_s(paLogPath, MAX_PATH + 1, pLogFileNma))    //拼接路径
		{
			if (PathFileExists(paLogPath))    //判断文件是否存在 -> 使用的是系统路径,默认是存在的
				//存在 -> 打开文件,进行追加
				hLogFile = CreateFile(paLogPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			else
			{
				//其他情况
				ErrorCode = GetLastError();
				if (ErrorCode == ERROR_FILE_NOT_FOUND)    //文件未找到
					//创建文件
					hLogFile = CreateFile(paLogPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			if (hLogFile != INVALID_HANDLE_VALUE) {//打开文件句柄成功
				SetFilePointer(hLogFile, NULL, NULL, FILE_END);    //设置偏移量 到文件尾部
				bInitFlag = TRUE;
			}
		}
	}

	return bInitFlag;
}



//使用者给定路径+文件名
BOOL InitLoggerByPath(LPCWCH pLogPath)
{
	DWORD ErrorCode = 0;

	if (pLogPath == NULL || wcslen(pLogPath) == 0) //判断路径 
		return FALSE;


	if (bInitFlag)    //不重复初始化
		return TRUE;


	if (PathFileExists(pLogPath))    //判断文件是否存在 -> 使用的是系统路径,默认是存在的
		//存在 -> 打开文件,进行追加
		hLogFile = CreateFile(pLogPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
	{
		//其他情况
		ErrorCode = GetLastError();
		if (ErrorCode == ERROR_FILE_NOT_FOUND)    //文件未找到
			//创建文件
			hLogFile = CreateFile(pLogPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (hLogFile != INVALID_HANDLE_VALUE) {//打开文件句柄成功
		SetFilePointer(hLogFile, NULL, NULL, FILE_END);    //设置偏移量 到文件尾部
		wcscpy_s(paLogPath, MAX_PATH + 1, pLogPath);    //备份路径
		bInitFlag = TRUE;
	}

	return bInitFlag;
}



//获取本地系统时间
BOOL GetLocalSystemTime(TCHAR* CurrentTime,DWORD SizeOfBuffer)
{
	SYSTEMTIME localtime;

	GetLocalTime(&localtime);

	if (
		swprintf_s(CurrentTime, SizeOfBuffer, L"%04d:%02d:%02d:%02d:%02d:%02d  ", localtime.wYear, localtime.wMonth, localtime.wDay, localtime.wHour,
			localtime.wMinute, localtime.wSecond))
		return TRUE;
	ZeroMemory(CurrentTime, SizeOfBuffer);
	return FALSE;

}


struct CustomerUTF8
{
	void* utf8char;
	DWORD sizeOfChar;
};

//UTF16转换为UTF8,返回指针,需要通过ClearUTF8释放内存
BOOL UTF16ToUTF8(const TCHAR* WideChar,DWORD SizeOfWideChar, CustomerUTF8* customerUTF8)
{
	//获取转换字符数
	int SizeOfBuffer = WideCharToMultiByte(CP_UTF8,0,WideChar,SizeOfWideChar,NULL,0,NULL,NULL);
	//申请空间
	LPSTR lpMultiByteStr = (LPSTR)	malloc(SizeOfBuffer);
	ZeroMemory(lpMultiByteStr, SizeOfBuffer);
	//转换
	int iRet = WideCharToMultiByte(CP_UTF8, 0, WideChar, SizeOfWideChar, lpMultiByteStr, SizeOfBuffer, NULL, NULL);

	if (iRet > 0 && iRet <= SizeOfBuffer) 
	{
		customerUTF8->sizeOfChar = SizeOfBuffer;
		customerUTF8->utf8char = lpMultiByteStr;
		return TRUE;
	}
		
		
	customerUTF8->utf8char = NULL;
	customerUTF8->sizeOfChar = 0;
	free(lpMultiByteStr);
	return FALSE;
}

void ClearUTF8(CustomerUTF8* customerUTF8)
{
	free(customerUTF8->utf8char);
	customerUTF8->utf8char = NULL;
	customerUTF8->sizeOfChar = 0;

}


//记录信息
BOOL RecordLog(const TCHAR* LogInfo)
{
	if (!bInitFlag)    //未初始化
		return FALSE;

	DWORD nNumberOfBytesToWrite = wcslen(LogInfo) * 2;    //预计写入字节数 TCHAR为双字节要*2
	DWORD nNumberOfBytesWritten = 0;    //实际写入字节数
	
	//写入
	if (!WriteFile(hLogFile, LogInfo, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL))
		return FALSE;

	//换行
	if (!WriteFile(hLogFile, L"\n", 2, &nNumberOfBytesWritten, NULL))
		return FALSE;


	//刷新文件缓冲区
	FlushFileBuffers(hLogFile);

	return TRUE;
}





//记录信息
BOOL RecordTimeAndLog(const TCHAR* LogInfo)
{
	CustomerUTF8 customerUTF8 = { 0 };

	if (!bInitFlag)    //未初始化
		return FALSE;



	TCHAR localSystemTime[40] = {0};
	//获取时间
	if (!GetLocalSystemTime(localSystemTime, 40))
		return FALSE;


	DWORD nNumberOfBytesWritten = 0;    //实际写入字节数

	if (UTF16ToUTF8(localSystemTime, wcslen(localSystemTime), &customerUTF8))
	{

		//写入时间
		if (!WriteFile(hLogFile, customerUTF8.utf8char, customerUTF8.sizeOfChar, &nNumberOfBytesWritten, NULL))
		{
			ClearUTF8(&customerUTF8);
			return FALSE;
		}
			

		ClearUTF8(&customerUTF8);


	}

	if (UTF16ToUTF8(LogInfo, wcslen(LogInfo), &customerUTF8))
	{

		//写入日志
		if (!WriteFile(hLogFile, customerUTF8.utf8char, customerUTF8.sizeOfChar, &nNumberOfBytesWritten, NULL))
		{
			ClearUTF8(&customerUTF8);
			return FALSE;
		}
			
		ClearUTF8(&customerUTF8);
	}


	//换行
	if (!WriteFile(hLogFile, L"\n", 2, &nNumberOfBytesWritten, NULL))
		return FALSE;

	//刷新文件缓冲区
	FlushFileBuffers(hLogFile);

	return TRUE;
}


//获取路径名
BOOL GetLogFilePath(TCHAR* LogPath, DWORD SizeOfPath)
{
	if (!bInitFlag)    //未初始化
		return FALSE;

	if (wcscpy_s(LogPath, SizeOfPath, paLogPath))
	{
		ZeroMemory(LogPath, SizeOfPath);
		return FALSE;
	}


	return TRUE;
}

//关闭记录器
BOOL CloseLogger()
{
	if (!bInitFlag)    //未初始化,何来关闭
		return TRUE;


	if (CloseHandle(hLogFile))//关闭句柄
	{
		//赋予初值
		ZeroMemory(paLogPath, MAX_PATH + 1);
		bInitFlag = FALSE;    //初始化标识
		hLogFile = INVALID_HANDLE_VALUE;    //文件句柄
		return TRUE;
	}

	return FALSE;
}