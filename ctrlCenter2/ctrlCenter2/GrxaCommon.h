#ifndef GRXA_COMMON_H
#define GRXA_COMMON_H

#include <stdio.h>
#include <string.h>
#include <time.h>



#include "customtypedef.h"



#define KEY_HEAD_LEN            1024            //  Key内容长度
#define KEY_CUSTOM_DATA_LEN     1024            //  Key自定义数据长度

#define MAX_READ_BUF_LEN        (1024*1024*10)  //  生成的xml文件最大占用空间

typedef enum eUseKeyType
{
    KEY_TYPE_NONUSE,        // 不使用Key
    KEY_TYPE_TAIJITONG,     // 泰吉通的Key
    KEY_TYPE_FEITIAN,       // 飞天诚信的Key
    KEY_TYPE_FEITIAN_ROCK5, // 飞天诚信Rock5
    KEY_TYPE_FEITIAN_ROCK3, // 飞天诚信Rock3
    KEY_TYPE_UNKNOWN,       // 未知
}USE_KEY_TYPE;

typedef struct tagUseKeyInfo
{
    USE_KEY_TYPE eKeyType;
    char         szKeyType[64];
}USE_KEY_INFO;

typedef enum E_KEY_TOOL_TYPE
{
    KEY_TOOL_TYPE_UNIVERSAL,        // 通用的工具类型
    KEY_TOOL_TYPE_WINDOWS   = 1,    // Windos主机检查工具
    KEY_TOOL_TYPE_TROJAN,           // 木马检查工具
    KEY_TOOL_TYPE_VIRUS,            // 病毒检查工具
    KEY_TOOL_TYPE_WEBSHELL,         // Webshll检查工具
    KEY_TOOL_TYPE_SERVER,           // 服务器程序
}KEY_TOOL_TYPE;

typedef struct  T_KEY_TOOL_TYPE_INFO
{
    KEY_TOOL_TYPE   eToolType;
    char            szToolName[128];
}KEY_TOOL_TYPE_INFO;

#define USE_KEY_TYPE_NUM    5
//extern USE_KEY_INFO g_useKeyInfo[USE_KEY_TYPE_NUM];

#define  TOOL_TYPE_NUM      6   // 工具种类个数
//extern KEY_TOOL_TYPE_INFO   g_keyToolInfo[TOOL_TYPE_NUM];

typedef struct tagGrxaKeyData
{
    char szEnable[16];
    KEY_TOOL_TYPE eToolType;    // 1, "Windows主机检查工具"；2, "木马检查工具"；3, "病毒检查工具"；4, "WebShell检查工具"
    char szToolName[128];       // Windows主机检查工具 | 木马检查工具 | 病毒检查工具 | WebShell检查工具
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
        strcpy(szToolName, "通用的工具类型");
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
