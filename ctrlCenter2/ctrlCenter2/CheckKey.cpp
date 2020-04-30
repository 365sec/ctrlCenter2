#include "IFtRock3Key.h"
#include "IHardInfo.h"
#include "GrxaCommon.h"
#include <stdlib.h>
#include <string.h>

#include "key.h"
#include "log.h"
#include "hard.h"

char g_szFeitianID[128]={0};


typedef IFtRock3Key*    (WINAPI *pfGetRock3Key)(CHAR szVendorID[16], CHAR szVendorPin[64]);
typedef VOID            (CALL_METHOD *pfReleaseRock3Key)(IFtRock3Key** ppRock3);

extern USE_KEY_INFO g_useKeyInfo[USE_KEY_TYPE_NUM];
extern KEY_TOOL_TYPE_INFO   g_keyToolInfo[TOOL_TYPE_NUM];

#ifdef OS_LINUX
#include <dlfcn.h>
#endif


BOOL    GetKeyData(char *szDecryptBuf, int iDescryptBufLen, GRXA_KEY_DATA& keyData)
{
    char *pDecryptBuf = NULL;

    char szData[512];
    int  iDataLen = sizeof(szData) - 1;
    char *szEnd     = "\r\n";

    BOOL bSuccess = FALSE;

    do
    {
        pDecryptBuf     =  szDecryptBuf;

        // ToolType=1
        if (GetConfigData(pDecryptBuf, iDescryptBufLen, "ToolType=", szEnd, szData, iDataLen))
        {
            KEY_TOOL_TYPE eToolType = (KEY_TOOL_TYPE)atoi(szData);
            switch (eToolType)
            {
            case KEY_TOOL_TYPE_UNIVERSAL:   // 通用的工具类型
            case KEY_TOOL_TYPE_WINDOWS:     // Windos主机检查工具
            case KEY_TOOL_TYPE_TROJAN:      // 木马检查工具
            case KEY_TOOL_TYPE_VIRUS:       // 病毒检查工具
            case KEY_TOOL_TYPE_WEBSHELL:    // Webshll检查工具
            case KEY_TOOL_TYPE_SERVER:      // 服务器
                {
                    keyData.eToolType = eToolType;
                    strcpy(keyData.szToolName, g_keyToolInfo[eToolType].szToolName);
                    break;
                }
            default:
                break;
            }
        }

        // ToolName=Windows主机安全检查工具

        // Enable=enable|disable
        if (GetConfigData(pDecryptBuf, iDescryptBufLen, "Enable=", szEnd, szData, iDataLen))
            strcpy(keyData.szEnable, szData);

        // SerialNumber=3AA00000000007683
        if (GetConfigData(pDecryptBuf, iDescryptBufLen, "SerialNumber=", szEnd, szData, iDataLen))
            strcpy(keyData.szSerialNum, szData);

        // Version=2.1
        if (GetConfigData(pDecryptBuf, iDescryptBufLen, "Version=", szEnd, szData, iDataLen))
            strcpy(keyData.szVersion, szData);

        // Buyer=购买者
        if (GetConfigData(pDecryptBuf, iDescryptBufLen, "Buyer=", szEnd, szData, iDataLen))
            strcpy(keyData.szBuyer, szData);

        // BuyTime=20141201
        if (GetConfigData(pDecryptBuf, iDescryptBufLen, "BuyTime=", szEnd, szData, iDataLen))
            strcpy(keyData.szBuyDate, szData);

        // LastAccessTime=2014020
        if (GetConfigData(pDecryptBuf, iDescryptBufLen, "LastAccessTime=", szEnd, szData, iDataLen))
            strcpy(keyData.szLastAccessDate, szData);

        if (0 != pDecryptBuf[KEY_HEAD_LEN])
            memcpy(keyData.szCustomData, pDecryptBuf+KEY_HEAD_LEN, KEY_CUSTOM_DATA_LEN);

        bSuccess = TRUE;

    } while (0);

    return bSuccess;
}

BOOL    GetCustomData(char *szDecryptBuf, int iDescryptBufLen, BYTE *pCustomBuf, DWORD &dwCustomBufLen)
{
    BOOL bSuccess = FALSE;

    if ((NULL == szDecryptBuf) || (0 == iDescryptBufLen) || (NULL == pCustomBuf) || (0 == dwCustomBufLen))
        return FALSE;

    do
    {
        memcpy(pCustomBuf, szDecryptBuf+KEY_HEAD_LEN, KEY_CUSTOM_DATA_LEN);
        dwCustomBufLen = KEY_CUSTOM_DATA_LEN;
        pCustomBuf[dwCustomBufLen] = 0;

        bSuccess = TRUE;

    }while (0);

    return bSuccess;
}

