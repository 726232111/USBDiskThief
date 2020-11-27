#include "pch.h"
#include "PathTools.h"

/**
 * ��ȡ��ǰ�û���������·��
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
    if (!GetTempPath(MAX_PATH + 1, UserTempPath)) return FALSE;    //·�����ΪMAX_PATH+1,ֻ����ʧ�ܵ����

    //��ȡ�û�Ŀ¼
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
    if (wcscat_s(WholeStartPath, MAX_PATH + 1, AutoStartPath)) return 0;    //ƴ������������Ŀ¼
    wcsLenWhole = wcslen(WholeStartPath);
    if (wcsLenWhole >= CountOfString) return wcsLenWhole + 1;    //�����ַ����ڻ�����,���������ַ���(������ֹ��0)

    if (wcscpy_s(UserAutoStartPaht, CountOfString, WholeStartPath)) return 0;    //����·����������


    return wcslen(UserAutoStartPaht);    //����ʵ���ַ�����,��������β0
}

/*
*  ��ȡϵͳ��������·��
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
    if (!GetWindowsDirectory(SystemPath, MAX_PATH)) return FALSE;    //·�����ΪMAX_PATH+1,ֻ����ʧ�ܵ����

    //��ȡϵͳ�̷�
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
    if (wcscat_s(WholeStartPath, MAX_PATH + 1, AutoStartPath)) return 0;    //ƴ������������Ŀ¼
    wcsLenWhole = wcslen(WholeStartPath);
    if (wcsLenWhole >= CountOfString) return wcsLenWhole + 1;    //�����ַ����ڻ�����,���������ַ���(������ֹ��0)

    if (wcscpy_s(SysAutoStartPaht, CountOfString, WholeStartPath)) return 0;    //����·����������
   

    return wcslen(SysAutoStartPaht);    //����ʵ���ַ�����,��������β0
}


/*
*  ������ݷ�ʽ
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
            //�ж�Դ����
            if (customShortCut->lpszPathObj != NULL && wcslen(customShortCut->lpszPathObj))
            {
                psl->SetPath(customShortCut->lpszPathObj);
                //�ж�����
                if (customShortCut->lpszDesc != NULL && wcslen(customShortCut->lpszDesc)) {
                    psl->SetDescription(customShortCut->lpszDesc);

                    // Query IShellLink for the IPersistFile interface, used for saving the 
                    // shortcut in persistent storage. 
                    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

                    if (SUCCEEDED(hres))
                    {
                        //�ж����ӵ�ַ
                        if (customShortCut->lpszPathLink != NULL && wcslen(customShortCut->lpszPathLink))
                        {
                            // Save the link by calling IPersistFile::Save. 
                            hres = ppf->Save(customShortCut->lpszPathLink, TRUE);
                            if (SUCCEEDED(hres))
                            {
                                ppf->Release();
                                flag = TRUE;        //�����겢�ͷ���Դ���������,��ʱ����TRUE
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