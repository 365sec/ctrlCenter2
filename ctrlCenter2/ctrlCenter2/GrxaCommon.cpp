#include "GrxaCommon.h"


USE_KEY_INFO g_useKeyInfo[USE_KEY_TYPE_NUM] = 
{
    { KEY_TYPE_NONUSE,          "��ʹ��" },
    { KEY_TYPE_TAIJITONG,       "̩��ͨ" },
    { KEY_TYPE_FEITIAN,         "�������" },
    { KEY_TYPE_FEITIAN_ROCK5,   "�������Rock5" },
    { KEY_TYPE_FEITIAN_ROCK3,   "�������Rock3" },
};

KEY_TOOL_TYPE_INFO   g_keyToolInfo[TOOL_TYPE_NUM] = 
{
    { KEY_TOOL_TYPE_UNIVERSAL,  "ͨ�ù�������" },
    { KEY_TOOL_TYPE_WINDOWS,    "Windows������鹤��" },
    { KEY_TOOL_TYPE_TROJAN,     "ľ���鹤��" },
    { KEY_TOOL_TYPE_VIRUS,      "������鹤��" },
    { KEY_TOOL_TYPE_WEBSHELL,   "Webshell��鹤��" },
    { KEY_TOOL_TYPE_SERVER,     "����������" },
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