BOOL    BuildConfigFile(KEY_TOOL_TYPE eToolType, CHAR szVersion[64], CHAR szBuyer[256], CHAR szBuyTime[64], VOID* pCustomBuf,
                        DWORD dwCustomBufLen, CHAR szHardID[64], char  szWriteBuf[ROCK5_KEY_MEM_LEN], int &iWriteBufLen)
{
    BOOL bSuccess = FALSE;

    char  szEncryptBuf[1024] = "";
    int   iEncryptBufLen = sizeof(szEncryptBuf);

    do
    {
        //ChangePwd(szKeyPwd/*USB_KEY_PWD*/);

        char  *pWriteBuf = szWriteBuf;
        char  szHeadBuf[KEY_HEAD_LEN] = "";

        // ToolType=1  // 工具类型:1、windows主机检查；2、木马检查工具；3、病毒检查工具；4、webshell检查工具
        // ToolName=Windows主机检查工具
        // Enable=enable|disable
        // SerialNumber=3AA00000000007683
        // Version=2.1
        // Buyer=购买者
        // BuyTime=20141201
        // LastAccessTime=2014020
        int iBufLen = sprintf(pWriteBuf,
            "ToolType=%d\r\n"\
            "ToolName=%s\r\n"
            "SerialNumber=%s\r\n"\
            "Version=%s\r\n"\
            "Buyer=%s\r\n"\
            "BuyTime=%s\r\n"\
            "LastAccessTime=%s",
            eToolType,
            g_keyToolInfo[eToolType].szToolName,
            szHardID,
            szVersion,
            szBuyer,
            szBuyTime,
            szBuyTime  // 第一次发布，购买时间与更新时间相同
            );

        iBufLen = strlen(pWriteBuf);

        pWriteBuf[iBufLen] = 0; // 将结尾的0包含进去，因为读取时会获取到未初始化的数据，无法判断结束位置。
        //iBufLen += 1;

        iWriteBufLen = KEY_HEAD_LEN;
        pWriteBuf = szWriteBuf + KEY_HEAD_LEN;
        // 前面1024(KEY_HEAD_LEN)字节写入配置信息，1024字节之后再写入备用数据，比如函数名
        if ((NULL != pCustomBuf) && (0 != dwCustomBufLen))
        {
            memcpy(pWriteBuf, pCustomBuf, dwCustomBufLen);
            pWriteBuf[dwCustomBufLen] = 0;  // 将结尾的0包含进去，因为读取时会获取到未初始化的数据，无法判断结束位置。
            iBufLen = dwCustomBufLen + 1;
        }
        else
        {
            pWriteBuf[0] = 0;  // 将结尾的0包含进去，因为读取时会获取到未初始化的数据，无法判断结束位置。
            iBufLen = dwCustomBufLen + 1;
        }

        iWriteBufLen += iBufLen;

//         int iPadLen = iBufLen&0xf;
//         if (iPadLen)
//         {
//             iPadLen = 16 - iPadLen;
//             memset(pWriteBuf+iBufLen, 0, iPadLen);
//         }
//         iWriteBufLen += iBufLen + iPadLen;
//
//         pWriteBuf = szWriteBuf;
//         //  检查开发商密码
//         //  设置解密Key
//         //  加密文件
//         Encrypt(pWriteBuf, iWriteBufLen, szEncryptBuf, iEncryptBufLen);
//
//         //写存储区(0-7168字节)
//         // DWORD WINAPI RY3_Write(RY_HANDLE handle, int offset, BYTE* pInbuf, int len);
//         m_dwLastError = RY3_Write(m_hRock5Device[m_dwCurDevice], 0, (BYTE *)szEncryptBuf, iEncryptBufLen);
//         if (RY3_SUCCESS != m_dwLastError)
//         {
//             return FALSE;
//         }

        bSuccess = TRUE;
    } while (0);

    return bSuccess;
}

