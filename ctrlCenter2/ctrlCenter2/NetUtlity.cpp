//#include "NetComm.h"	//qizc。此文件头包含windows等，不要了。

#include "NetUtlity.h"

//qizc
#include <string>
typedef long LONG;

//对应三个等级的关键词策略 0-level1 1-level2 2-level3
//MsgKeyPolicy	g_tKeyPolicy[3] =	
//{
//	{0, 0, 0, 0, "共产党;"}
//};
//MsgExtPolicy	g_tExtPolicy;

//数据库中的策略配置- 放在这里定义是为了服务器/客户端都能使用
DbCfgPlcy	g_tDbCfgPlcy;

// ★★★★★★★★★★★★★★★★★★★★★
// 在编解码的时候一定要注意 char -> WORD 的转换问题，会出错!!!!!!!!!!!
// ★★★★★★★★★★★★★★★★★★★★★


int NetEncodeAuth(char *pBuf, const int &nLen, NetMsgAuth *pPara)
{
	//需要做内存边界检测
	char *p = pBuf;
	*(WORD*)p = (WORD)NET_MSG_AUTH_REQ;
	p+=2;
	//PID TYPE
	*p++ = pPara->bPidType;
	//length
	*p++ = strlen(pPara->strPid);
	*p++ = strlen(pPara->strName);
	*p++ = strlen(pPara->strPwd);
	//copy pid/wid/pwd
	memcpy_s(p, sizeof(pPara->strPid)-1, &pPara->strPid[0],strlen(pPara->strPid));
	p+=strlen(pPara->strPid);
	memcpy_s( p, sizeof(pPara->strName)-1, &pPara->strName[0],strlen(pPara->strName));
	p+=strlen(pPara->strName);
	memcpy_s(p, sizeof(pPara->strPwd)-1, &pPara->strPwd[0], strlen(pPara->strPwd));
	p+= strlen(pPara->strPwd);
	*(WORD*)p = pPara->wVersion;
	p+=2;
	return (p-pBuf);
}

int NetDecodeAuth(char *pBuf, const int &nLen, NetMsgAuth *pPara)
{
	//需要做内存边界检测
	char *p = pBuf;
	BYTE	bPidLen;
	BYTE	bNameLen;
	BYTE	bPwdLen;
	memset(pPara, 0, sizeof(NetMsgAuth));
	pPara->bPidType = *p++;
	bPidLen = *p++;
	bNameLen = *p++;
	bPwdLen = *p++;
	memcpy_s(&pPara->strPid[0], sizeof(pPara->strPid)-1, p, bPidLen);
	p+=bPidLen;
	memcpy_s(&pPara->strName[0], sizeof(pPara->strName)-1, p, bNameLen);
	p+=bNameLen;
	memcpy_s(&pPara->strPwd[0], sizeof(pPara->strPwd)-1, p, bPwdLen);
	p+=bPwdLen;
	pPara->wVersion = *(WORD*)p;
	return (bPidLen+bNameLen+bPwdLen+3+2);
}

int NetEncodeAuthEx(char *pBuf, const int &nLen, NetMsgAuthEx *pPara)
{
	//需要做内存边界检测
	char *p = pBuf;
	*(WORD*)p = (WORD)NET_MSG_AUTH_REQ;
	p+=2;
	//PID TYPE
	*p++ = pPara->bPidType;
	//length
	*p++ = strlen(pPara->strPid);
	*p++ = strlen(pPara->strName);
	*p++ = strlen(pPara->strPwd);
	//copy pid/wid/pwd
	memcpy_s(p, sizeof(pPara->strPid)-1, &pPara->strPid[0],strlen(pPara->strPid));
	p+=strlen(pPara->strPid);
	//strName
	if (strlen(pPara->strName) > 0)
	{
		memcpy_s( p, sizeof(pPara->strName)-1, &pPara->strName[0],strlen(pPara->strName));
		p+=strlen(pPara->strName);
	}
	//strPwd
	if (strlen(pPara->strPwd) > 0)
	{
		memcpy_s(p, sizeof(pPara->strPwd)-1, &pPara->strPwd[0], strlen(pPara->strPwd));
		p+= strlen(pPara->strPwd);
	}
	//version
	*(WORD*)p = pPara->wVersion;
	p+=2;
	//bExVer and baFlag
	*p++ = (char)pPara->bExVer;
	memcpy(p, pPara->baFlag, GR_AUTH_FLAG_N);
	p+=GR_AUTH_FLAG_N;
	//IpAddr Number
	BYTE i;
	for (i=0;i<GR_AUTH_MAXIP_N;i++)
	{
		if (pPara->dwIpAddr[i] == 0)
			break;
	}
	*p++ = (char)i;
	if (i > 0)
	{
		//baAddrType
		memcpy(p, &pPara->baAddrType[0], i*sizeof(BYTE));
		p+=i*sizeof(BYTE);
		//dwIpAddr
		memcpy(p, &pPara->dwIpAddr[0], i*sizeof(DWORD32));
		p+= i*sizeof(DWORD32);
		//baMacAddr
		memcpy(p, &pPara->baMacAddr[0], i*6); //mac address length-48bits
		p+= i*6;
	}
	return (p-pBuf);
}

int NetDecodeAuthEx(char *pBuf, const int &nLen, NetMsgAuthEx *pPara)
{
	//需要做内存边界检测
	char *p = pBuf;
	BYTE	bPidLen;
	BYTE	bNameLen;
	BYTE	bPwdLen;
	memset(pPara, 0, sizeof(NetMsgAuthEx));
	pPara->bPidType = *p++;
	bPidLen = *p++;
	bNameLen = *p++;
	bPwdLen = *p++;
	if (bPidLen > 0)
	{
		memcpy_s(&pPara->strPid[0], sizeof(pPara->strPid)-1, p, bPidLen);
		p+=bPidLen;
	}
	if (bNameLen > 0)
	{
		memcpy_s(&pPara->strName[0], sizeof(pPara->strName)-1, p, bNameLen);
		p+=bNameLen;
	}
	if (bPwdLen > 0)
	{
		memcpy_s(&pPara->strPwd[0], sizeof(pPara->strPwd)-1, p, bPwdLen);
		p+=bPwdLen;
	}
	//wVersion
	pPara->wVersion = *(WORD*)p;
	p+=2;
	//bExVer 
	pPara->bExVer = (BYTE)*p++;
	memcpy(pPara->baFlag, p, GR_AUTH_FLAG_N);
	p+=GR_AUTH_FLAG_N;
	//IpAddr Number
	bPwdLen = (BYTE)*p++;
	//如果超过合法值，说明异常,不解析
	if (bPwdLen>0 && bPwdLen <= GR_AUTH_MAXIP_N)
	{
		//baAddrType
		memcpy(&pPara->baAddrType[0], p, bPwdLen);
		p += bPwdLen;
		//dwIpAddr
		memcpy(&pPara->dwIpAddr[0], p, bPwdLen*sizeof(DWORD32));
		p += bPwdLen*sizeof(DWORD32);
		//dwIpAddr
		memcpy(&pPara->baMacAddr[0], p, bPwdLen*6);
		p += bPwdLen*6;
	}
	return (p-pBuf);
}

////////////////////////////////////////////////////////

//add by cy
int NetEncodeAuthRsp(char *pBuf, const int &nLen, NetMsgAuthRsp *pPara)
{
	//需要做内存边界检测
	char *p = pBuf;
	*(WORD*)p = (WORD)NET_MSG_AUTH_RSP;
	p+=2;
	//状态0表示成功，其他失败
	*p++ = pPara->status;
	//length
	*p++ = strlen(pPara->strHostDepart);
	*p++ = strlen(pPara->strHostResponsor);

	//strHostDepart
	if (strlen(pPara->strHostDepart) > 0)
	{
		memcpy_s( p, sizeof(pPara->strHostDepart)-1, &pPara->strHostDepart[0],strlen(pPara->strHostDepart));
		p+=strlen(pPara->strHostDepart);
	}
	//strHostResponsor
	if (strlen(pPara->strHostResponsor) > 0)
	{
		memcpy_s(p, sizeof(pPara->strHostResponsor)-1, &pPara->strHostResponsor[0], strlen(pPara->strHostResponsor));
		p+= strlen(pPara->strHostResponsor);
	}
	return (p-pBuf);
}


