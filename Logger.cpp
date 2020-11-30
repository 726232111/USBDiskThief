#include "pch.h"
#include "Logger.h"


TCHAR paLogPath[MAX_PATH + 1] = { 0 };    //�����û�����,�޸����Ժ�ÿ�λ�ȡ��·�������������
BOOL bInitFlag = FALSE;    //��ʼ����ʶ
HANDLE hLogFile = INVALID_HANDLE_VALUE;    //�ļ����


//Ĭ��·��Ϊ��ǰ�û���ʱĿ¼
BOOL InitLoggerByFile(LPCWCH pLogFileNma)
{
	DWORD ErrorCode = 0;

	if (pLogFileNma == NULL || wcslen(pLogFileNma) == 0)    //�ж��ļ���
		return FALSE;

	if (bInitFlag)    //���ظ���ʼ��
		return TRUE;


	if (GetTempPath(MAX_PATH + 1, paLogPath))    //��ȡ�û���ʱĿ¼
	{
		if (!wcscat_s(paLogPath, MAX_PATH + 1, pLogFileNma))    //ƴ��·��
		{
			if (PathFileExists(paLogPath))    //�ж��ļ��Ƿ���� -> ʹ�õ���ϵͳ·��,Ĭ���Ǵ��ڵ�
				//���� -> ���ļ�,����׷��
				hLogFile = CreateFile(paLogPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			else
			{
				//�������
				ErrorCode = GetLastError();
				if (ErrorCode == ERROR_FILE_NOT_FOUND)    //�ļ�δ�ҵ�
					//�����ļ�
					hLogFile = CreateFile(paLogPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			if (hLogFile != INVALID_HANDLE_VALUE) {//���ļ�����ɹ�
				SetFilePointer(hLogFile, NULL, NULL, FILE_END);    //����ƫ���� ���ļ�β��
				bInitFlag = TRUE;
			}
		}
	}

	return bInitFlag;
}



//ʹ���߸���·��+�ļ���
BOOL InitLoggerByPath(LPCWCH pLogPath)
{
	DWORD ErrorCode = 0;

	if (pLogPath == NULL || wcslen(pLogPath) == 0) //�ж�·�� 
		return FALSE;


	if (bInitFlag)    //���ظ���ʼ��
		return TRUE;


	if (PathFileExists(pLogPath))    //�ж��ļ��Ƿ���� -> ʹ�õ���ϵͳ·��,Ĭ���Ǵ��ڵ�
		//���� -> ���ļ�,����׷��
		hLogFile = CreateFile(pLogPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
	{
		//�������
		ErrorCode = GetLastError();
		if (ErrorCode == ERROR_FILE_NOT_FOUND)    //�ļ�δ�ҵ�
			//�����ļ�
			hLogFile = CreateFile(pLogPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (hLogFile != INVALID_HANDLE_VALUE) {//���ļ�����ɹ�
		SetFilePointer(hLogFile, NULL, NULL, FILE_END);    //����ƫ���� ���ļ�β��
		wcscpy_s(paLogPath, MAX_PATH + 1, pLogPath);    //����·��
		bInitFlag = TRUE;
	}

	return bInitFlag;
}



//��ȡ����ϵͳʱ��
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

//UTF16ת��ΪUTF8,����ָ��,��Ҫͨ��ClearUTF8�ͷ��ڴ�
BOOL UTF16ToUTF8(const TCHAR* WideChar,DWORD SizeOfWideChar, CustomerUTF8* customerUTF8)
{
	//��ȡת���ַ���
	int SizeOfBuffer = WideCharToMultiByte(CP_UTF8,0,WideChar,SizeOfWideChar,NULL,0,NULL,NULL);
	//����ռ�
	LPSTR lpMultiByteStr = (LPSTR)	malloc(SizeOfBuffer);
	ZeroMemory(lpMultiByteStr, SizeOfBuffer);
	//ת��
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


//��¼��Ϣ
BOOL RecordLog(const TCHAR* LogInfo)
{
	if (!bInitFlag)    //δ��ʼ��
		return FALSE;

	DWORD nNumberOfBytesToWrite = wcslen(LogInfo) * 2;    //Ԥ��д���ֽ��� TCHARΪ˫�ֽ�Ҫ*2
	DWORD nNumberOfBytesWritten = 0;    //ʵ��д���ֽ���
	
	//д��
	if (!WriteFile(hLogFile, LogInfo, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL))
		return FALSE;

	//����
	if (!WriteFile(hLogFile, L"\n", 2, &nNumberOfBytesWritten, NULL))
		return FALSE;


	//ˢ���ļ�������
	FlushFileBuffers(hLogFile);

	return TRUE;
}





//��¼��Ϣ
BOOL RecordTimeAndLog(const TCHAR* LogInfo)
{
	CustomerUTF8 customerUTF8 = { 0 };

	if (!bInitFlag)    //δ��ʼ��
		return FALSE;



	TCHAR localSystemTime[40] = {0};
	//��ȡʱ��
	if (!GetLocalSystemTime(localSystemTime, 40))
		return FALSE;


	DWORD nNumberOfBytesWritten = 0;    //ʵ��д���ֽ���

	if (UTF16ToUTF8(localSystemTime, wcslen(localSystemTime), &customerUTF8))
	{

		//д��ʱ��
		if (!WriteFile(hLogFile, customerUTF8.utf8char, customerUTF8.sizeOfChar, &nNumberOfBytesWritten, NULL))
		{
			ClearUTF8(&customerUTF8);
			return FALSE;
		}
			

		ClearUTF8(&customerUTF8);


	}

	if (UTF16ToUTF8(LogInfo, wcslen(LogInfo), &customerUTF8))
	{

		//д����־
		if (!WriteFile(hLogFile, customerUTF8.utf8char, customerUTF8.sizeOfChar, &nNumberOfBytesWritten, NULL))
		{
			ClearUTF8(&customerUTF8);
			return FALSE;
		}
			
		ClearUTF8(&customerUTF8);
	}


	//����
	if (!WriteFile(hLogFile, L"\n", 2, &nNumberOfBytesWritten, NULL))
		return FALSE;

	//ˢ���ļ�������
	FlushFileBuffers(hLogFile);

	return TRUE;
}


//��ȡ·����
BOOL GetLogFilePath(TCHAR* LogPath, DWORD SizeOfPath)
{
	if (!bInitFlag)    //δ��ʼ��
		return FALSE;

	if (wcscpy_s(LogPath, SizeOfPath, paLogPath))
	{
		ZeroMemory(LogPath, SizeOfPath);
		return FALSE;
	}


	return TRUE;
}

//�رռ�¼��
BOOL CloseLogger()
{
	if (!bInitFlag)    //δ��ʼ��,�����ر�
		return TRUE;


	if (CloseHandle(hLogFile))//�رվ��
	{
		//�����ֵ
		ZeroMemory(paLogPath, MAX_PATH + 1);
		bInitFlag = FALSE;    //��ʼ����ʶ
		hLogFile = INVALID_HANDLE_VALUE;    //�ļ����
		return TRUE;
	}

	return FALSE;
}