BOOL    CheckKey(GRXA_KEY_DATA &tKeyData, KEY_TOOL_TYPE eToolType, CHAR szHardID[64])
{
    BOOL bCheckOK = FALSE;

    do
    {

        //         if (!GetKeyData(tKeyData))
        //             break;

        // Enable=enable|disable
        // SerialNumber=3AA00000000007683
        // Version=2.1
        // Buyer=购买者
        // BuyTime=20141201
        // LastAccessTime=2014020
        BOOL bToolValid = TRUE;   // 检查工具是否有效，配置文件中有工具类型
        switch (eToolType)
        {
        case KEY_TOOL_TYPE_WINDOWS:
        case KEY_TOOL_TYPE_TROJAN:
        case KEY_TOOL_TYPE_VIRUS:
        case KEY_TOOL_TYPE_WEBSHELL:
        case KEY_TOOL_TYPE_SERVER:
            {
                if (eToolType != tKeyData.eToolType)
                    bToolValid = FALSE;
            }
            break;
        case KEY_TOOL_TYPE_UNIVERSAL:
        default:
            break;
        }

        if (!bToolValid)
            break;

        // Enable=enable|disable
        if(strstr(tKeyData.szEnable, "disable"))
        {
            bCheckOK = TRUE;
            break;
        }

        // SerialNumber=0415150000000543
        if (0 == strncmp(tKeyData.szSerialNum, szHardID, strlen(szHardID)))
        {
            bCheckOK = TRUE;
            break;
        }else{
		    printf("szSerial not equal\n");
		}

    } while (0);

    return bCheckOK;
}


