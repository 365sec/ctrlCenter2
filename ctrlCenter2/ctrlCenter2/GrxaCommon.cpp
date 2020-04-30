#include "GrxaCommon.h"


USE_KEY_INFO g_useKeyInfo[USE_KEY_TYPE_NUM] = 
{
    { KEY_TYPE_NONUSE,          "不使用" },
    { KEY_TYPE_TAIJITONG,       "泰吉通" },
    { KEY_TYPE_FEITIAN,         "飞天诚信" },
    { KEY_TYPE_FEITIAN_ROCK5,   "飞天诚信Rock5" },
    { KEY_TYPE_FEITIAN_ROCK3,   "飞天诚信Rock3" },
};

KEY_TOOL_TYPE_INFO   g_keyToolInfo[TOOL_TYPE_NUM] = 
{
    { KEY_TOOL_TYPE_UNIVERSAL,  "通用工具类型" },
    { KEY_TOOL_TYPE_WINDOWS,    "Windows主机检查工具" },
    { KEY_TOOL_TYPE_TROJAN,     "木马检查工具" },
    { KEY_TOOL_TYPE_VIRUS,      "病毒检查工具" },
    { KEY_TOOL_TYPE_WEBSHELL,   "Webshell检查工具" },
    { KEY_TOOL_TYPE_SERVER,     "服务器程序" },
};

BOOL    GetConfigData(char* pBuf, int iBufLen, char szBegin[32], char szEnd[4], char *pszData, int iDataLen)
{
    char *pBegin;
    char *pEnd;
    int iLen;
    if (pBegin = strstr(pBuf, szBegin))
    {
        pBegin += strlen(szBegin);
        pEnd = strstr(pBegin, szEnd);
        if (NULL == pEnd)
            pEnd = pBegin + strlen(pBegin);
        iLen = pEnd - pBegin + 1;
        if (iDataLen > iLen)
        {
            strncpy(pszData, pBegin, iLen);
            pszData[iLen] = 0;
            return TRUE;
        }
    }

    return FALSE;
}


