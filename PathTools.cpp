#include "pch.h"
#include "PathTools.h"

/**
 * 获取当前用户开机自启路径
 */
UINT GetCurrentUserAutoStartPath(TCHAR* UserAutoStartPaht, UINT CountOfString)
{
    TCHAR WholeStartPath[MAX_PATH + 1] = { 0 };
    TCHAR AutoStartPath[] = L"AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup";
    UINT flag = 0;
    UINT wcsLenWhole = 0;
    TCHAR* TempPath = NULL;
    TCHAR* StartPath = NULL;

    TCHAR UserTempPath[MAX_PATH + 1] = { 0 };
    if (!GetTempPath(MAX_PATH + 1, UserTempPath)) return FALSE;    //路径最大为MAX_PATH+1,只考虑失败的情况

    //获取用户目录
    TempPath = UserTempPath;
    StartPath = WholeStartPath;
    while (*TempPath)
    {
        *StartPath = *TempPath;

        if (*TempPath == '\\')    flag++;
        if (flag >= 3)    break;

        StartPath++;
        TempPath++;

    }
    if (wcscat_s(WholeStartPath, MAX_PATH + 1, AutoStartPath)) return 0;    //拼接完整的启动目录
    wcsLenWhole = wcslen(WholeStartPath);
    if (wcsLenWhole >= CountOfString) return wcsLenWhole + 1;    //所需字符大于缓冲区,返回所需字符数(包含终止的0)

    if (wcscpy_s(UserAutoStartPaht, CountOfString, WholeStartPath)) return 0;    //复制路径到缓冲区


    return wcslen(UserAutoStartPaht);    //返回实际字符数量,不包含结尾0
}

/*
*  获取系统开机自启路径
*/
UINT GetSystemAutoStartPath(TCHAR* SysAutoStartPaht, UINT CountOfString)
{
    TCHAR WholeStartPath[MAX_PATH + 1] = { 0 };
    TCHAR AutoStartPath[] = L"ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp";
    BOOL flag = FALSE;
    UINT wcsLenWhole = 0;
    TCHAR* SysPath = NULL;
    TCHAR* StartPath = NULL;

    TCHAR SystemPath[MAX_PATH + 1] = { 0 };
    if (!GetWindowsDirectory(SystemPath, MAX_PATH)) return FALSE;    //路径最大为MAX_PATH+1,只考虑失败的情况

    //获取系统盘符
    SysPath = SystemPath;
    StartPath = WholeStartPath;
    while (*SysPath)
    {
        *StartPath = *SysPath;

        if (*SysPath == '\\')    flag = TRUE;
        if (flag)    break;

        StartPath++;
        SysPath++;

    }
    if (wcscat_s(WholeStartPath, MAX_PATH + 1, AutoStartPath)) return 0;    //拼接完整的启动目录
    wcsLenWhole = wcslen(WholeStartPath);
    if (wcsLenWhole >= CountOfString) return wcsLenWhole + 1;    //所需字符大于缓冲区,返回所需字符数(包含终止的0)

    if (wcscpy_s(SysAutoStartPaht, CountOfString, WholeStartPath)) return 0;    //复制路径到缓冲区
   

    return wcslen(SysAutoStartPaht);    //返回实际字符数量,不包含结尾0
}


/*
*  创建快捷方式
*/
BOOL CustomCreateLink(CustomShortCut* customShortCut)
{
    HRESULT hres;
    IShellLink* psl;
    BOOL flag = FALSE;

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoInitialize(NULL);
    if (SUCCEEDED(hres))
    {
        hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
        if (SUCCEEDED(hres))
        {
            IPersistFile* ppf;
            //判断源程序
            if (customShortCut->lpszPathObj != NULL && wcslen(customShortCut->lpszPathObj))
            {
                psl->SetPath(customShortCut->lpszPathObj);
                //判断描述
                if (customShortCut->lpszDesc != NULL && wcslen(customShortCut->lpszDesc)) {
                    psl->SetDescription(customShortCut->lpszDesc);

                    // Query IShellLink for the IPersistFile interface, used for saving the 
                    // shortcut in persistent storage. 
                    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

                    if (SUCCEEDED(hres))
                    {
                        //判断链接地址
                        if (customShortCut->lpszPathLink != NULL && wcslen(customShortCut->lpszPathLink))
                        {
                            // Save the link by calling IPersistFile::Save. 
                            hres = ppf->Save(customShortCut->lpszPathLink, TRUE);
                            if (SUCCEEDED(hres))
                            {
                                ppf->Release();
                                flag = TRUE;        //保存完并释放资源代表创建完成,此时返回TRUE
                            }

                        }

                    }

                }

            }

            psl->Release();
        }
        CoUninitialize();
    }

    return flag;
}