#include "hard.h"
#include <stdio.h>
#include "log.h"

#ifdef OS_LINUX
#define strnicmp(a,b,c) strcmp(a, b)
#endif // OS_LINUX
typedef IHardware*      (WINAPI *pfGetHardware)();
typedef VOID            (CALL_METHOD *pfReleaseHardware)(IHardware **ppHardware);

// 过滤掉虚拟网卡
int FilterNetCards(NetworkCardInfo *pCards, int cardNum, NetworkCardInfo *pSortCards, int sortCardNum)
{
    NetworkCardInfo cards2[4];
    int idx=0;
    int i=0;
    for(i=0; i<cardNum; i++)
    {
        NetworkCardInfo &netCard = pCards[i];
        if (0 == strnicmp(netCard.brand, "VMware", strlen("VMware")))
            continue;

        if (idx >= sortCardNum)
            break;

        NetworkCardInfo &sordCard = pSortCards[idx++];
        sordCard = netCard;
    }

    NetworkCardInfo tmpCard;
    for (i=0; i<idx; i++)
    {
        for (int j=i+1; j<idx; j++)
        {
            if (strcmp(pSortCards[i].MACAddress, pSortCards[j].MACAddress) > 0)
            {
                tmpCard = pSortCards[i];
                pSortCards[i] = pSortCards[j];
                pSortCards[j] = tmpCard;
            }
        }
    }
    return idx;
}

extern char g_szFeitianID[128];

void buildPwd(char szPwd[128], int pwdLen)
{
#if 1
	ZeroMemory(szPwd, pwdLen);
	strcpy(szPwd,g_szFeitianID);
	printf("buildPwd=%s\n",szPwd);
#else
    BOOL rt;
    int iRt;
    // 获取硬件信息
    HINSTANCE hinstLib = NULL;
    IHardware *pHard = NULL;
    pfReleaseHardware pfReleaseHard = NULL;
    do
    {
        #ifdef OS_LINUX
        pfGetHardware pfHardware = GetHardware;
        pfReleaseHard = ReleaseHardware;
        /*hinstLib = dlopen("HardInfo.so", RTLD_LAZY);
        if (!hinstLib)
        {
            TraceLevelLog("loadlibrary(HardInfo.so) fail, last error is %d", dlerror());
            break;
        }

        pfGetHardware pfHardware = (pfGetHardware)dlsym(hinstLib, "GetHardware");
        if (NULL == pfHardware)
        {
            TraceLevelLog("dlsym(GetHardware) fail, last error is %d", dlerror());
            break;
        }
        pfReleaseHard = (pfReleaseHardware)dlsym(hinstLib, "ReleaseHardware");
        if (NULL == pfReleaseHard)
        {
            TraceLevelLog("dlsym(ReleaseHardware) fail, last error is %d", dlerror());
            break;
        }*/
        #else
        hinstLib = LoadLibrary("HardInfo.dll");
        if (NULL == hinstLib)
        {
            TraceLevelLog("loadlibrary(HardInfo.dll) fail, last error is %d", ::GetLastError());
            break;
        }

        pfGetHardware pfHardware = (pfGetHardware)GetProcAddress(hinstLib, "GetHardware");
        if (NULL == pfHardware)
        {
            TraceLevelLog("GetProcAddress(GetHardware) fail, last error is %d", ::GetLastError());
            break;
        }
        pfReleaseHard = (pfReleaseHardware)GetProcAddress(hinstLib, "ReleaseHardware");
        if (NULL == pfReleaseHard)
        {
            TraceLevelLog("GetProcAddress(ReleaseHardware) fail, last error is %d", ::GetLastError());
            break;
        }
        #endif // OS_LINUX

        pHard = pfHardware();
        if (NULL == pHard)
        {
            TraceLevelLog("GetHardware fail, malloc fail");
            break;
        }
        MainBoardInfo board;
        rt = pHard->GetBoardInfo(board);
        if (!rt)
        {
            TraceLevelLog("GetBoardInfo() fail ,rt=%d, last error is %d", rt, pHard->LastErr());
        }

        CpuInfo cpu;
        rt = pHard->GetCPUInfo(cpu);
        if (!rt)
        {
            TraceLevelLog("GetCPUInfo() fail ,rt=%d, last error is %d", rt, pHard->LastErr());
        }
        HardDiskInfo disk[1];
        iRt = pHard->GetHardDiskInfo(disk, sizeof(disk)/sizeof(disk[0]));
        if (1 > iRt)
        {
            TraceLevelLog("GetHardDiskInfo() fail ,iRt=%d, last error is %d", iRt, pHard->LastErr());
        }
        NetworkCardInfo netCards[5];
        int netCardNum = sizeof(netCards)/sizeof(netCards[0]);
        netCardNum = pHard->GetNetworkCardInfo(netCards, netCardNum);
        if (1 > netCardNum)
        {
            TraceLevelLog("GetNetworkCardInfo() fail ,netCardNum=%d, last error is %d", netCardNum, pHard->LastErr());
        }
        // 设置密码
        // 密码可以用硬件信息作为标识，比如 硬盘序列号+CPU序列号+主板序列号+网卡1MAC地址[+网卡2MAC地址[+网卡3MAC地址[+网卡4MAC地址]]]
        ZeroMemory(szPwd, pwdLen);

        /*strcpy(disk[0].factoryNumber, "/FV5R2Y1/CN7016335F0B0A/");
        netCardNum = 1;
        strcpy(netCards[0].MACAddress, "00:0C:29:98:14:A2");
        strcpy(cpu.factoryNumber, "BFEBFBFF00030687");
        strcpy(board.factoryNumber, "WD-WCABCFETDG56");*/

        sprintf(szPwd, "%s|", disk[0].factoryNumber);
        sprintf(szPwd+strlen(szPwd), "%s|", cpu.factoryNumber);
        sprintf(szPwd+strlen(szPwd), "%s|", board.factoryNumber);
        NetworkCardInfo filterCards[4] = {{}};
        int filterNum = sizeof(filterCards)/sizeof(filterCards[0]);
        // 过滤网卡
        filterNum = FilterNetCards(netCards, netCardNum, filterCards, filterNum);
        for (int i=0; i<filterNum; i++)
        {
			if (filterCards[i].MACAddress[0] != 0)
				sprintf(szPwd+strlen(szPwd), "%s|", filterCards[i].MACAddress);
        }
    } while (0);

    // 释放
    if (pfReleaseHard && pHard)
    {
        pfReleaseHard(&pHard);
    }
    if (hinstLib)
    {
        #ifdef OS_LINUX
        //dlclose(hinstLib);
        #else
        FreeLibrary(hinstLib);
        #endif // OS_LINUX
    }
#endif
}

unsigned int hashFun(char *str)
{
    register unsigned int h;
    register unsigned char *p;
    for(h=0, p = (unsigned char *)str; *p ; p++)
        h = 31 * h + *p;
    return h;
}