int NetDecodeAuthRsp(char *pBuf, const int &nLen, NetMsgAuthRsp *pPara)
{
	//需要做内存边界检测
	char *p = pBuf;
	BYTE	bHostDepartLen;
	BYTE	bHostResponsorLen;
	memset(pPara, 0, sizeof(NetMsgAuthRsp));
	//状态0表示成功，其他失败
	pPara->status = *p++;
	bHostDepartLen = *p++;
	bHostResponsorLen = *p++;
	memcpy_s(&pPara->strHostDepart[0], sizeof(pPara->strHostDepart)-1, p, bHostDepartLen);
	p+=bHostDepartLen;
	memcpy_s(&pPara->strHostResponsor[0], sizeof(pPara->strHostResponsor)-1, p, bHostResponsorLen);
	p+=bHostResponsorLen;
	return (bHostDepartLen+bHostResponsorLen+4);
}

/////////////////////////////////////////////////////////////

int NetEncodeAuthEx2(char *pBuf, const int &nLen, NetMsgAuthEx2 *pPara)
{
	//需要做内存边界检测
	BYTE *p = (BYTE*)pBuf;
	*(WORD*)p = (WORD)NET_MSG_AUTH_REQ;
	p+=2;
	//PID TYPE
	*p++ = pPara->bPidType;
	//length
	*p++ = strlen(pPara->strPid);
	*p++ = strlen(pPara->strName);
	*p++ = strlen(pPara->strPwd);
	//copy pid/wid/pwd
	memcpy_s((char*)p, sizeof(pPara->strPid)-1, &pPara->strPid[0],strlen(pPara->strPid));
	p+=strlen(pPara->strPid);
	//strName
	if (strlen(pPara->strName) > 0)
	{
		memcpy_s( (char*)p, sizeof(pPara->strName)-1, &pPara->strName[0],strlen(pPara->strName));
		p+=strlen(pPara->strName);
	}
	//strPwd
	if (strlen(pPara->strPwd) > 0)
	{
		memcpy_s((char*)p, sizeof(pPara->strPwd)-1, &pPara->strPwd[0], strlen(pPara->strPwd));
		p+= strlen(pPara->strPwd);
	}
	//version
	*(WORD*)p = pPara->wVersion;
	p+=2;
	//bExVer and baFlag
	*p++ = pPara->bExVer;
	memcpy(p, pPara->baFlag, GR_AUTH_FLAG_N);
	p+=GR_AUTH_FLAG_N;
	//IpAddr Number
	BYTE i;
	for (i=0;i<GR_AUTH_MAXIP_N;i++)
	{
		if (pPara->dwIpAddr[i] == 0)
			break;
	}
	*p++ = i;
	if (i > 0)
	{
		//baAddrType
		memcpy(p, &pPara->baAddrType[0], i*sizeof(BYTE));
		p+=i*sizeof(BYTE);
		//dwIpAddr
		memcpy(p, &pPara->dwIpAddr[0], i*sizeof(DWORD32));
		p+= i*sizeof(DWORD32);
		//baMacAddr
		memcpy(p, &pPara->baMacAddr[0], i*6); //mac address length-48bits
		p+= i*6;
	}
	//host name len
	i = strlen(pPara->strHost); //越界就截取
	*p++ = i;
	memcpy(p, pPara->strHost, i);
	p += i;
	
	return (p-(BYTE*)pBuf);
}
int NetDecodeAuthEx2(char *pBuf, const int &nLen, NetMsgAuthEx2 *pPara)
{
	//需要做内存边界检测
	char *p = pBuf;
	BYTE	bPidLen;
	BYTE	bNameLen;
	BYTE	bPwdLen;
	memset(pPara, 0, sizeof(NetMsgAuthEx2));
	pPara->bPidType = *p++;
	bPidLen = *p++;
	bNameLen = *p++;
	bPwdLen = *p++;
	if (bPidLen > 0)
	{
		memcpy_s(&pPara->strPid[0], sizeof(pPara->strPid)-1, p, bPidLen);
		p+=bPidLen;
	}
	if (bNameLen > 0)
	{
		memcpy_s(&pPara->strName[0], sizeof(pPara->strName)-1, p, bNameLen);
		p+=bNameLen;
	}
	if (bPwdLen > 0)
	{
		memcpy_s(&pPara->strPwd[0], sizeof(pPara->strPwd)-1, p, bPwdLen);
		p+=bPwdLen;
	}
	//wVersion
	pPara->wVersion = *(WORD*)p;
	p+=2;

	//为了兼容性, 如果待解码的空间不足结构体大小则终止解码
	//如果已解码的长度 + 1 > nLen 则表明没有东西可以解码了
	if (p - pBuf + 1 + 1 > nLen)
		return p-pBuf;
	
	//bExVer 
	pPara->bExVer = (BYTE)*p++;
	memcpy(pPara->baFlag, p, GR_AUTH_FLAG_N);
	p+=GR_AUTH_FLAG_N;
	//IpAddr Number
	bPwdLen = (BYTE)*p++;
	//如果超过合法值，说明异常,不解析
	if (bPwdLen>0 && bPwdLen <= GR_AUTH_MAXIP_N)
	{
		//baAddrType
		memcpy(&pPara->baAddrType[0], p, bPwdLen);
		p += bPwdLen;
		//dwIpAddr
		memcpy(&pPara->dwIpAddr[0], p, bPwdLen*sizeof(DWORD32));
		p += bPwdLen*sizeof(DWORD32);
		//dwIpAddr
		memcpy(&pPara->baMacAddr[0], p, bPwdLen*6);
		p += bPwdLen*6;
	}
	//
	if (pPara->bExVer >= 2)
	{
		//strHost
		bNameLen = *p++;
		memcpy(pPara->strHost, p, bNameLen);
	}
	return (p-pBuf);
}
//[Parameter]
//	nMsg	: 	0 - only encode content , exclude the message type and length
//				1 - normal message
int NetEncodeDbLvlStr(char *pBuf, const WORD &nMsg, DbWarnLvStr *pPara)
{
	if (pBuf == NULL || pPara == NULL )
		return 0;
	
	char *p = pBuf;
	//WORD wLen = sizeof(DbWarnLvStr);//pPara->strMonitor - (char*)pPara;
	//wLen += strlen(pPara->strMonitor);
	pPara->wMsgLen = sizeof(DbWarnLvStr) + strlen(pPara->strMonitor);

	//如果需要填写消息类型则写入整个数据
//	if (nMsg)
	{
		memcpy(p, pPara, pPara->wMsgLen);
		//message type - overlay
		*(WORD*)p = (WORD)nMsg;
	}
	return (int)pPara->wMsgLen;
/*	
	for (int i=0;i<sizeof(pPara->waOffset)/sizeof(pPara->waOffset[0]); i++)
	{
		//waOffset
		memcpy(p, (char *)&pPara->waOffset[i], sizeof(WORD)); 
		p += sizeof(WORD);
	}
	//strMonitor len --- strMonitor - 这个可能会超过256, 故用WORD保存长度
	*(WORD*)p = strlen(pPara->strMonitor);
	p += sizeof(WORD);
	memcpy(p, pPara->strMonitor, strlen(pPara->strMonitor));
	p += strlen(pPara->strMonitor);
	//fill the msg len
	if (nMsg)
	{
		wLen = p-pBuf;
		*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	}
	return (p-pBuf);
*/
}
int NetDecodeDbLvlStr(char *pBuf, const int &nLen, DbWarnLvStr **pPara)
{
	if (pBuf == NULL || nLen <= 0 || pPara == NULL)
		return 0;

	//-------- 必须确保 pPara->strMonitor 不为空并有足够内存------------

	//不能清空，否则指针和长度数据都没了
	//memset(pPara, 0, sizeof(DbFileRegMon));
	
	//WORD wLen = pPara->wLen; //临时保存该变量申请的实际内存,后面再恢复
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	
	//length
	WORD wLen = *(WORD*)p ;	//包含自身message type and message len - at least 4
	if (wLen < sizeof(DbWarnLvStr))
		return 0;
	else if (wLen > 65536)	//过大长度认为无效
		return 0;
	if (!UtlAllocMem(pPara, wLen + 2)) //UtlAllocMemEx 本身已经申请了余量的内存
	{
		return wLen-2; //内存不足无法解析当前数据包,但不能影响后续解包
	}
	
	p += NET_CTNT_LEN;
	
	//waOffset
	memcpy(&(*pPara)->waOffset[0], p, sizeof((*pPara)->waOffset));
	p+=sizeof((*pPara)->waOffset);
	//skip wLen +++++++++++ MUST NOT OVERLAY !!!!!!!
	p+=sizeof((*pPara)->wLen);
	//strMonitor
	memcpy(&(*pPara)->strMonitor[0], p, wLen - sizeof(DbWarnLvStr));
	p+=(wLen - sizeof(DbWarnLvStr));
	(*pPara)->strMonitor[wLen - sizeof(DbWarnLvStr)] = 0;				//append null terminater 
	(*pPara)->wMsgLen = wLen;

	return (p-pBuf);

/*	
	for (int i=0;i<sizeof(pPara->waOffset)/sizeof(pPara->waOffset[0]); i++)
	{
		//waOffset
		memcpy((char *)&pPara->waOffset[i], p, sizeof(WORD)); 
		p += sizeof(WORD);
	}

	//strMonitor len - 这个可能会超过256, 故用WORD保存长度
	wLen = *(WORD *)p ;
	p += sizeof(WORD);
	if (pPara->wLen == 0)
		;
	else if (wLen >= pPara->wLen)
	{
		memcpy(pPara->strMonitor, p, pPara->wLen-1);
		pPara->strMonitor[pPara->wLen-1]=0;
	}
	else
	{
		memcpy(pPara->strMonitor, p, wLen);
		pPara->strMonitor[wLen]=0;
	}
	p += wLen;
	return (p - pBuf);
*/
}
int NetEncodeUsbAct(char *pBuf, const int &nLen, MsgUsbAct *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	if (pPara->nActInsert)
		*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	else
		*(WORD*)p = (WORD)NET_MSG_USB_REMOVE;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//time
	LONG tm = (LONG)pPara->tmEvent;
	memcpy(p, (char *)&tm, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//driver
	*p++ = pPara->cDrive;
	//driver type
	*p++ = pPara->nType;
	//action
	*p++ = pPara->nActInsert;
	//fs name len
	*p++ = strlen(pPara->szFSName);
	memcpy(p, pPara->szFSName, strlen(pPara->szFSName));
	p += strlen(pPara->szFSName);
	//volume name len
	*p++ = strlen(pPara->szVolume);
	memcpy(p, pPara->szVolume, strlen(pPara->szVolume));
	p += strlen(pPara->szVolume);
	//szFriendName len - MAX_PATH-260 ->word
	//*p++ = strlen(pPara->szFriendName);
	*(WORD*)p = strlen(pPara->szFriendName);
	p+=2;
	memcpy(p, pPara->szFriendName, strlen(pPara->szFriendName));
	p += strlen(pPara->szFriendName);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
	//return wLen + NET_MSGTYPE_LEN;
}
int NetDecodeUsbAct(char *pBuf, const int &nLen, MsgUsbAct *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(MsgUsbAct));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	p += NET_CTNT_LEN;
	//time
	//LONG tm = (LONG)pPara->tmEvent;
	memcpy((char *)&pPara->tmEvent, p, (NET_TIME_LEN)); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//driver
	pPara->cDrive = *p++;
	//driver type
	pPara->nType = *p++;
	//action
	pPara->nActInsert = *p++;
	//fs name len
	wLen = *p++;
	memcpy_s(pPara->szFSName, sizeof(pPara->szFSName)-1, p, wLen);
	p += wLen;
	//volume name len
	wLen = *p++;
	memcpy_s(pPara->szVolume, sizeof(pPara->szVolume)-1, p, wLen);
	p += wLen;
	//szFriendName len
	wLen = *(WORD*)p;
	p+=2;
	memcpy_s(pPara->szFriendName, sizeof(pPara->szFriendName)-1, p, wLen);
	p += wLen;

	//fill the msg len
	//WORD wLen = p-pBuf;
	//wLen = *(WORD*)pBuf;

	return (p - pBuf);
}
/*
//发送策略的时候打包发送高中低三个等级???
int NetEncodeKeyPolicy(char *pBuf, const int &nLen, MsgKeyPolicy *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_KEY_POLICY;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, (char *)&pPara->nID, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nLevel
	memcpy(p, (char *)&pPara->nLevel, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nVer
	memcpy(p, (char *)&pPara->nVer, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nType
	memcpy(p, (char *)&pPara->nType, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//fs name len
	*p++ = strlen(pPara->strKeys);
	memcpy(p, pPara->strKeys, strlen(pPara->strKeys));
	p += strlen(pPara->strKeys);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
*/
int NetEncodeWarnLvl3Policy(char *pBuf, const int &nLen, DbWarnLevel *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_WARN_LEVEL;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;

	memcpy(p, pPara, sizeof(DbWarnLevel));
	p+=sizeof(DbWarnLevel);
	memcpy(p, pPara+1, sizeof(DbWarnLevel));
	p+=sizeof(DbWarnLevel);
	memcpy(p, pPara+2, sizeof(DbWarnLevel));
	p+=sizeof(DbWarnLevel);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeWarnLvl3Policy(char *pBuf, const int &nLen, DbWarnLevel *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(DbWarnLevel));
	
	char *p = pBuf;
	//length
	p += NET_CTNT_LEN;
	
	memcpy((char *)pPara, p, sizeof(DbWarnLevel)); 
	p += sizeof(DbWarnLevel);
	memcpy((char *)(pPara+1), p, sizeof(DbWarnLevel)); 
	p += sizeof(DbWarnLevel);
	memcpy((char *)(pPara+2), p, sizeof(DbWarnLevel)); 
	p += sizeof(DbWarnLevel);

	return (p - pBuf);
}
int NetEncode3KeyPolicy(char *pBuf, const int &nLen, DbWarnLvStr *pPara)
{
	return NetEncodeDbLvlStr(pBuf, NET_MSG_KEY3_POLICY, pPara);
}
int NetDecode3KeyPolicy(char *pBuf, const int &nLen, DbWarnLvStr *pPara)
{
	return NetDecodeDbLvlStr(pBuf, nLen, &pPara);
}
int NetEncodeExtPolicy(char *pBuf, const int &nLen, char *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_EXT_POLICY;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	
	//pPara len --- pPara - 这个可能会超过256, 故用WORD保存长度
	*(WORD*)p = strlen(pPara);
	p += sizeof(WORD);
	memcpy(p, pPara, strlen(pPara));
	p += strlen(pPara);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeExtPolicy(char *pBuf, const int &nLen, char *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

//	memset(pPara, 0, sizeof(DbWarnLvStr));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	
	//pPara len - 这个可能会超过256, 故用WORD保存长度
	//下面需要做保护??????
	wLen = *(WORD *)p ;
	p += sizeof(WORD);
	memcpy(pPara, p, wLen);
	pPara[wLen] = 0;
	p += wLen;

	return (p - pBuf);
}

//////////////////////////////////////////////////////////////////////////
// 全局配置策略(包括三级告警关键词/监控后缀名/策略动作)
int NetEncodeCfgPlcy(char *pBuf, const int &nLen, DbCfgPlcy *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_CFG_POLICY;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, (char *)&pPara->nDbId, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nVer
	memcpy(p, (char *)&pPara->nVersion, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwTime
	memcpy(p, (char *)&pPara->dwTime, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;

	//warn level
	memcpy(p, (char *)&pPara->tWarnLvl[0], sizeof(pPara->tWarnLvl));
	p += sizeof(pPara->tWarnLvl);
	
	//key words
	p += NetEncodeDbLvlStr(p, 0, pPara->tKeysPlcy);

	//doc extention type
	//strFileTypes len --- strFileTypes - 这个可能会超过256, 故用WORD保存长度
	*(WORD*)p = strlen(pPara->strFileTypes);
	p += sizeof(WORD);
	memcpy(p, pPara->strFileTypes, strlen(pPara->strFileTypes));
	p += strlen(pPara->strFileTypes);

	
	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeCfgPlcy(char *pBuf, const int &nLen, DbCfgPlcy *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	//不能清除, 否则相关指针就为空了
	//memset(pPara, 0, sizeof(DbCfgPlcy));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//id
	memcpy((char *)&pPara->nDbId, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nVer
	memcpy((char *)&pPara->nVersion, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwTime
	memcpy((char *)&pPara->dwTime, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;

	//warn level
	memcpy((char *)&pPara->tWarnLvl[0], p, sizeof(pPara->tWarnLvl));
	p += sizeof(pPara->tWarnLvl);
	
	//key words
	//下面这个函数的参数起点可能会有问题?????
	p += NET_MSGTYPE_LEN;	//skip message type field
	p += NetDecodeDbLvlStr(p, nLen, &pPara->tKeysPlcy);
	
	//strFileTypes len --- strFileTypes - 这个可能会超过256, 故用WORD保存长度
	wLen = *(WORD *)p ;
	p += sizeof(WORD);
	if (wLen == 0)
		pPara->strFileTypes[wLen]=0;
	else if (wLen >= sizeof(pPara->strFileTypes))
	{
		memcpy(pPara->strFileTypes, p, sizeof(pPara->strFileTypes)-1);
		pPara->strFileTypes[sizeof(pPara->strFileTypes)-1]=0;
	}
	else
	{
		memcpy(pPara->strFileTypes, p, wLen);
		pPara->strFileTypes[wLen]=0;
	}
	p += wLen;

	return (p - pBuf);
}

//////////////////////////////////////////////////////////////////////////
// USB 操作记录编解码
int NetEncodeUsbLog(char *pBuf, const int &nLen, DbUsbLog *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	if (pPara->nActInsert)
		*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	else
		*(WORD*)p = (WORD)NET_MSG_USB_REMOVE;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//wid
	memcpy(p, (char *)&pPara->nWid, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//cDrive
	*p++ = pPara->cDrive;
	//nType
	*p++ = pPara->nType;
	//nActInsert
	*p++ = pPara->nActInsert;
	//tmEvent
	memcpy(p, (char *)&pPara->tmEvent, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwFSFlags
	memcpy(p, (char *)&pPara->dwFSFlags, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwSerialNum
	memcpy(p, (char *)&pPara->dwSerialNum, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//fs name len
	*p++ = strlen(pPara->szFSName);
	memcpy(p, pPara->szFSName, strlen(pPara->szFSName));
	p += strlen(pPara->szFSName);
	//szVolume len
	*p++ = strlen(pPara->szVolume);
	memcpy(p, pPara->szVolume, strlen(pPara->szVolume));
	p += strlen(pPara->szVolume);
	//szFriendName len
	*p++ = strlen(pPara->szFriendName);
	memcpy(p, pPara->szFriendName, strlen(pPara->szFriendName));
	p += strlen(pPara->szFriendName);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeUsbLog(char *pBuf, const int &nLen, DbUsbLog *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(DbUsbLog));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//nWid
	memcpy((char *)&pPara->nWid, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//cDrive
	pPara->cDrive = *p++;
	//nType
	pPara->nType = *p++;
	//nActInsert
	pPara->nActInsert = *p++;
	//tmEvent
	memcpy((char *)&pPara->tmEvent, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwFSFlags
	memcpy((char *)&pPara->dwFSFlags, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwSerialNum
	memcpy((char *)&pPara->dwSerialNum, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//szFSName len
	wLen = *p++ ;
	memcpy(pPara->szFSName, p, wLen);
	p += wLen;
	//szVolume len
	wLen = *p++ ;
	memcpy(pPara->szVolume, p, wLen);
	p += wLen;
	//szFriendName len
	wLen = *p++ ;
	memcpy(pPara->szFriendName, p, wLen);
	p += wLen;

	return (p - pBuf);
}

//////////////////////////////////////////////////////////////////////////
// 上报告警事件编解码
int NetEncodeAlarmEvent(char *pBuf, const int &nLen, DbAlarmEvent *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_ALARM_EVENT;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//wid
	memcpy(p, (char *)&pPara->nWid, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nTime
	memcpy(p, (char *)&pPara->nTime, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nAlarmType
	*p++ = pPara->nAlarmType;
	//nAlarmLevel
	*p++ = pPara->nAlarmLevel;
	//bDiskType
	*p++ = pPara->bDiskType;
	//strDocExt len
	*p++ = strlen(pPara->strDocExt);
	memcpy(p, pPara->strDocExt, strlen(pPara->strDocExt));
	p += strlen(pPara->strDocExt);
	//strKey len
	*p++ = strlen(pPara->strKey);
	memcpy(p, pPara->strKey, strlen(pPara->strKey));
	p += strlen(pPara->strKey);
	//strFile len
	*p++ = strlen(pPara->strFile);
	memcpy(p, pPara->strFile, strlen(pPara->strFile));
	p += strlen(pPara->strFile);
	//strContent len
	*p++ = strlen(pPara->strContent);
	memcpy(p, pPara->strContent, strlen(pPara->strContent));
	p += strlen(pPara->strContent);
	//strDes len
	*p++ = strlen(pPara->strDes);
	memcpy(p, pPara->strDes, strlen(pPara->strDes));
	p += strlen(pPara->strDes);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeAlarmEvent(char *pBuf, const int &nLen, DbAlarmEvent *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(DbAlarmEvent));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//nWid
	memcpy((char *)&pPara->nWid, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nTime
	memcpy((char *)&pPara->nTime, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nAlarmType
	pPara->nAlarmType = *p++;
	//nAlarmLevel
	pPara->nAlarmLevel = *p++;
	//bDiskType
	pPara->bDiskType = *p++;
	//strDocExt len
	wLen = *p++ ;
	memcpy_s(pPara->strDocExt, sizeof(pPara->strDocExt)-1, p, wLen);
	p += wLen;
	//strKey len
	wLen = *p++ ;
	memcpy_s(pPara->strKey, sizeof(pPara->strKey)-1, p, wLen);
	p += wLen;
	//strFile len
	wLen = *p++ ;
	memcpy_s(pPara->strFile, sizeof(pPara->strFile)-1, p, wLen);
	p += wLen;
	//strContent len
	wLen = *p++ ;
	memcpy_s(pPara->strContent, sizeof(pPara->strContent)-1, p, wLen);
	p += wLen;
	//strDes len
	wLen = *p++ ;
	memcpy_s(pPara->strDes, sizeof(pPara->strDes)-1, p, wLen);
	p += wLen;

	return (p - pBuf);
}
//////////////////////////////////////////////////////////////////////////
// 上报告警事件编解码
int NetEncodeAlarmList(char *pBuf, const int &nLen, FmSrchRpt *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	unsigned char *p = (unsigned char *)pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_ALARM_LIST;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//wid
	//memcpy(p, (char *)&pPara->nWid, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	//p += NET_TIME_LEN;
	//dwChangeTime
	memcpy(p, (char *)&pPara->dwChangeTime, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwFileSize
	memcpy(p, (char *)&pPara->dwFileSize, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//tFileCreate
	memcpy(p, (char *)&pPara->tFileCreate, sizeof(pPara->tFileCreate)); //fixed 4 bytes whichever operation system
	p += sizeof(pPara->tFileCreate);
	//nAlarmType
	*p++ = pPara->nFileChangeType;
	//bDiskType
	*p++ = pPara->bDiskType;
	//bReserved
	*p++ = pPara->bReserved;
	
	//bDiskType
	//*p++ = pPara->bDiskType;
	//strDocExt len
	//*p++ = strlen(pPara->strDocExt);
	//memcpy(p, pPara->strDocExt, strlen(pPara->strDocExt));
	//p += strlen(pPara->strDocExt);
	//strFile len

	//不能使用BYTE, MAX_PATH=260 ->越界!!!!!
	*(WORD*)p = strlen(pPara->szScanFile);
	p+=2;
//	*p++ = strlen(pPara->szScanFile);
	memcpy(p, pPara->szScanFile, strlen(pPara->szScanFile));
	p += strlen(pPara->szScanFile);

	//nItemN
	memcpy(p, (char *)&pPara->nItemN, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;

	//
	for (int i=0;i<pPara->nItemN && i< COMM_SRCH_MAXSAMEKEY;i++)
	{
		//nAlarmLevel
		*p++ = pPara->tItems[i].nLevel;
		//nFilePos
		memcpy(p, (char *)&pPara->tItems[i].nFilePos, NET_TIME_LEN); //fixed 4 bytes whichever operation system
		p += NET_TIME_LEN;
		//strKey len
		*p++ = strlen(pPara->tItems[i].strKey);
		memcpy(p, pPara->tItems[i].strKey, strlen(pPara->tItems[i].strKey));
		p += strlen(pPara->tItems[i].strKey);
		//strContent len
		*p++ = strlen(pPara->tItems[i].strContent);
		memcpy(p, pPara->tItems[i].strContent, strlen(pPara->tItems[i].strContent));
		p += strlen(pPara->tItems[i].strContent);
		//strDes len
		//*p++ = strlen(pPara->strDes);
		//memcpy(p, pPara->strDes, strlen(pPara->strDes));
		//p += strlen(pPara->strDes);
	}

	//fill the msg len
	WORD wLen = p-(unsigned char *)pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-(unsigned char *)pBuf);
}
int NetDecodeAlarmList(char *pBuf, const int &nLen, FmSrchRpt *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(FmSrchRpt));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	unsigned char *p = (unsigned char *)pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//nWid
	//memcpy((char *)&pPara->nWid, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	//p += NET_TIME_LEN;
	//dwChangeTime
	memcpy((char *)&pPara->dwChangeTime, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwFileSize
	memcpy((char *)&pPara->dwFileSize, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//tFileCreate
	memcpy((char *)&pPara->tFileCreate, p, sizeof(pPara->tFileCreate)); 
	p += sizeof(pPara->tFileCreate);
	//nAlarmType
	pPara->nFileChangeType = *p++;
	//bDiskType
	pPara->bDiskType = *p++;
	//bReserved
	pPara->bReserved = *p++;
	//strDocExt len
	//wLen = *p++ ;
	//memcpy(pPara->strDocExt, p, wLen);
	//p += wLen;
	//strFile len
	wLen = *(WORD*)p ;
	p+=2;
	memcpy_s(pPara->szScanFile, sizeof(pPara->szScanFile)-1, (char*)p, wLen);
	p += wLen;
	
	//nItemN
	memcpy((char *)&pPara->nItemN, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	if (pPara->nItemN > COMM_SRCH_MAXSAMEKEY)
	{
		pPara->nItemN = COMM_SRCH_MAXSAMEKEY;
	}
	p += NET_TIME_LEN;
	for (int i=0;i<pPara->nItemN && i< COMM_SRCH_MAXSAMEKEY;i++)
	{
		//nAlarmLevel
		pPara->tItems[i].nLevel = *p++;
		//nFilePos
		memcpy((char *)&pPara->tItems[i].nFilePos, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
		p += NET_TIME_LEN;
		//strKey len
		wLen = *p++ ;
		memcpy_s(pPara->tItems[i].strKey, COMM_SRCH_KEY_LEN-1, (char*)p, wLen);
		p += wLen;
		//strContent len
		wLen = *p++ ;
		memcpy_s(pPara->tItems[i].strContent, COMM_SRCH_CONTENT_LEN-1, (char*)p, wLen);
		p += wLen;
		//strDes len
		//wLen = *p++ ;
		//memcpy(pPara->strDes, p, wLen);
		//p += wLen;
	}

	return (p - (unsigned char *)pBuf);
}
//////////////////////////////////////////////////////////////////////////
// 上报告警事件编解码
int NetEncodeNetAlarm(char *pBuf, const int &nLen, NetmSrchRpt *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	unsigned char *p = (unsigned char *)pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_ALARM_NET;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;

	//stTime
	memcpy(p, (char *)&pPara->stTime, sizeof(pPara->stTime)); //fixed 4 bytes whichever operation system
	p += sizeof(pPara->stTime);
	//uCmdType
	memcpy(p, (char *)&pPara->uCmdType, sizeof(UINT16)); 
	p += sizeof(UINT16);
	//uCmdFlag
	memcpy(p, (char *)&pPara->uCmdFlag, sizeof(UINT16)); 
	p += sizeof(UINT16);
	//uDstIpPort
	memcpy(p, (char *)&pPara->uDstIpPort, sizeof(UINT16)); 
	p += sizeof(UINT16);
	//uSrcIpPort
	memcpy(p, (char *)&pPara->uSrcIpPort, sizeof(UINT16)); 
	p += sizeof(UINT16);
	//ulDstIpAddress
	memcpy(p, (char *)&pPara->ulDstIpAddress, sizeof(ULONG32)); 
	p += sizeof(ULONG32);
	//ulSrcIpAddress
	memcpy(p, (char *)&pPara->ulSrcIpAddress, sizeof(ULONG32)); 
	p += sizeof(ULONG32);
	
	//nItemSum
	memcpy(p, (char *)&pPara->nItemSum, sizeof(UINT16)); 
	p += sizeof(UINT16);
	
	//szDstAddress
	*p++ = strlen(pPara->szDstAddress);
	memcpy(p, pPara->szDstAddress, strlen(pPara->szDstAddress));
	p += strlen(pPara->szDstAddress);
	//szSrcAddress
	*p++ = strlen(pPara->szSrcAddress);
	memcpy(p, pPara->szSrcAddress, strlen(pPara->szSrcAddress));
	p += strlen(pPara->szSrcAddress);

	//
	for (UINT16 i=0;i<pPara->nItemSum;i++)
	{
		//uLevel -->BYTE !!!!!!!!!
		*p++ = pPara->tItems[i].uLevel;
		//nFilePos
		memcpy(p, (char *)&pPara->tItems[i].nPos, sizeof(pPara->tItems[i].nPos)); 
		p += sizeof(pPara->tItems[i].nPos);
		//szKey len
		*p++ = strlen(pPara->tItems[i].szKey);
		memcpy(p, pPara->tItems[i].szKey, strlen(pPara->tItems[i].szKey));
		p += strlen(pPara->tItems[i].szKey);
		//szContent len
		*p++ = strlen(pPara->tItems[i].szContent);
		memcpy(p, pPara->tItems[i].szContent, strlen(pPara->tItems[i].szContent));
		p += strlen(pPara->tItems[i].szContent);
	}

	//fill the msg len
	WORD wLen = p-(unsigned char *)pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-(unsigned char *)pBuf);
}
int NetDecodeNetAlarm(char *pBuf, const int &nLen, NetmSrchRpt *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(*pPara));
	
	WORD wLen = 0;

	unsigned char *p = (unsigned char *)pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;

	//stTime
	memcpy((char *)&pPara->stTime, p, sizeof(pPara->stTime)); //fixed 4 bytes whichever operation system
	p += sizeof(pPara->stTime);
	//uCmdType
	memcpy((char *)&pPara->uCmdType, p, sizeof(pPara->uCmdType)); 
	p += sizeof(pPara->uCmdType);
	//uCmdFlag
	memcpy((char *)&pPara->uCmdFlag, p, sizeof(pPara->uCmdFlag)); 
	p += sizeof(pPara->uCmdFlag);
	//uDstIpPort
	memcpy((char *)&pPara->uDstIpPort, p, sizeof(pPara->uDstIpPort)); 
	p += sizeof(pPara->uDstIpPort);
	//uSrcIpPort
	memcpy((char *)&pPara->uSrcIpPort, p, sizeof(pPara->uSrcIpPort)); 
	p += sizeof(pPara->uSrcIpPort);
	//ulDstIpAddress
	memcpy((char *)&pPara->ulDstIpAddress, p, sizeof(pPara->ulDstIpAddress)); 
	p += sizeof(pPara->ulDstIpAddress);
	//ulSrcIpAddress
	memcpy((char *)&pPara->ulSrcIpAddress, p, sizeof(pPara->ulSrcIpAddress)); 
	p += sizeof(pPara->ulSrcIpAddress);

	//nItemSum
	memcpy((char *)&pPara->nItemSum, p, sizeof(pPara->nItemSum)); 
	p += sizeof(pPara->nItemSum);

	//szDstAddress len
	wLen = *p++ ;
	memcpy_s(pPara->szDstAddress, NF_ADDRESS_LENGTH-1, (char*)p, wLen);
	p += wLen;
	//szSrcAddress len
	wLen = *p++ ;
	memcpy_s(pPara->szSrcAddress, NF_ADDRESS_LENGTH-1, (char*)p, wLen);
	p += wLen;
	
	for (UINT16 i=0;i<pPara->nItemSum && i<COMM_SRCH_MAXSAMEKEY;i++)
	{
		//uLevel -->BYTE !!!!!!!!!
		pPara->tItems[i].uLevel = *p++;
		//nPos
		memcpy((char *)&pPara->tItems[i].nPos, p, sizeof(pPara->tItems[i].nPos)); //fixed 4 bytes whichever operation system
		p += sizeof(pPara->tItems[i].nPos);
		//szKey len
		wLen = *p++ ;
		memcpy_s(pPara->tItems[i].szKey, COMM_SRCH_KEY_LEN-1, (char*)p, wLen);
		p += wLen;
		//szContent len
		wLen = *p++ ;
		memcpy_s(pPara->tItems[i].szContent, COMM_SRCH_CONTENT_LEN-1, (char*)p, wLen);
		p += wLen;
	}

	return (p - (unsigned char *)pBuf);
}
//////////////////////////////////////////////////////////////////////////
// 上报告警事件编解码
int NetEncodeRegAlarm(char *pBuf, const int &nLen, RegMonRpt *pPara, const int &nItemN)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	unsigned char *p = (unsigned char *)pBuf;
	unsigned char *pBase ;
	for (int i=0;i<nItemN && i< COMM_SRCH_MAXSAMEKEY;i++)
	{
		pBase = p;
		//message type
		*(WORD*)p = (WORD)NET_MSG_ALARM_REG;
		p += NET_MSGTYPE_LEN;
		//length
		p += NET_CTNT_LEN;

		//stTime
		memcpy(p, (char *)&pPara->stTime, sizeof(pPara->stTime)); //fixed 4 bytes whichever operation system
		p += sizeof(pPara->stTime);
		//uLevel
		memcpy(p, (char *)&pPara->uLevel, sizeof(UINT16)); 
		p += sizeof(UINT16);
		//uCmdType
		memcpy(p, (char *)&pPara->uCmdType, sizeof(UINT16)); 
		p += sizeof(UINT16);
		//uCmdRet
		memcpy(p, (char *)&pPara->uCmdRet, sizeof(UINT16)); 
		p += sizeof(UINT16);
		
		
		//szExePath
		*p++ = strlen(pPara->szExePath);
		memcpy(p, pPara->szExePath, strlen(pPara->szExePath));
		p += strlen(pPara->szExePath);
		
		//szRegPath - 这个可能会超过256, 故用WORD保存长度
		*(WORD*)p = strlen(pPara->szRegPath);
		p += sizeof(WORD);
		memcpy(p, pPara->szRegPath, strlen(pPara->szRegPath));
		p += strlen(pPara->szRegPath);
		//szRegValue - 这个可能会超过256, 故用WORD保存长度
		*(WORD*)p = strlen(pPara->szRegValue);
		p += sizeof(WORD);
		memcpy(p, pPara->szRegValue, strlen(pPara->szRegValue));
		p += strlen(pPara->szRegValue);

		//fill the msg len
		WORD wLen = p-(unsigned char *)pBase;
		*(WORD*)(pBase+NET_MSGTYPE_LEN) = wLen;
	}
	return (p-(unsigned char *)pBuf);
}
int NetDecodeRegAlarm(char *pBuf, const int &nLen, RegMonRpt *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(*pPara));
	
	WORD wLen = 0;

	unsigned char *p = (unsigned char *)pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;

	//stTime
	memcpy((char *)&pPara->stTime, p, sizeof(pPara->stTime)); //fixed 4 bytes whichever operation system
	p += sizeof(pPara->stTime);
	//uLevel
	memcpy((char *)&pPara->uLevel, p, sizeof(pPara->uLevel)); 
	p += sizeof(pPara->uLevel);
	//uCmdType
	memcpy((char *)&pPara->uCmdType, p, sizeof(pPara->uCmdType)); 
	p += sizeof(pPara->uCmdType);
	//uCmdRet
	memcpy((char *)&pPara->uCmdRet, p, sizeof(pPara->uCmdRet)); 
	p += sizeof(pPara->uCmdRet);

	//szExePath len
	wLen = *p++ ;
	memcpy_s(pPara->szExePath, sizeof(pPara->szExePath)-1, (char*)p, wLen);
	p += wLen;
	//szRegPath len - 这个可能会超过256, 故用WORD保存长度
	wLen = *(WORD *)p ;
	p += sizeof(WORD);
	memcpy_s(pPara->szRegPath,sizeof(pPara->szRegPath)-1, (char*)p, wLen);
	p += wLen;
	//szRegValue len - 这个可能会超过256, 故用WORD保存长度
	wLen = *(WORD *)p ;
	p += sizeof(WORD);
	memcpy_s(pPara->szRegValue,sizeof(pPara->szRegValue)-1, (char*)p, wLen);
	p += wLen;
	
	return (p - (unsigned char *)pBuf);
}
int NetEncodeFlchgAlarm(char *pBuf, const int &nLen, FileMonSys *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_ALARM_FILECHG;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;

	//拷贝整个结构体,但不包括多余的空字符串
	memcpy(p, pPara, sizeof(FileMonSys) - sizeof(pPara->szFilePath) + strlen(pPara->szFilePath));	
	p+=sizeof(FileMonSys) - sizeof(pPara->szFilePath) + strlen(pPara->szFilePath);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeFlchgAlarm(char *pBuf, const int &nLen, FileMonSys *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara->szFilePath, 0, sizeof(pPara->szFilePath));
	
	char *p = pBuf;
	WORD wLen = *(WORD*)p;
	if (wLen < NET_MSGTYPE_LEN + NET_CTNT_LEN)
		return NET_CTNT_LEN;
	wLen -= (NET_MSGTYPE_LEN + NET_CTNT_LEN);	//content length
	//length
	p += NET_CTNT_LEN;

	//解码需要保护?????
	memcpy((char *)pPara, p, wLen); 
	p += wLen;

	return (p - pBuf);
}
int NetEncodeFRegPolicy(char *pBuf, const int &nLen, DbWarnLvStr *pPara)
{
	return NetEncodeDbLvlStr(pBuf, NET_MSG_REG_MON_PLY, pPara);
}
int NetDecodeFRegPolicy(char *pBuf, const int &nLen, DbWarnLvStr *pPara)
{
	return NetDecodeDbLvlStr(pBuf, nLen, &pPara);
}
int NetEncodeCustMsg(char *pBuf, const DWORD32 &nLen, const WORD &wMsg, 
		const char *pCustMsg, const DWORD32 & dwCustLen )
{
	if (pBuf == NULL || nLen <= 0 || dwCustLen +NET_MSGTYPE_LEN+NET_CTNT_LEN> nLen)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)wMsg;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	if (dwCustLen > 0 && pCustMsg)
	{
		memcpy(p, pCustMsg, dwCustLen);
	}
	
	//fill the msg len
	//WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = (WORD)(dwCustLen +NET_MSGTYPE_LEN+NET_CTNT_LEN);
	return (p-pBuf);
}
int NetDecodeCustMsg(char *pBuf, const int &nLen, void *pPara, const DWORD32 &dwLen)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0 || dwLen <= 0)
		return 0;

	WORD wLen = *(WORD*)pBuf;
	if (wLen <  NET_CTNT_LEN + dwLen)
		return 0;
	//解码需要保护?????
	memcpy((char *)pPara, pBuf+NET_CTNT_LEN, dwLen); 

	return (dwLen + NET_CTNT_LEN);
}
int NetEncodeCommand(char *pBuf, const int &nLen, const WORD &wMsg, const DWORD32 &dwMID, const int & wAttach )
{
	if (pBuf == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)wMsg;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, &dwMID, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nSubID
	memset(p, 0, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//wAttach === 传入的是INT ，实际写入的WORD ，严格依赖于CPU的字节序??????????
	memcpy(p, &wAttach, 2); 
	p += 2;
	
	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}

//int NetDecodeCommand(char *pBuf, const int &nLen, MsgTransFile *pPara)
int NetEncodeTransFile(char *pBuf, const int &nLen, MsgTransFile *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	unsigned char *p = (unsigned char *)pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_UP_FILE;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, (char *)&pPara->nID, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy(p, (char *)&pPara->nSubID, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//dwFileSize
	memcpy(p, (char *)&pPara->dwFileSize, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//strUpFile len
	*p++ = strlen(pPara->strUpFile);
	if (strlen(pPara->strUpFile) > 0)
	{
		memcpy(p, pPara->strUpFile, strlen(pPara->strUpFile));
		p += strlen(pPara->strUpFile);
	}
	//fill the msg len
	WORD wLen = p-(unsigned char *)pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-(unsigned char *)pBuf);
}
int NetDecodeTransFile(char *pBuf, const int &nLen, MsgTransFile *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(MsgTransFile));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	unsigned char *p = (unsigned char *)pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//id
	memcpy((char *)&pPara->nID, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy((char *)&pPara->nSubID, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//dwFileSize
	memcpy((char *)&pPara->dwFileSize, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//strUpFile name len
	wLen = *p++ ;
	if (wLen > 0)
	{
		memcpy_s(pPara->strUpFile, sizeof(pPara->strUpFile)-1, (char*)p, wLen);
		p += wLen;
	}
	return (p - (unsigned char *)pBuf);
}
int NetEncodeSendFile(char *pBuf, const int &nLen, MsgSendFile *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	unsigned char *p = (unsigned char *)pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_SEND_FILE;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, (char *)&pPara->nID, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nPeerTid
	memcpy(p, (char *)&pPara->nPeerTid, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwFileSize
	memcpy(p, (char *)&pPara->dwFileSize, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//strUpFile len
	*p++ = strlen(pPara->strUpFile);
	if (strlen(pPara->strUpFile) > 0)
	{
		memcpy(p, pPara->strUpFile, strlen(pPara->strUpFile));
		p += strlen(pPara->strUpFile);
	}
	//fill the msg len
	WORD wLen = p-(unsigned char *)pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-(unsigned char *)pBuf);
}
int NetDecodeSendFile(char *pBuf, const int &nLen, MsgSendFile *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(MsgSendFile));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	unsigned char *p = (unsigned char *)pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//id
	memcpy((char *)&pPara->nID, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nPeerTid
	memcpy((char *)&pPara->nPeerTid, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//dwFileSize
	memcpy((char *)&pPara->dwFileSize, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//strUpFile name len
	wLen = *p++ ;
	if (wLen > 0)
	{
		memcpy(pPara->strUpFile, p, wLen);
		p += wLen;
	}
	return (p - (unsigned char *)pBuf);
}
int NetEncodeFileChunk(char *pBuf, const int &nLen, MsgFileChunk *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_TFILE_CHUNK;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, (char *)&pPara->nID, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy(p, (char *)&pPara->nSubID, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nSeq
	memcpy(p, (char *)&pPara->nSeq, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nFilePos
	memcpy(p, (char *)&pPara->nFilePos, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nFileLen
	memcpy(p, (char *)&pPara->nFileLen, NET_TIME_LEN); 
	p += NET_TIME_LEN;

	//strContent
	memcpy(p, pPara->strContent, pPara->nFileLen);
	p += pPara->nFileLen;

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeFileChunk(char *pBuf, const int &nLen, MsgFileChunk *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(MsgFileChunk));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;

	//内存边界检查
	if (wLen < 2 || wLen -2 > nLen)
		return 0;
	//id
	memcpy((char *)&pPara->nID, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy((char *)&pPara->nSubID, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nSeq
	memcpy((char *)&pPara->nSeq, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nFilePos
	memcpy((char *)&pPara->nFilePos, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nFileLen
	memcpy((char *)&pPara->nFileLen, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;

	//strContent
	wLen = pPara->nFileLen ;
	memcpy_s(pPara->strContent, sizeof(pPara->strContent), p, wLen);
	p += wLen;

	return (p - pBuf);
}
int NetEncodeSnapScrn(char *pBuf, const int &nLen, MsgTransFile *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_SNAP_SCRN;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, (char *)&pPara->nID, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy(p, (char *)&pPara->nSubID, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//dwFileSize
	memcpy(p, (char *)&pPara->dwFileSize, NET_TIME_LEN); 
	p += NET_TIME_LEN;

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeSnapScrn(char *pBuf, const int &nLen, MsgTransFile *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(MsgTransFile));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//id
	memcpy((char *)&pPara->nID, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy((char *)&pPara->nSubID, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//dwFileSize
	memcpy((char *)&pPara->dwFileSize, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;

	return (p - pBuf);
}
int NetEncodeSetUsrPwd(char *pBuf, const int &nLen, MsgSetUsrPwd *pPara)
{
	if (pBuf == NULL || pPara == NULL )
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_USR_SET_PWD;
	p += NET_MSGTYPE_LEN;
	//length
	*(WORD*)p = (WORD)sizeof(MsgSetUsrPwd)+NET_MSGTYPE_LEN+NET_CTNT_LEN;
	p += NET_CTNT_LEN;
	
	memcpy(p, pPara, sizeof(MsgSetUsrPwd));

	return sizeof(MsgSetUsrPwd)+NET_MSGTYPE_LEN+NET_CTNT_LEN;
}
int NetDecodeSetUsrPwd(char *pBuf, const int &nLen, MsgSetUsrPwd *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	char *p = pBuf;
	//length
	WORD wLen = *(WORD*)p ;	//包含自身message type and message len - at least 4
	p += NET_CTNT_LEN;
	
	//waOffset
	memcpy(pPara, p, sizeof(MsgSetUsrPwd));
	p+=sizeof(MsgSetUsrPwd);
	pPara->strOldPwd[NET_MAX_USER_NAME - 1] = 0;
	pPara->strNewPwd[NET_MAX_USER_NAME - 1] = 0;

	return (p-pBuf);

}
int NetEncodeDskSc(char *pBuf, const int &nLen, DbTaskScanDisk *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	unsigned char *p = (unsigned char *)pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_UP_FILE;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, (char *)&pPara->nID, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy(p, (char *)&pPara->nSubID, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//strDir len
	//strDir - 这个可能会超过256, 故用WORD保存长度
	*(WORD*)p = strlen(pPara->strDir);
	p += sizeof(WORD);
	memcpy(p, pPara->strDir, strlen(pPara->strDir));
	p += strlen(pPara->strDir);
	//strKeys - 这个可能会超过256, 故用WORD保存长度

	//modify by cy   -------------------------
	*(WORD*)p = sizeof(pPara->strKeys)-1;
	p += sizeof(WORD);
	memcpy(p, pPara->strKeys, sizeof(pPara->strKeys)-1);
	p += sizeof(pPara->strKeys)-1;
	//------------------------------------------------------

	//fill the msg len
	WORD wLen = p-(unsigned char *)pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-(unsigned char *)pBuf);
}
int NetDecodeDskSc(char *pBuf, const int &nLen, DbTaskScanDisk *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(DbTaskScanDisk));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	unsigned char *p = (unsigned char *)pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//id
	memcpy((char *)&pPara->nID, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy((char *)&pPara->nSubID, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	
	//strDir len - 这个可能会超过256, 故用WORD保存长度
	wLen = *(WORD *)p ;
	p += sizeof(WORD);
	memcpy_s(pPara->strDir, sizeof(pPara->strDir)-1, (char*)p, wLen);
	p += wLen;
	//strKeys len - 这个可能会超过256, 故用WORD保存长度
	wLen = *(WORD *)p ;
	p += sizeof(WORD);
	memcpy_s(pPara->strKeys, sizeof(pPara->strKeys)-1, (char*)p, wLen);
	p += wLen;

	return (p - (unsigned char *)pBuf);
}
int NetEncodeErrNtfy(char *pBuf, const int &nLen, MsgErrNtfy *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_ERR_NTFY;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//id
	memcpy(p, (char *)&pPara->nID, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwTid
	memcpy(p, (char *)&pPara->dwTid, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//dwErr
	memcpy(p, (char *)&pPara->dwErr, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//strMsg len
	//strMsg - BYTE保存长度
	*p++ = (BYTE)strlen(pPara->strMsg);
	memcpy(p, pPara->strMsg, (BYTE)strlen(pPara->strMsg));
	p+= (BYTE)strlen(pPara->strMsg);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeErrNtfy(char *pBuf, const int &nLen, MsgErrNtfy *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(MsgErrNtfy));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//id
	memcpy((char *)&pPara->nID, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//dwTid
	memcpy((char *)&pPara->dwTid, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//dwErr
	memcpy((char *)&pPara->dwErr, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;

	wLen = (BYTE)*p++;
	memcpy_s(pPara->strMsg, sizeof(pPara->strMsg)-1, p, wLen);
	p += wLen;

	return (p - pBuf);
}
int NetEncodeAck(char *pBuf, const int &nLen, MsgAck *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;
	
	char *p = pBuf;
	//message type
	*(WORD*)p = (WORD)NET_MSG_ACK;
	p += NET_MSGTYPE_LEN;
	//length
	p += NET_CTNT_LEN;
	//nPeerID
	memcpy(p, (char *)&pPara->nPeerID, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy(p, (char *)&pPara->nSubID, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nMsgSeq
	memcpy(p, (char *)&pPara->nMsgSeq, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nStatus
	memcpy(p, (char *)&pPara->nStatus, sizeof(pPara->nStatus)); 
	p += sizeof(pPara->nStatus);
	//wMsgType
	memcpy(p, (char *)&pPara->wMsgType, sizeof(pPara->wMsgType)); 
	p += sizeof(pPara->wMsgType);

	//fill the msg len
	WORD wLen = p-pBuf;
	*(WORD*)(pBuf+NET_MSGTYPE_LEN) = wLen;
	return (p-pBuf);
}
int NetDecodeAck(char *pBuf, const int &nLen, MsgAck *pPara)
{
	if (pBuf == NULL || pPara == NULL || nLen <= 0)
		return 0;

	memset(pPara, 0, sizeof(MsgAck));
	
	WORD wLen = 0;
	//message type
	//*(WORD*)p = (WORD)NET_MSG_USB_INSERT;
	//p += NET_MSGTYPE_LEN;
	char *p = pBuf;
	//length
	wLen = *(WORD*)p ;
	p += NET_CTNT_LEN;
	//nPeerID
	memcpy((char *)&pPara->nPeerID, p, NET_TIME_LEN); //fixed 4 bytes whichever operation system
	p += NET_TIME_LEN;
	//nSubID
	memcpy((char *)&pPara->nSubID, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nMsgSeq
	memcpy((char *)&pPara->nMsgSeq, p, NET_TIME_LEN); 
	p += NET_TIME_LEN;
	//nStatus
	memcpy((char *)&pPara->nStatus, p, sizeof(pPara->nStatus)); 
	p += sizeof(pPara->nStatus);
	//wMsgType
	memcpy((char *)&pPara->wMsgType, p, sizeof(pPara->wMsgType)); 
	p += sizeof(pPara->wMsgType);

	return (p - pBuf);
}
int NetEncodePacket(const int &nMsgType, char *pBuf, const int &nLen, void *pPara)
{
	return 0;
}

//[Return Value]
//	- 0 Failure
//	> 0 Decoded length
//[Parameter]
//	pBuf	:	buffer to be decoded
//	nLen :	buffer length
//	wMsg : == 0 normal decode
//			> 0 the content must be the wMsg type, otherwise failure

int NetDecodePacket(char *pBuf, const int &nLen, WORD &wMsg, void *pPara)
{
	//if wMsg>0, means the next message type must also be the message
	if (wMsg > 0 && wMsg != *(WORD*)pBuf)
		return 0;
		
	//对于服务器发送出去的报文
	// 1.SystemFlag 2.EncrypeFlag 3.CompressFlag 4.MessageType 5.Len 6.Value
	wMsg = *(WORD*)pBuf;
	switch (wMsg)
	{
	case NET_MSG_AUTH_REQ:
		return NetDecodeAuth(pBuf + 2, nLen - 2, (NetMsgAuth*)pPara);
		break;
	default:
		break;
	}
	return 1;
}
int NetEncryptData(char *pBuf, const int &nLen, char *pOut)
{
	for (int i=0;i<nLen; i++)
		pBuf[i] = ~pBuf[i];
	return 0;
}
int NetDecryptData(char *pBuf, const int &nLen, char *pOut)
{
	for (int i=0;i<nLen; i++)
		pBuf[i] = ~pBuf[i];
	return 0;
}
int NetZipData(char *pBuf, const int &nLen, char *pOut)
{
	return 0;
}
int NetUnZipData(char *pBuf, const int &nLen, char *pOut)
{
	return 0;
}
/*
int HandleIncoming(char *pBuf, DWORD &dwTrans, NetSession *pSession)
{
	if (pBuf == NULL || dwTrans == 0 || pSession == NULL)
	{
		//log err
		return 1;
	}
	return 0;
}
*/

int UtlAllocMem(DbWarnLvStr **ptr,  WORD wAllocLen)
{
	if (ptr == NULL || wAllocLen == 0)
		return 0;
	if (*ptr && (*ptr)->wLen >= wAllocLen + sizeof(DbWarnLvStr))
		return 1;

	if (*ptr )
	{
		delete [](char *)(*ptr);
	}
	 //额外多申请一部分
	*ptr = (DbWarnLvStr*) new char [sizeof(DbWarnLvStr) + wAllocLen + 128];
	if (*ptr == NULL)
		return 0;
	memset(*ptr, 0, sizeof(DbWarnLvStr) + wAllocLen + 128);
	(*ptr)->wLen = sizeof(DbWarnLvStr) + wAllocLen + 128;
	return 2;
}
void UtlFreeMem(DbWarnLvStr *ptr)
{
	if (ptr)
	{
		delete [](char *)(ptr);
	}
}

#if !defined(_WIN32) || _MSC_VER<=1200

int strcpy_s(char *_DEST, size_t _SIZE, const char *_SRC)
{
	if (_DEST == NULL || _SRC == NULL || _SIZE == 0)
		return -1;
	char *p;
    size_t available;

    /* validation section */
    //_VALIDATE_STRING(_DEST, _SIZE);
    //_VALIDATE_POINTER_RESET_STRING(_SRC, _DEST, _SIZE);

    p = _DEST;
    available = _SIZE;
    while ((*p++ = *_SRC++) != 0 && --available > 0)
    {
    }

    if (available == 0)
    {
        //_RESET_STRING(_DEST, _SIZE);
        //_RETURN_BUFFER_TOO_SMALL(_DEST, _SIZE);
        _DEST[_SIZE-1] = 0;
        return 2;
    }
    //_FILL_STRING(_DEST, _SIZE, _SIZE - available + 1);
    //_RETURN_NO_ERROR;
    return 0;
}
int strcat_s(char *_DEST, size_t _SIZE, const char *_SRC)
{
	if (_DEST == NULL || _SRC == NULL || _SIZE == 0)
		return -1;
    char *p;
    size_t available;

    /* validation section */
    //_VALIDATE_STRING(_DEST, _SIZE);
    //_VALIDATE_POINTER_RESET_STRING(_SRC, _DEST, _SIZE);

    p = _DEST;
    available = _SIZE;
    while (available > 0 && *p != 0)
    {
        p++;
        available--;
    }

    if (available == 0)
    {
        //_RESET_STRING(_DEST, _SIZE);
        //_RETURN_DEST_NOT_NULL_TERMINATED(_DEST, _SIZE);
        return 1;
    }

    while ((*p++ = *_SRC++) != 0 && --available > 0)
    {
    }

    if (available == 0)
    {
        //_RESET_STRING(_DEST, _SIZE);
        //_RETURN_BUFFER_TOO_SMALL(_DEST, _SIZE);
        _DEST[_SIZE-1] = 0;
        return 2;
    }
    //_FILL_STRING(_DEST, _SIZE, _SIZE - available + 1);
    //_RETURN_NO_ERROR;
    return 0;
}


int memcpy_s(char *Dst, int nDstSize, const char *Src, int nCpyLen)
{
	if (nCpyLen >= nDstSize)
		memcpy(Dst, Src, nDstSize);
	else
		memcpy(Dst, Src, nCpyLen);
    return 0;
}
#endif
//Dst 最大长度nDstSize
//Src 待拷贝长度nCpyLen
//拷贝nCpyLen到Dst，同时保证不能超过nDstSize,并且最后一个字节为0
int memcpy_ex_s(char *Dst, const DWORD &nDstSize, const char *Src, const DWORD &nCpyLen)
{
	if (nDstSize <= 1)
		return 0;
	if (nCpyLen >= nDstSize)
	{
		memcpy(Dst, Src, nDstSize-1);
		*(Dst + nDstSize-1) = 0;
	}
	else
	{
		memcpy(Dst, Src, nCpyLen);
		*(Dst + nCpyLen) = 0;
	}
    return 1;
}
