#ifndef GRXA_COMMON_H
#define GRXA_COMMON_H

#include <stdio.h>
#include <string.h>
#include <time.h>



#include "customtypedef.h"



#define KEY_HEAD_LEN            1024            //  Key���ݳ���
#define KEY_CUSTOM_DATA_LEN     1024            //  Key�Զ������ݳ���

#define MAX_READ_BUF_LEN        (1024*1024*10)  //  ���ɵ�xml�ļ����ռ�ÿռ�

typedef enum eUseKeyType
{
    KEY_TYPE_NONUSE,        // ��ʹ��Key
    KEY_TYPE_TAIJITONG,     // ̩��ͨ��Key
    KEY_TYPE_FEITIAN,       // ������ŵ�Key
    KEY_TYPE_FEITIAN_ROCK5, // �������Rock5
    KEY_TYPE_FEITIAN_ROCK3, // �������Rock3
    KEY_TYPE_UNKNOWN,       // δ֪
}USE_KEY_TYPE;

typedef struct tagUseKeyInfo
{
    USE_KEY_TYPE eKeyType;
    char         szKeyType[64];
}USE_KEY_INFO;

typedef enum E_KEY_TOOL_TYPE
{
    KEY_TOOL_TYPE_UNIVERSAL,        // ͨ�õĹ�������
    KEY_TOOL_TYPE_WINDOWS   = 1,    // Windos������鹤��
    KEY_TOOL_TYPE_TROJAN,           // ľ���鹤��
    KEY_TOOL_TYPE_VIRUS,            // ������鹤��
    KEY_TOOL_TYPE_WEBSHELL,         // Webshll��鹤��
    KEY_TOOL_TYPE_SERVER,           // ����������
}KEY_TOOL_TYPE;

typedef struct  T_KEY_TOOL_TYPE_INFO
{
    KEY_TOOL_TYPE   eToolType;
    char            szToolName[128];
}KEY_TOOL_TYPE_INFO;

#define USE_KEY_TYPE_NUM    5
//extern USE_KEY_INFO g_useKeyInfo[USE_KEY_TYPE_NUM];

#define  TOOL_TYPE_NUM      6   // �����������
//extern KEY_TOOL_TYPE_INFO   g_keyToolInfo[TOOL_TYPE_NUM];

typedef struct tagGrxaKeyData
{
    char szEnable[16];
    KEY_TOOL_TYPE eToolType;    // 1, "Windows������鹤��"��2, "ľ���鹤��"��3, "������鹤��"��4, "WebShell��鹤��"
    char szToolName[128];       // Windows������鹤�� | ľ���鹤�� | ������鹤�� | WebShell��鹤��
    char szSerialNum[32];
    char szVersion[32];
    char szBuyer[512];
    char szBuyDate[64];
    char szLastAccessDate[64];
    BYTE szCustomData[KEY_CUSTOM_DATA_LEN+1];

  /*  tagGrxaKeyData()
    {
        strcpy(szEnable, "enable");    // enable|disable
        eToolType   = KEY_TOOL_TYPE_UNIVERSAL;
        strcpy(szToolName, "ͨ�õĹ�������");
        szSerialNum[0]  = 0;
        strcpy(szVersion, "2.1");
        szBuyer[0]      = 0;
        struct tm *ltm;
#ifdef OS_LINUX
        time_t long_time;
        time(&long_time);
        ltm = localtime(&long_time);
#else
        __time64_t long_time;
        _time64( &long_time );           // Get time as 64-bit integer.
        //Convert to local time.
        ltm = _localtime64( &long_time ); // C4996 localtime
#endif
        sprintf(szBuyDate, "%04d%02d%02d", ltm->tm_year+1900, ltm->tm_mon+1, ltm->tm_mday);
        strcpy(szLastAccessDate, szBuyDate);

        ZeroMemory(szCustomData, sizeof(szCustomData));
    }*/
}GRXA_KEY_DATA ;


BOOL    GetConfigData(char* pBuf, int iBufLen, char szBegin[32], char szEnd[4], char *pszData, int iDataLen);


#define MAX_DRIVE_NUM   32

#endif  //GRXA_COMMON_H
