
#include "pch.h"
#include "FileTools.h"


//获取最大剩余空间的硬盘驱动器
BOOL GetMAXFreeSpaceDiskDrive(TCHAR* MAXFreeSpaceDiskDrive, int strLength)
{
    TCHAR ptaSystemDrive[MAX_PATH] = { 0 };    //当前系统盘

    TCHAR ptaDriveList[MAX_PATH] = { 0 };    //驱动器列表
    PTCHAR pDrive = ptaDriveList;    //用于遍历驱动器列表

    ULARGE_INTEGER MAXFreeSpace = { 0 };    //最大剩余磁盘数
    ULARGE_INTEGER TotalNumberOfFreeBytes = { 0 };    //遍历驱动器时接收剩余空间



    //返回值
    UINT uRetGetSystemDirectory = 0; //GetSystemDirectory返回值
    DWORD  dRetGetLogicalDriveStrings = 0;//GetLogicalDriveStrings返回值


    /*
    1 获取系统磁盘
    2 获取本地所有磁盘列表
    3 遍历磁盘列表
        1 判断当前磁盘是否为系统盘 -> 是,退出本次循环
        2 判断当前磁盘是否为硬盘 -> 否,退出本次循环
        3 获取当前磁盘剩余大小
        4 判断当前磁盘是否为剩余空间最多的磁盘
            使用变量标识MAX磁盘的剩余空间,进行比较 -> 当前磁盘剩余空间多则替换
    */
    //0 默认设置
    ZeroMemory(MAXFreeSpaceDiskDrive, strLength);    //置零,避免含有垃圾数据


    //1 获取系统磁盘
    uRetGetSystemDirectory = GetSystemDirectory(ptaSystemDrive, MAX_PATH);
    if (!uRetGetSystemDirectory || uRetGetSystemDirectory > MAX_PATH) //GetSystemDirectory失败返回0,如果缓冲区不足,返回所需换冲区大小
    {
        MessageBox(NULL, L"GetMAXFreeSpaceDiskDriver->uRetGetSystemDirectory()失败", NULL, MB_OK);
        return FALSE;
    }

    //2 获取本地所有磁盘列表
    dRetGetLogicalDriveStrings = GetLogicalDriveStrings(MAX_PATH, ptaDriveList);
    if (!dRetGetLogicalDriveStrings || dRetGetLogicalDriveStrings > MAX_PATH) //GetLogicalDriveStrings失败返回0,如果缓冲区不足,返回所需换冲区大小
    {
        MessageBox(NULL, L"GetMAXFreeSpaceDiskDriver->GetLogicalDriveStrings()失败", NULL, MB_OK);
        return FALSE;
    }

    //3 遍历磁盘列表
    do
    {
        //MessageBox(NULL, pDrive, NULL, MB_OK);    //测试


        //1 判断当前磁盘是否为系统盘->是, 退出本次循环
        //2 判断当前磁盘是否为硬盘->否, 退出本次循环
        //3 获取当前驱动器剩余空间->失败, 退出本次循环
        if (*pDrive == *ptaSystemDrive || GetDriveType(pDrive) != DRIVE_FIXED || !GetDiskFreeSpaceEx(pDrive, NULL, NULL, &TotalNumberOfFreeBytes))
        {
            pDrive += 4; //指向下一个驱动器名,结束是以2个0结束,因此指向0时说明没有驱动器了
            continue;
        }


        //4  判断当前磁盘是否为剩余空间最多的磁盘
        //        使用变量标识MAX磁盘的剩余空间, 进行比较->当前磁盘剩余空间多则替换
        if (TotalNumberOfFreeBytes.QuadPart > MAXFreeSpace.QuadPart)
        {
            ZeroMemory(MAXFreeSpaceDiskDrive, strLength);
            if (wcscpy_s(MAXFreeSpaceDiskDrive, strLength, pDrive))//拷贝目录
            {
                //非0失败
                MessageBox(NULL, L"wcscpy_s(MAXFreeSpaceDiskDrive, strLength, pDrive)失败", NULL, MB_OK);
                return FALSE;
            }
            MAXFreeSpace.QuadPart = TotalNumberOfFreeBytes.QuadPart;
        }
        pDrive += 4; //指向下一个驱动器名,结束是以2个0结束,因此指向0时说明没有驱动器了
    } while (*pDrive);


    if (*MAXFreeSpaceDiskDrive == 0) return FALSE;    //判断获取磁盘剩余空间是否一直失败

    return TRUE;
}


