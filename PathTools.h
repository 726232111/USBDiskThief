#pragma once
#include <windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>

struct CustomShortCut
{
    LPCWSTR lpszPathObj;    //快捷方式来自哪一个文件
    LPCWSTR lpszDesc;    //快捷方式描述
    LPCWSTR lpszPathLink;    //快捷方式存放路径+名字    .lnk结尾
};


/**
 * 功能:        获取当前用户开机自启目录
 * 参数1:    字符缓冲区指针,用于接收目录
 * 参数2:    字符数量
 * 返回值:    如果函数成功，则返回值是TCHAR中复制到缓冲区的字符串的长度，不包括终止空字符。
 *            如果长度大于缓冲区的大小，则返回值是保存路径所需的缓冲区的大小,包括终止空字符。
 *            如果函数失败，则返回值为零。
 */
UINT GetCurrentUserAutoStartPath(TCHAR* UserAutoStartPaht, UINT CountOfString);

/**
 * 功能:        获取系统开机自启目录
 * 参数1:    字符缓冲区指针,用于接收目录
 * 参数2:    字符数量
* 返回值:    如果函数成功，则返回值是TCHAR中复制到缓冲区的字符串的长度，不包括终止空字符。
 *            如果长度大于缓冲区的大小，则返回值是保存路径所需的缓冲区的大小,包括终止空字符。
 *            如果函数失败，则返回值为零
 */
UINT GetSystemAutoStartPath(TCHAR* SysAutoStartPaht, UINT CountOfString);

/**
 * 功能:        创建快捷方式
 * 参数:        自定义快捷方式结构体
 * 返回值:
 * 备注:        更改了微软示例 -> https://docs.microsoft.com/en-us/windows/win32/shell/links
 */
BOOL CustomCreateLink(CustomShortCut* customShortCut);