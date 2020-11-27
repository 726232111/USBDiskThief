#pragma once
#include <windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>

struct CustomShortCut
{
    LPCWSTR lpszPathObj;    //��ݷ�ʽ������һ���ļ�
    LPCWSTR lpszDesc;    //��ݷ�ʽ����
    LPCWSTR lpszPathLink;    //��ݷ�ʽ���·��+����    .lnk��β
};


/**
 * ����:        ��ȡ��ǰ�û���������Ŀ¼
 * ����1:    �ַ�������ָ��,���ڽ���Ŀ¼
 * ����2:    �ַ�����
 * ����ֵ:    ��������ɹ����򷵻�ֵ��TCHAR�и��Ƶ����������ַ����ĳ��ȣ���������ֹ���ַ���
 *            ������ȴ��ڻ������Ĵ�С���򷵻�ֵ�Ǳ���·������Ļ������Ĵ�С,������ֹ���ַ���
 *            �������ʧ�ܣ��򷵻�ֵΪ�㡣
 */
UINT GetCurrentUserAutoStartPath(TCHAR* UserAutoStartPaht, UINT CountOfString);

/**
 * ����:        ��ȡϵͳ��������Ŀ¼
 * ����1:    �ַ�������ָ��,���ڽ���Ŀ¼
 * ����2:    �ַ�����
* ����ֵ:    ��������ɹ����򷵻�ֵ��TCHAR�и��Ƶ����������ַ����ĳ��ȣ���������ֹ���ַ���
 *            ������ȴ��ڻ������Ĵ�С���򷵻�ֵ�Ǳ���·������Ļ������Ĵ�С,������ֹ���ַ���
 *            �������ʧ�ܣ��򷵻�ֵΪ��
 */
UINT GetSystemAutoStartPath(TCHAR* SysAutoStartPaht, UINT CountOfString);

/**
 * ����:        ������ݷ�ʽ
 * ����:        �Զ����ݷ�ʽ�ṹ��
 * ����ֵ:
 * ��ע:        ������΢��ʾ�� -> https://docs.microsoft.com/en-us/windows/win32/shell/links
 */
BOOL CustomCreateLink(CustomShortCut* customShortCut);