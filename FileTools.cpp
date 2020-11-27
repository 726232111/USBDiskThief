
#include "pch.h"
#include "FileTools.h"


//��ȡ���ʣ��ռ��Ӳ��������
BOOL GetMAXFreeSpaceDiskDrive(TCHAR* MAXFreeSpaceDiskDrive, int strLength)
{
    TCHAR ptaSystemDrive[MAX_PATH] = { 0 };    //��ǰϵͳ��

    TCHAR ptaDriveList[MAX_PATH] = { 0 };    //�������б�
    PTCHAR pDrive = ptaDriveList;    //���ڱ����������б�

    ULARGE_INTEGER MAXFreeSpace = { 0 };    //���ʣ�������
    ULARGE_INTEGER TotalNumberOfFreeBytes = { 0 };    //����������ʱ����ʣ��ռ�



    //����ֵ
    UINT uRetGetSystemDirectory = 0; //GetSystemDirectory����ֵ
    DWORD  dRetGetLogicalDriveStrings = 0;//GetLogicalDriveStrings����ֵ


    /*
    1 ��ȡϵͳ����
    2 ��ȡ�������д����б�
    3 ���������б�
        1 �жϵ�ǰ�����Ƿ�Ϊϵͳ�� -> ��,�˳�����ѭ��
        2 �жϵ�ǰ�����Ƿ�ΪӲ�� -> ��,�˳�����ѭ��
        3 ��ȡ��ǰ����ʣ���С
        4 �жϵ�ǰ�����Ƿ�Ϊʣ��ռ����Ĵ���
            ʹ�ñ�����ʶMAX���̵�ʣ��ռ�,���бȽ� -> ��ǰ����ʣ��ռ�����滻
    */
    //0 Ĭ������
    ZeroMemory(MAXFreeSpaceDiskDrive, strLength);    //����,���⺬����������


    //1 ��ȡϵͳ����
    uRetGetSystemDirectory = GetSystemDirectory(ptaSystemDrive, MAX_PATH);
    if (!uRetGetSystemDirectory || uRetGetSystemDirectory > MAX_PATH) //GetSystemDirectoryʧ�ܷ���0,�������������,�������軻������С
    {
        MessageBox(NULL, L"GetMAXFreeSpaceDiskDriver->uRetGetSystemDirectory()ʧ��", NULL, MB_OK);
        return FALSE;
    }

    //2 ��ȡ�������д����б�
    dRetGetLogicalDriveStrings = GetLogicalDriveStrings(MAX_PATH, ptaDriveList);
    if (!dRetGetLogicalDriveStrings || dRetGetLogicalDriveStrings > MAX_PATH) //GetLogicalDriveStringsʧ�ܷ���0,�������������,�������軻������С
    {
        MessageBox(NULL, L"GetMAXFreeSpaceDiskDriver->GetLogicalDriveStrings()ʧ��", NULL, MB_OK);
        return FALSE;
    }

    //3 ���������б�
    do
    {
        //MessageBox(NULL, pDrive, NULL, MB_OK);    //����


        //1 �жϵ�ǰ�����Ƿ�Ϊϵͳ��->��, �˳�����ѭ��
        //2 �жϵ�ǰ�����Ƿ�ΪӲ��->��, �˳�����ѭ��
        //3 ��ȡ��ǰ������ʣ��ռ�->ʧ��, �˳�����ѭ��
        if (*pDrive == *ptaSystemDrive || GetDriveType(pDrive) != DRIVE_FIXED || !GetDiskFreeSpaceEx(pDrive, NULL, NULL, &TotalNumberOfFreeBytes))
        {
            pDrive += 4; //ָ����һ����������,��������2��0����,���ָ��0ʱ˵��û����������
            continue;
        }


        //4  �жϵ�ǰ�����Ƿ�Ϊʣ��ռ����Ĵ���
        //        ʹ�ñ�����ʶMAX���̵�ʣ��ռ�, ���бȽ�->��ǰ����ʣ��ռ�����滻
        if (TotalNumberOfFreeBytes.QuadPart > MAXFreeSpace.QuadPart)
        {
            ZeroMemory(MAXFreeSpaceDiskDrive, strLength);
            if (wcscpy_s(MAXFreeSpaceDiskDrive, strLength, pDrive))//����Ŀ¼
            {
                //��0ʧ��
                MessageBox(NULL, L"wcscpy_s(MAXFreeSpaceDiskDrive, strLength, pDrive)ʧ��", NULL, MB_OK);
                return FALSE;
            }
            MAXFreeSpace.QuadPart = TotalNumberOfFreeBytes.QuadPart;
        }
        pDrive += 4; //ָ����һ����������,��������2��0����,���ָ��0ʱ˵��û����������
    } while (*pDrive);


    if (*MAXFreeSpaceDiskDrive == 0) return FALSE;    //�жϻ�ȡ����ʣ��ռ��Ƿ�һֱʧ��

    return TRUE;
}