int CheckKeyValid(char szVendorID[16], char szVendorPin[64], KeyContent &keyCon)
{
    char  plaintext[2048] = "";
    int   plaintextLen = sizeof(plaintext);
    char  ciphertext[2048] = "";
    int   ciphertextLen = sizeof(ciphertext);

    int err = 0;
    BOOL rt = FALSE;
    int iRt = 0;
    HINSTANCE hinstLib = NULL;
    IFtRock3Key *pKey = NULL;
    pfReleaseRock3Key pfReleaseKey = NULL;

    // 写入Key的头
	GRXA_KEY_DATA keyData={0};

    do
    {
        #ifdef OS_LINUX
        pfGetRock3Key pfRock3 = GetRock3Key;
        pfReleaseKey = ReleaseRock3Key;
        /*
        hinstLib = dlopen("ftrock3key.so", RTLD_LAZY);
        if (NULL == hinstLib)
        {
            TraceLevelLog("dlopen(ftrock3key.so) fail, last error is %d\n", dlerror());
            break;
        }

        pfGetRock3Key pfRock3 = (pfGetRock3Key)dlsym(hinstLib, "GetRock3Key");
        if (NULL == pfRock3)
        {
            TraceLevelLog("dlsym(GetRock3Key) fail, last error is %d\n", dlerror());
            break;
        }
        pfReleaseKey = (pfReleaseRock3Key)dlsym(hinstLib, "ReleaseRock3Key");
        if (NULL == pfReleaseKey)
        {
            TraceLevelLog("dlsym(ReleaseRock3Key) fail, last error is %d\n", dlerror());
            break;
        }*/
        #else
        hinstLib = LoadLibrary("ftrock3key.dll");
        if (NULL == hinstLib)
        {
            err = -1;
            TraceLevelLog("loadlibrary(ftrock3key.dll) fail, last error is %d", ::GetLastError());
            break;
        }

        pfGetRock3Key pfRock3 = (pfGetRock3Key)GetProcAddress(hinstLib, "GetRock3Key");
        if (NULL == pfRock3)
        {
            err = -2;
            TraceLevelLog("GetProcAddress(GetRock3Key) fail, last error is %d", ::GetLastError());
            break;
        }
        pfReleaseKey = (pfReleaseRock3Key)GetProcAddress(hinstLib, "ReleaseRock3Key");
        if (NULL == pfReleaseKey)
        {
            err = -3;
            TraceLevelLog("GetProcAddress(ReleaseRock3Key) fail, last error is %d", ::GetLastError());
            break;
        }
        #endif // OS_LINUX

        pKey = pfRock3(szVendorID, szVendorPin);
        if (NULL == pKey)
        {
            err = -4;
            TraceLevelLog("pfRock3() fail, malloc fail\n");
            break;
        }
        TraceLevelLog("pfRock3() ok\n");

        DWORD keyNum = 0;
        rt = pKey->GetDevices(keyNum);
        if (!rt)
        {
            err = -5;
            TraceLevelLog("GetDevices rt=%d, has total %d key, last error is %d\n", rt, keyNum, pKey->GetLastError());
            break;
        }
        TraceLevelLog("GetDevices rt=%d, has total %d key\n", rt, keyNum);

        // 多个Keyd需要根据实际需求设置，从1开始到keyNum+1
        int keyidx = 1;
        rt = pKey->SetDevice(keyidx);
        if (!rt)
        {
            err = -6;
            TraceLevelLog("SetDevice(%d) rt=%d, last error is %d\n", keyidx, rt, pKey->GetLastError());
            break;
        }
        TraceLevelLog("SetDevice(%d) ok\n", keyidx);

        rt = pKey->OpenDevice();
        if (!rt)
        {
            err = -7;
            TraceLevelLog("OpenDevice() rt=%d, last error is %d\n", rt, pKey->GetLastError());
            break;
        }
        TraceLevelLog("OpenDevice ok\n");

		//
		pKey->GetHardID(g_szFeitianID);


		//
        char szPwd[512] = "";
        // 构造加解密密码
        buildPwd(szPwd, sizeof(szPwd));
        TraceLevelLog("buildPwd(%s)\n", szPwd);

        // 密码转成哈希值
        unsigned int hashValue = hashFun(szPwd);
        sprintf(szPwd, "%x", hashValue);
        //TraceLevelLog("hashFun = %s\n", szPwd);

        // 设置密码
        pKey->SetEncryptPwd(szPwd);
        printbuf(szPwd, strlen(szPwd));

        // 读取数据
        rt = pKey->ReadKeyBuf(ciphertext, ciphertextLen, 0);

        if (!rt)
        {
            err = -8;
            TraceLevelLog("ReadKeyBuf() len=%d rt=%d, last error is %d\n", ciphertextLen, rt, pKey->GetLastError());
            break;
        }
        TraceLevelLog("ReadKeyBuf rt=%d\n", rt);

        memcpy(plaintext, ciphertext, ciphertextLen);
        // 解密
        rt = pKey->Decrypt(plaintext, plaintextLen);
        if (!rt)
        {
            err = -9;
            TraceLevelLog("Decrypt() iRt=%d\n", rt);
            break;
        }

        printbuf(plaintext, plaintextLen);
        printbuf(ciphertext, ciphertextLen);

        //TraceLevelLog("Decrypt = %d, buf is %s, custom data is %s\n", rt, plaintext, plaintext+KEY_HEAD_LEN);

        rt = GetKeyData(plaintext, plaintextLen, keyData);

        char szKeyHard[128] = "";
        rt = pKey->GetHardID(szKeyHard);
        if (!rt)
        {
            err = -10;
            TraceLevelLog("GetHardID() fail, rt=%d, last error is %d\n", rt, pKey->GetLastError());
            break;
        }
        //TraceLevelLog("GetHardID(%s)\n", szKeyHard);

        // 检查Key有效性，需要自己根据实际情况修改
        rt = CheckKey(keyData, KEY_TOOL_TYPE_SERVER, szKeyHard);
		if (!rt)
		{
            err = -100;
            TraceLevelLog("CheckKey() fail, rt=%d, last error is %d\n", rt, pKey->GetLastError());
			break;
		}
        TraceLevelLog("CheckKey = %d\n", rt);

        char kc[10][128] = {{0}};
        memset(kc, 0, sizeof(char**));
        char *substr = strtok((char*)keyData.szCustomData,"|");
        int i = 0;
        while (substr != NULL)
        {
            strcpy(kc[i], substr);
            i++;
            substr = strtok(NULL, "|");
        }
		if(i < 10){
			if (i >4){
			//为了兼容老key
				 strcpy(keyCon.commandSql, kc[0]);
				 strcpy(keyCon.resultSql, kc[1]);
				 strcpy(keyCon.clientSql, kc[2]);
				 strcpy(keyCon.userSql, kc[3]); 
				 keyCon.userNum=1000;
				 keyCon.lasttime=0;
			}else{
			   err = -88;
			   printf("i=%d\n",i);
			   TraceLevelLog("CheckKey()  error ! i < 10");
			   break;
			}
		}else{
		    strcpy(keyCon.commandSql, kc[4]);
			strcpy(keyCon.resultSql, kc[5]);
			strcpy(keyCon.clientSql, kc[6]);
			strcpy(keyCon.userSql, kc[7]);
			strcpy(keyCon.key_version,kc[8]);
			keyCon.userNum = atoi(kc[9]);
			keyCon.lasttime= atol(kc[10]);
			//verfify time
			printf("key_version=%s \n",keyCon.key_version);
			printf("keyCon.userNum= %d ;  keyCon.lasttime=%d ; time= %d \n",keyCon.userNum ,keyCon.lasttime,time(NULL));
		}
     
		//check for time out 
		if(keyCon.lasttime>0 && keyCon.lasttime < time(NULL)){
            err = -87;
		    TraceLevelLog("CheckKey()  key expired"); 
			break;
		}
    } while (0);

    // 释放申请的资源
    if (pfReleaseKey && pKey)
    {
        pfReleaseKey(&pKey);
    }
    if (hinstLib)
    {
        #ifdef OS_LINUX
        //dlclose(hinstLib);
        #else
        FreeLibrary(hinstLib);
        #endif // OS_LINUX
    }
    
    return err;
}

int ReadKey(char szVendorID[16], char szVendorPin[64], KeyContent& keyCon)
{
    int iRt = CheckKeyValid(szVendorID, szVendorPin, keyCon);
	return iRt;
}

