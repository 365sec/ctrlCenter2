#include "boost/atomic.hpp"
#include "CBInterDB.h"
#include "NetDef.h"
#include "NetUtlity.h"
#include "NetWorld.h"
#include <deque>
#include "both.h"

LoginManage g_login ;
//用于周期性用户信息和设备信息更新的时间戳
//SYSTEMTIME		g_tCurLocTime;
time_t  g_tCurLocTime;
//本地内存保存的文件监控信息 -> T_MonitoringFile
DbWarnLvStr *g_tDbFilesMon;
//本地内存保存的注册表监控信息 -> T_RegistrySettings
DbWarnLvStr *g_tDbRegstyMon;

#define GR_DB_MAX_KEYWORD_N		2000			//本地缓存支持的数据库最大关键字数目
#define GR_DB_MAX_DOCEXT_N		100			//本地缓存支持的数据库最大文档后缀名类型数目

//[Design:] 初始化的时候从服务器下载所有数据,包括所有关键词
//		然后周期性的刷新数据库，如果有变化,则更新本地内存
#define		DB_LOOKUP_KEYWORDS		1
#define		DB_LOOKUP_FILETYPES		2
//efine		DB_LOOKUP_KEYWORDS		3

#define		DB_VER_CHANGE_COL_NUM	8

//==================================================
typedef struct _DB_TBL_CHANGE_T
{
	INT		nWarnLevel;		//1-T_WarnLevel
	INT		nTaskType;		//2-T_TaskType
	INT		nFileVer;		//3-T_FileType
	INT		nAction;		//4-T_FileBehaviorType
	INT		nKeyVer;		//5-T_KeyWordsSet
	INT		nPolicy;		//6-T_Strategy
	INT		nRgstyVer;		//7-T_RegistrySettings
	INT		nDfmVer;		//8-T_MonitoringFile - disk file change monitor
}DbTblChange;

INT	g_tTblChange[DB_VER_CHANGE_COL_NUM];
//数据库中关键词的数目
//static int g_nDbKeyWords = 0; //For debug - no static
int g_nDbKeyWords = 0;

//本地内存保存的关键词数组
DbKeyWord g_tDbKeyWords[GR_DB_MAX_KEYWORD_N];

//数据库中监控文件类型和网络协议的数目
static int g_nDbFileType = 0;

LONG volatile g_nUsbLogMaxId = 0;
//LONG		  g_nComputerMaxId = 0;
boost::atomic<long> g_nComputerMaxId(0);

//本地内存保存的文件类型和网络协议数组
DbKeyWord g_tDbExtType[GR_DB_MAX_DOCEXT_N];

bool  CBGetWarnPol(const int &nType)            ;
bool  CBGetSrchExtPol(const int &nRefresh)      ;
bool  CBGetKeyPolicy(const int &nRefresh)       ;
bool  CBGetTmpltPolicy(const int &nRefresh)                ;
bool  CBGetFileRegMonPol(const int &nTableType) ;

//数据库中的个性化策略 T_Strategy	
std::vector<DbCfgPlcy> g_vectDbPlcy;

//=========================================================
typedef struct DB_LOG_INFO_T
{
//	INT32	nDbID;
	INT32	nPid;
	INT32	nWid;
	BYTE	bType;	//0-logoff 1-login
	BYTE	bReserved;
	WORD	wExcept;
}DbLogInfo;

typedef struct DB_INSRT_DEV_T
{
	INT32	nDbID;
//	INT32	nSsnIdx;
	char	strPid[NET_MAX_USER_PID];	//目前保存硬盘序列号或者GUID
	BYTE	baFlag[GR_AUTH_FLAG_N];		//消息包的标识,防止恶意攻击
	BYTE	baAddrType[GR_AUTH_MAXIP_N];	//IPV4 地址类型-
	DWORD32	dwIpAddr[GR_AUTH_MAXIP_N];		//最多携带4个IPV4的地址
	BYTE	baMacAddr[GR_AUTH_MAXIP_N*6];	//4*6=24 -> 4个IP地址对应的MAC地址
	char	strHost[64];					//=== 【注意】只保留64位长度
}DbInsrtDev;

#ifdef GR_FN_UPDT_DEVNAME
typedef struct DB_UPDT_COMPUTER_NAME_T
{
	INT32			nDbID;			//数据库表中的ID记录，自动增长
	char			strDevName[100];	//终端名称-数据库varchar(30)
	BYTE	baMacAddr[GR_AUTH_MAXIP_N*6];	//4*6=24 -> 4个IP地址对应的MAC地址
	char	strHost[64];					//=== 【注意】只保留64位长度
	DWORD32	dwIpAddr[GR_AUTH_MAXIP_N];
}DbUpdtDevName;
std::deque<DbUpdtDevName> g_deqUpdtDevName;
#endif	

//========================================
std::deque<DbInsrtDev> g_deqInsrtDev;
MY_CS_MUTEX g_LockInsrtDev; 

std::deque<DbLogInfo> g_deqLogInfo;
MY_CS_MUTEX g_LockLogInfo; 
LONG g_lLogProcExit = 0;
LONG g_lLogPushN = 0;
LONG g_lLogPopN = 0;

//用于描述
typedef struct _DB_PLCY_HANDLER_T
{
	WORD		nTableId;
	WORD		nNoDelay;			//一旦变动立即下发至终端- if 0 不下发至客户端
	WORD		nPreVer;			//原有版本号
	WORD		nCurVer;			//当前版本号
	INT			nFuncPara;			//handler 函数的参数
    bool   (*handler)(const int &nType);	
	INT			nMsgType;			//关联的消息类型
	VOID *		pEncodeGbl;			//关联的全局变量地址或者编码地址
	char		strTableName[32];
}DbPlcyHandler;

#define GRSVR_QUERY_PRN(string)	\
		char strQuery[1024];\
		strcpy_s(strQuery, sizeof(strQuery), string.c_str());\
		log_error(strQuery); 


DbPlcyHandler g_tDbPlcyHnd[DB_VER_CHANGE_COL_NUM] =
{
	{0, 1, 0, 0, 0, &CBGetWarnPol, 		 NET_MSG_WARN_LEVEL ,   (VOID *)&g_tDbCfgPlcy.tWarnLvl[0],  "T_WarnLevel"		},
#ifndef CFG_MDL_FLCHG_MON		
	{1, 1, 0, 0, 1, &CBGetSrchExtPol, 	 NET_MSG_EXT_POLICY ,   (VOID *)&g_tDbCfgPlcy.strFileTypes[0],  "T_FileType"		},
	{2, 1, 0, 0, 1, &CBGetKeyPolicy, 	 NET_MSG_KEY3_POLICY,   (VOID *)g_tDbCfgPlcy.tKeysPlcy,  "t_keyword_dic"	},
	{3, 0, 0, 0, 0, &CBGetTmpltPolicy, 	 NET_MSG_CFG_POLICY ,   (VOID *)&g_vectDbPlcy  ,  "T_Strategy"		},
#else		
	{1, 0, 0, 0, 0, NULL, 	 0 ,   0,  "T_FileType"		},
	{2, 1, 0, 0, 1, &CBGetKeyPolicy, 	 NET_MSG_KEY3_POLICY,   (VOID *)g_tDbCfgPlcy.tKeysPlcy,  "t_keyword_dic"	},
	{3, 0, 0, 0, 0, NULL, 	 0 ,   0,  "T_Strategy"		},
	{4, 1, 0, 0, 1, &CBGetFileRegMonPol, NET_MSG_REG_MON_PLY,   (VOID *)g_tDbRegstyMon,  "T_RegistrySettings"},
	{5, 1, 0, 0, 0, &CBGetFileRegMonPol, NET_MSG_FILE_MON_PLY,  (VOID *)g_tDbFilesMon ,  "T_MonitoringFile"	},
#endif
//	{6, 1, 0, 0, 0, &CBHandle_NULL, 	 NET_MSG_CFG_POLICY ,   (VOID *)&g_tDbCfgPlcy ,  "T_FileBehaviorType"},
//	{7, 1, 0, 0, 0, &CBHandle_NULL, 	 NET_MSG_CFG_POLICY ,   (VOID *)&g_tDbCfgPlcy ,  "T_TaskType"		},
};

bool LoginManage::InitLogInfoProc(void *param)
{
	g_lLogProcExit = 0;
	g_deqLogInfo.clear();
	MyInitCsMutex(g_LockLogInfo);
	MyInitCsMutex(g_LockInsrtDev);

	DWORD dwThreadID = 0;
	return 0;
}

void LoginManage::CBPushNewDevice(INT32 &nid, const NetMsgAuthEx2 &tAuth)
{
	DbInsrtDev dev;
	memset(&dev, 0, sizeof(dev));
	
	//dev.nDbID = (INT32)InterlockedIncrement(&g_nComputerMaxId);
	g_nComputerMaxId+=1;
	dev.nDbID = g_nComputerMaxId;
	nid = dev.nDbID;

	strcpy(dev.strPid, tAuth.strPid);
	memcpy(&dev.baFlag[0], &tAuth.baFlag[0], GR_AUTH_FLAG_N);
	memcpy(&dev.baAddrType, &tAuth.baAddrType[0], GR_AUTH_MAXIP_N);
	memcpy(&dev.dwIpAddr[0], &tAuth.dwIpAddr[0], GR_AUTH_MAXIP_N*sizeof(DWORD32));
	memcpy(&dev.baMacAddr[0], &tAuth.baMacAddr[0], GR_AUTH_MAXIP_N*6);
	if (tAuth.strHost[0])
	{
		strcpy_s(dev.strHost, sizeof(dev.strHost), tAuth.strHost);
	}

	MY_LOCK(&g_LockInsrtDev);
	g_deqInsrtDev.push_back(dev);
	MY_UNLOCK(&g_LockInsrtDev);
	
}

bool LoginManage::UnInitLogInfoProc()
{
	g_lLogProcExit = 1;
	MY_LOCK(&g_LockLogInfo);
	g_deqLogInfo.clear();
	MY_UNLOCK(&g_LockLogInfo);
	MyDeleteMutex(&g_LockLogInfo);
	MyDeleteMutex(&g_LockInsrtDev);
	return 0;
}

bool LoginManage::CBWriteInsrtDevice()
{	
	char str[2048] = {0};
	char substr[2048] = {0};

	if (g_deqInsrtDev.size() == 0){
		return false;
	}
	DbInsrtDev& info = g_deqInsrtDev.front();
	//获取IP地址列表
	//第一个IP地址写入IP字段，其它IP地址写入remark字段
	BYTE i;
	for (i=0;i<GR_AUTH_MAXIP_N;i++)
	{
		if (info.dwIpAddr[i] == 0)
			break;
	}

	//单线程--- 为加快效率可以使用静态局部变量
	static INT32 s_nDevDbID;
	static char strPid[NET_MAX_USER_PID],strDevName[30],strHost[64], strIp1[24],strMac1[36],strOne[60],strMark[160];
	memset(strIp1, 0, sizeof(strIp1));
	memset(strMac1, 0, sizeof(strMac1));
	memset(strHost, 0, sizeof(strHost));
	strMark[0] = 0;
	s_nDevDbID = info.nDbID;
	strcpy_s(strPid, NET_MAX_USER_PID, info.strPid);
	sprintf(strDevName, "%d",s_nDevDbID);//qizc
	strcpy_s(strHost,sizeof(strHost),info.strHost);
	
	if (i > 0)
	{
		sprintf(strIp1, "%d.%d.%d.%d",(info.dwIpAddr[0]&0x0ff),((info.dwIpAddr[0]>>8)&0x0ff),
					((info.dwIpAddr[0]>>16)&0x0ff),((info.dwIpAddr[0]>>24)&0x0ff));
		sprintf(strMac1, "%02x-%02x-%02x-%02x-%02x-%02x",info.baMacAddr[0],info.baMacAddr[1],info.baMacAddr[2],
						info.baMacAddr[3],info.baMacAddr[4],info.baMacAddr[5]);
	}
	if (i>1)
	{
		for (BYTE kk =1;kk<i;kk++)
		{
			sprintf(strOne, "IP:%d.%d.%d.%d MAC:%02x-%02x-%02x-%02x-%02x-%02x +++ ", 
					(info.dwIpAddr[kk]&0x0ff),((info.dwIpAddr[kk]>>8)&0x0ff),
					((info.dwIpAddr[kk]>>16)&0x0ff),((info.dwIpAddr[kk]>>24)&0x0ff),
					info.baMacAddr[kk*6+0],info.baMacAddr[kk*6+1],info.baMacAddr[kk*6+2],
					info.baMacAddr[kk*6+3],info.baMacAddr[kk*6+4],info.baMacAddr[kk*6+5]);
			strcat_s(strMark, sizeof(strMark), strOne);
		}
	}
	
	MY_LOCK(&g_LockInsrtDev);
	g_deqInsrtDev.pop_front();
	MY_UNLOCK(&g_LockInsrtDev);
	
	try
	{
        sprintf(str, "insert into t_client (id,client_guid,name,hostcompany,gip,gmac,hardware,remark) values (%d,'%s','%s','%s','%s','%s','%s','%s') ",
				s_nDevDbID, strDevName,(strHost[0]?strHost:strDevName), "1", strIp1, strMac1, strPid,strMark);
		SACommand cmd(getSAConnection(), str);
		cmd.Execute();
		log_info("CBInsertDevice DBID:%d Pid:%s Ip:%s sql= %s ", s_nDevDbID, strPid, strIp1,str);
		printf("CBInsertDevice DBID:%d Pid:%s Ip:%s ", s_nDevDbID, strPid, strIp1);
	}
	catch (const boost::exception &e)
	{
	//	log_error("CBInsertDevice Excp DBID:%d Msg:%s", s_nDevDbID, boost::diagnostic_information(e));//qizc
	}
	catch (...)
	{
		log_error("CBInsertDevice Error DBID:%d ", s_nDevDbID);
		//GRSVR_QUERY_PRN(str)
	}
	return TRUE;
}

void LoginManage::PushLogInfo(const INT32 &nPid, const INT32 &nWid, const INT32 &nType, const INT32 &nExcp)
{
	++g_lLogPushN;
	//同一时间如果超过10000条记录，则直接丢弃新的记录
	if (g_deqLogInfo.size() > 10000)
	{
		log_warning("PushLogInfo Dicard Pid:%d Wid:%d Type:%d Ret:%d", 
						nPid, nWid, nType, nExcp);
		return;
	}
	DbLogInfo info;
	info.nPid = nPid;
	info.nWid = nWid;
	info.bType		= (BYTE)nType;
	info.wExcept	= (WORD)nExcp;
	
	MY_LOCK(&g_LockLogInfo);
	g_deqLogInfo.push_back(info);
	MY_UNLOCK(&g_LockLogInfo);
}

//==========================================
#ifdef GR_FN_UPDT_DEVNAME
void LoginManage::CBPushUpdtDevName(DbDeviceInfo *pDev, const char *strName)
{
	if (strName == NULL || strName[0] == 0)
		return;
	
	DbUpdtDevName dev;
	memset(&dev, 0, sizeof(dev));
	dev.nDbID = pDev->nDbID;
	strcpy_s(dev.strDevName, sizeof(dev.strDevName), strName); /*hardware*/
	memcpy(&dev.dwIpAddr[0], &pDev->dwIpAddr[0], GR_AUTH_MAXIP_N*sizeof(DWORD32));
	memcpy(&dev.baMacAddr[0], &pDev->baMacAddr[0], GR_AUTH_MAXIP_N*6);

	MY_LOCK(&g_LockInsrtDev);
	g_deqUpdtDevName.push_back(dev);
	MY_UNLOCK(&g_LockInsrtDev);
}

bool LoginManage::CBUpdtDbDevName()
{
	if (g_deqUpdtDevName.size() == 0)
		return false;

	DbUpdtDevName dev;
	std::string substr;
	char str[1024] = {0};

	MY_LOCK(&g_LockInsrtDev);
	DbUpdtDevName& info = g_deqUpdtDevName.front();
	memcpy(&dev, &info, sizeof(dev));
	g_deqUpdtDevName.pop_front();
	MY_UNLOCK(&g_LockInsrtDev);
	
	try
	{
		BYTE i;
		for (i=0;i<GR_AUTH_MAXIP_N;i++)
		{
			if (dev.dwIpAddr[i] == 0)
				break;
		}
		static char strIp1[24],strMac1[36],strOne[60],strMark[160];
		memset(strIp1, 0, sizeof(strIp1));
		memset(strMac1, 0, sizeof(strMac1));
		memset(strOne, 0, sizeof(strOne));
		if (i > 0)
		{
			sprintf(strIp1, "%d.%d.%d.%d",(info.dwIpAddr[0]&0x0ff),((info.dwIpAddr[0]>>8)&0x0ff),
						((info.dwIpAddr[0]>>16)&0x0ff),((info.dwIpAddr[0]>>24)&0x0ff));
			sprintf(strMac1, "%02x-%02x-%02x-%02x-%02x-%02x",info.baMacAddr[0],info.baMacAddr[1],info.baMacAddr[2],
							info.baMacAddr[3],info.baMacAddr[4],info.baMacAddr[5]);
		}
		if (i>1)
		{
			for (BYTE kk =1;kk<i;kk++)
			{
				sprintf(strOne, "IP:%d.%d.%d.%d MAC:%02x-%02x-%02x-%02x-%02x-%02x +++ ", 
						(info.dwIpAddr[kk]&0x0ff),((info.dwIpAddr[kk]>>8)&0x0ff),
						((info.dwIpAddr[kk]>>16)&0x0ff),((info.dwIpAddr[kk]>>24)&0x0ff),
						info.baMacAddr[kk*6+0],info.baMacAddr[kk*6+1],info.baMacAddr[kk*6+2],
						info.baMacAddr[kk*6+3],info.baMacAddr[kk*6+4],info.baMacAddr[kk*6+5]);
				strcat_s(strMark, sizeof(strMark), strOne);
			}
		}

        sprintf(str, "update t_client set hardware='%s' , gip='%s', gmac='%s' where id=%d ",
					dev.strDevName, strIp1, strMac1, dev.nDbID);

		SACommand cmd(getSAConnection(), str);
		cmd.Execute();

		DB_MAP_DEVINFO::const_iterator it = NetWorld::m_mapDbDev.find(dev.nDbID);
		if (it != NetWorld::m_mapDbDev.end())
		{
			DbDeviceInfo *pDev = it->second;
			if (pDev)
			{
				/*更新内存数据*/
				strcpy_s(pDev->strDevName, sizeof(pDev->strDevName), dev.strDevName);/*strhost硬件号*/
				memcpy(&pDev->dwIpAddr[0], &dev.dwIpAddr[0], GR_AUTH_MAXIP_N*sizeof(DWORD32));
				memcpy(&pDev->baMacAddr[0], &dev.baMacAddr[0], GR_AUTH_MAXIP_N*6);
			}
		}

		log_info("CBUpdtDbDevName DBID:%d Name:%s ", 
						dev.nDbID, dev.strDevName);
	}
	catch (SAException &x)
	{
		log_error("CBUpdtDbDevName Exp Msg:%s", x.ErrText().GetMultiByteChars());
		handle_database_exception(x);
	}
	catch (...)
	{
		log_error("CBUpdtDbDevName Error DBID:%d ", 
						dev.nDbID);
		//GRSVR_QUERY_PRN(str)
	}
	return true;
}
#endif	

//log recorder
//这个函数并没有进行互斥处理，在大用户的时候肯定有问题
void LoginManage::CBWriteLogin(const INT32 &nPid, const INT32 &nWid, char *strPid, char *strWidName, 
						const INT32 &nType, const INT32 &nRet)
{
//#ifndef _DEBUG
	PushLogInfo(nPid, nWid, nType, nRet);
//#endif
}
//这个函数并没有进行互斥处理，在大用户的时候肯定有问题
int LoginManage::CBWriteLogOut(const NetSession *pSession, void *pErr, void *p2)
{
	if (pSession == NULL)
		return 1;
//#ifndef _DEBUG
	PushLogInfo(pSession->nID, (pSession->m_pCurUser?pSession->m_pCurUser->m_nWorkID:0), 0, (pErr?*(INT32*)pErr:0));
//#endif
    return 0;
}

void LoginManage::DbLoginsProc()
{
	int i;
 //	LONG lLogId;
	std::string str;
	char substr[2048] = {0};
	char str1[2048] = {0};
//	log_info("+++++ DbLoginsProc start :%d +++++", MyGetTID());//qizc
	while(!connect_db()){
		boost::this_thread::sleep(boost::posix_time::seconds(5));
	}
	SACommand cmd(getSAConnection(),"SET NAMES UTF8");
	cmd.Execute();

	while(!g_lLogProcExit)
	{
		if (g_deqLogInfo.size() == 0)
		{
			MySleepMilliSecond(2000);
			if (g_lLogProcExit)
				break;
			else
				continue;
		}
		//写入新注册的终端信息
		if (CBWriteInsrtDevice())
			continue;
#ifdef GR_FN_UPDT_DEVNAME
		//更新可能的新的终端名称
		if (CBUpdtDbDevName())
			continue;
#endif		

		//id 数据库设置成自增长
       str = "insert into T_RegistryMonitor (create_by_computer,create_by_user,connection_status,exception_types)";

		for (i=0;(i<4000 && g_deqLogInfo.size()>0);i++)
		{
			DbLogInfo& info = g_deqLogInfo.front();
			//lLogId = InterlockedIncrement(&g_nUsbLogMaxId);

			if (i==0)
				sprintf(substr, " select %d,%d,%d,%d", info.nPid, info.nWid, info.bType, info.wExcept);
			else
				sprintf(substr, " union all select %d,%d,%d,%d", info.nPid, info.nWid, info.bType, info.wExcept);
			str += substr;

			time_t cur_time = time(NULL);
			tm* t = localtime(&cur_time);
			if(info.bType == 0x01){ /*登入*/
				sprintf(str1, "UPDATE t_client SET client_status = %d, last_connect_time='%d-%02d-%02d %d:%d:%d' WHERE client_guid = %d", 
					info.bType, t->tm_year+1900, t->tm_mon+1, t->tm_mday, 
					t->tm_hour, t->tm_min, t->tm_sec, info.nPid);
				//sprintf(str1, "UPDATE t_client SET client_status = %d WHERE client_guid = %d", info.bType, info.nPid);
			}else{
				sprintf(str1, "UPDATE t_client SET client_status = %d, last_disconnect_time='%d-%02d-%02d %d:%d:%d' WHERE client_guid = %d", 
					info.bType, t->tm_year+1900, t->tm_mon+1, t->tm_mday, 
					t->tm_hour, t->tm_min, t->tm_sec, info.nPid);
			}

			try
			{
				SACommand cmd2(getSAConnection(), str1);
				cmd2.Execute();
			}
			catch (SAException &x)
			{
				log_error("DbLoginsProc Exp Msg:%s", x.ErrText().GetMultiByteChars());
				handle_database_exception(x);
			}
			catch(...)
			{
				log_error("DbLoginsProc Err DBID:%d ",g_nUsbLogMaxId);
				GRSVR_QUERY_PRN(str);
			}

			MY_LOCK(&g_LockLogInfo);
			g_deqLogInfo.pop_front();
			MY_UNLOCK(&g_LockLogInfo);
			++g_lLogPopN;
		}

		try
		{
			SACommand cmd(getSAConnection(), str.c_str());
			cmd.Execute();

		}
		catch (SAException &x)
		{
			log_error("DbLoginsProc Exp Msg:%s", x.ErrText().GetMultiByteChars());
			handle_database_exception(x);
		}
		catch (const boost::exception &e)
		{
			//	log_error("DbLoginsProc Excp DBID:%d Msg:%s", g_nUsbLogMaxId, boost::diagnostic_information(e));//qizc
			GRSVR_QUERY_PRN(str)
		}
		catch(...)
		{
			log_error("DbLoginsProc Err DBID:%d ",g_nUsbLogMaxId);
			GRSVR_QUERY_PRN(str);
		}
	}

	log_info("----- DbLoginsProc exit -----");
}

void CBUpldateLocalPolicy()
{
	INT naChange[DB_VER_CHANGE_COL_NUM];
	
	memset(naChange, 0, sizeof(naChange));

	std::string strTable;
	int i=0,j=0;
	//从数据库获取策略到本地缓存
	
	std::string strsql;
	strsql = "select judgement, tableName from t_change";
	SACommand cmd(g_mt.getSAConnection(), strsql.c_str());
	cmd.Execute();

	while (cmd.FetchNext() && i<sizeof(g_tTblChange)/sizeof(g_tTblChange[0]))
	{
		naChange[i] = cmd[1].asLong();
		strTable = cmd[2].asString().GetMultiByteChars();
		//查找对应的数组-保存当前版本号
		for (j=0;j<sizeof(g_tDbPlcyHnd)/sizeof(g_tDbPlcyHnd[0]);j++)
		{
			if (my_stricmp(g_tDbPlcyHnd[j].strTableName, strTable.c_str()) == 0)
			{
				g_tDbPlcyHnd[j].nPreVer = g_tDbPlcyHnd[j].nCurVer;
				g_tDbPlcyHnd[j].nCurVer = naChange[i];
				break;
			}
		}
		i++;
	}

	//Update local memory policy
	for (i=0;i<sizeof(g_tDbPlcyHnd)/sizeof(g_tDbPlcyHnd[0]);i++)
	{
		try{
			if (g_tDbPlcyHnd[i].handler && (g_tDbPlcyHnd[i].nCurVer > g_tDbPlcyHnd[i].nPreVer))
				g_tDbPlcyHnd[i].handler(g_tDbPlcyHnd[i].nFuncPara);
		}catch (SAException &x)
		{
			log_error("CBUpldateLocalPolicy Exp Msg:%s", x.ErrText().GetMultiByteChars());
		}
	}
	
}

void CBRefreshChangeTbl()
{
	CBUpldateLocalPolicy();

	int i=0, j=0;
	//如果策略变化,是否强制通知,还是在用户重新登录后才生效?
    for (i=0;i<g_net_max_session;i++)
    {
		if (g_tWorld.GetSession(i).GetSockState() == E_NET_SOCK_ESTAB)
		{
			for (j=0;j<sizeof(g_tDbPlcyHnd)/sizeof(g_tDbPlcyHnd[0]);j++)
			{
				if (g_tDbPlcyHnd[j].nCurVer > g_tDbPlcyHnd[j].nPreVer 
					&&g_tDbPlcyHnd[j].nNoDelay && g_tDbPlcyHnd[j].nMsgType && g_tDbPlcyHnd[j].pEncodeGbl){
					g_tWorld.GetSession(i).SendCommand(g_tDbPlcyHnd[j].nMsgType, 0, 0, (char *)g_tDbPlcyHnd[j].pEncodeGbl);
				}
			}
		}
    }
}


bool CBGetWarnPol(const int &nType)
{
	//由于告警策略表比较小,防止后续增加字段,暂全部取到本地
	std::string strsql;
	strsql = "select ID,communication,USBPort from T_WarnLevel";
	SACommand cmd(g_mt.getSAConnection(), strsql.c_str());
	cmd.Execute();

	int nn=0;
	
	//这里应该从数据库获取用户信息，此处临时赋值
	while (cmd.FetchNext())
	{
		//目前的数据库设计1-red 2->次级 3->lowest 
		nn = cmd[1].asLong();
		if (nn < 1 || nn > 3)
		{
			continue;
		}
		///////////////////////////////////////////////////
		// 告警等级在数据库里面1-2-3 , 但在本地内存 0-1-2
		--nn; 
		///////////////////////////////////////////////////////
		//g_tDbKeyWords[g_nDbKeyWords].bLevel = nn;
		g_tDbCfgPlcy.tWarnLvl[nn].bNetCtrl	= cmd[2].asLong();
		g_tDbCfgPlcy.tWarnLvl[nn].bUsbCtrl 	= cmd[3].asLong();
#ifdef CFG_MDL_FLCHG_MON		
		g_tDbCfgPlcy.tWarnLvl[nn].bAlarm 	= cmd[4].asLong();
		g_tDbCfgPlcy.tWarnLvl[nn].bShowWarn	= cmd[5].asLong();
		g_tDbCfgPlcy.tWarnLvl[nn].bLogKept 	= cmd[6].asLong();
#else		
		g_tDbCfgPlcy.tWarnLvl[nn].bAlarm 	= 0;
		g_tDbCfgPlcy.tWarnLvl[nn].bShowWarn	= 0;
		g_tDbCfgPlcy.tWarnLvl[nn].bLogKept 	= 0;
#endif			
	}

	log_info(" CBGetWarnPol nType:%d", nType);
	return TRUE;
}

bool CBGetSrchExtPol(const int &nRefresh)
{
	std::string strsql;
	strsql = "select classification,ID,isNoGlobal,typeName from t_filetype";
	SACommand cmd(g_mt.getSAConnection(), strsql.c_str());
	cmd.Execute();
	
	int i=0;
	BYTE nType = 0;
	std::string str;
	
	g_nDbFileType = 0;
	memset(&g_tDbExtType[0], 0, sizeof(g_tDbExtType));
	
	//这里应该从数据库获取用户信息，此处临时赋值
	while (cmd.FetchNext())
	{
		i++;
		nType = cmd[1].asLong();
		//if (nType == 1)
		{
			g_tDbExtType[g_nDbFileType].nIndex  = cmd[2].asLong();
			g_tDbExtType[g_nDbFileType].bGlobal = cmd[3].asLong();
			g_tDbExtType[g_nDbFileType].bLevel  = nType;
			str = cmd[4].asString().GetMultiByteChars();
			strcpy_s(g_tDbExtType[g_nDbFileType++].strKey, COMM_SRCH_KEY_LEN, str.c_str());
		}	
		if (g_nDbFileType >= sizeof(g_tDbExtType)/sizeof(g_tDbExtType[0]))
		{
			log_error("GetSrchExtPol overflow Num:%d", g_nDbFileType);
			break;
		}
	}
	if (i <= 0)
		return 0;

	//后缀名下载到本地是以单条记录形式存在,为便于下发转化为字符串
	UpdateGblPolicy(0x02);

	return true;
}

bool CBGetKeyPolicy(const INT &nRefresh)
{
	int i=0;
	std::string str;
	BYTE nn = 0;

	g_nDbKeyWords = 0;
	memset(&g_tDbKeyWords[0], 0, sizeof(g_tDbKeyWords));
	
	//for (i=0;i<3;i++)
	//	memset(&g_tKeyPolicy[i], 0, sizeof(MsgKeyPolicy));
	//这里应该从数据库获取用户信息，此处临时赋值
	std::string strsql;
	strsql = "select isNoGlobal,warnLevel,id,keyword from t_keyword_dic";
	SACommand cmd(g_mt.getSAConnection(), strsql.c_str());
	cmd.Execute();

	while (cmd.FetchNext())
	{
		i++;
		//if disable -> next
		nn = cmd[1].asLong();
		if (nn == 0)
		{
			continue;
		}
		
		//目前的数据库设计1-red 2->次级 3->lowest 
		nn = cmd[2].asLong();
#ifdef CFG_MDL_FLCHG_MON		
		nn = 1;
#else
		if (nn < 1 || nn > 3)
		{
			continue;
		}
#endif		
		///////////////////////////////////////////////////
		// 告警等级在数据库里面1-2-3 , 但在本地内存 0-1-2
		--nn; 
		///////////////////////////////////////////////////////
		g_tDbKeyWords[g_nDbKeyWords].nIndex = cmd[3].asLong();
		g_tDbKeyWords[g_nDbKeyWords].bLevel = nn;
		g_tDbKeyWords[g_nDbKeyWords].bGlobal = cmd[1].asLong();
		str = cmd[4].asString().GetMultiByteChars();
		//安全拷贝函数 ---> 会切断超过COMM_SRCH_KEY_LEN 长度的关键词!!!!!
		strcpy_s(g_tDbKeyWords[g_nDbKeyWords++].strKey, COMM_SRCH_KEY_LEN, str.c_str());
		if (g_nDbKeyWords >= sizeof(g_tDbKeyWords)/sizeof(g_tDbKeyWords[0]))
		{
			log_error("GetKeyPolicy overflow Num:%d", g_nDbKeyWords);
			break;
		}
	}
	if(i <=0)
		return false;

	log_info("GetKeyPolicy KeyWords Num:%d", g_nDbKeyWords);
	//关键字下载到本地是以单条记录形式存在,为便于下发转化为字符串
	UpdateGblPolicy(0x01);
	
	return true;
}

bool CBGetTmpltPolicy(const int &nRefresh)
{
	//if (nRefresh)
	//{
	//	db->TableStrategy->Active = false;
	//	db->TableStrategy->Active = true;
	//}

	std::string strsql;
	strsql = "select ID,keyWords,fileType from T_Strategy";
	SACommand cmd(g_mt.getSAConnection(), strsql.c_str());
	cmd.Execute();

	char str[4096] = {0};
	DbCfgPlcy tplcy;
	
	//---------------------------------------
	//  目前没有解决共享互斥问题
	//---------------------------------------
	g_vectDbPlcy.clear();
	while (cmd.FetchNext())
	{
		memset(&tplcy, 0, sizeof(tplcy));
		tplcy.nDbId = cmd[1].asLong();
		strcpy(str, cmd[2].asString().GetMultiByteChars());
		//parse keywords
		ConvertIntsToLevels(str, tplcy);
		strcpy(str, cmd[3].asString().GetMultiByteChars());
		if (str && str[0] != '\0')
			ConvertDbIntsToStr(str, tplcy.strFileTypes, 256, DB_LOOKUP_FILETYPES);
		g_vectDbPlcy.push_back(tplcy);
	}

	log_info(" CBGetTmpltPolicy nRefresh:%d", nRefresh);
	return true;
}

void UpdateGblPolicy(const BYTE &bFlag)
{
	//=========================================
	// 设置全局策略

	int i=0, nCount = 0;
	//不能全部清零,否则USB/NET 控制会丢失
	//memset(&g_tDbCfgPlcy, 0, sizeof(g_tDbCfgPlcy));
	if (bFlag & 0x01)
	{
		//Get buffer length
		WORD dwSize = 0;		//注意(WORD): 最大只能65536 !!!!!
		for (i=0;i<g_nDbKeyWords;i++)
		{
			if (g_tDbKeyWords[i].bGlobal && g_tDbKeyWords[i].bLevel < GR_DB_WARN_LEVEL)
				dwSize += strlen(g_tDbKeyWords[i].strKey) + 1 ; // + ';'
		}
		//in case of overleap - max 20k
		if (dwSize >= 20 * 1024)
			dwSize = 20 * 1024;

		//dwSize += ((DbWarnLvStr*)0)->strMonitor;
		dwSize += sizeof(DbWarnLvStr) + 1;
		
		//allocate memory  - if existing size bigger, no need allocate
		if (!UtlAllocMem(&g_tDbCfgPlcy.tKeysPlcy, dwSize))
		{
			//if allocated fail, g_tDbCfgPlcy.tKeysPlcy = NULL
			//g_tDbCfgPlcy.tKeysPlcy->wLen = 0;
			log_error(" UpdateGblPolicy :%d no memory!", bFlag);
			return;
		}	
		//clear memory - not wLen and the string pointer
		g_tDbCfgPlcy.tKeysPlcy->strMonitor[0] = 0;
		//memset(&g_tDbCfgPlcy.tKeysPlcy->strMonitor, 0, g_tDbCfgPlcy.tKeysPlcy->wLen);
		memset(&g_tDbCfgPlcy.tKeysPlcy->waOffset[0], 0, sizeof(g_tDbCfgPlcy.tKeysPlcy->waOffset));
		
		//按照优先次序填充关键词
		for (int j=0;j<GR_DB_WARN_LEVEL;j++)
		{
			for (i=0;i<g_nDbKeyWords;i++)
			{
				if (g_tDbKeyWords[i].bGlobal && g_tDbKeyWords[i].bLevel == j)
				{
					strcat(g_tDbCfgPlcy.tKeysPlcy->strMonitor, g_tDbKeyWords[i].strKey);
					strcat(g_tDbCfgPlcy.tKeysPlcy->strMonitor, ";");
					++nCount;
				}
			}
			if (j<GR_DB_WARN_LEVEL-1)
				g_tDbCfgPlcy.tKeysPlcy->waOffset[j] = strlen(g_tDbCfgPlcy.tKeysPlcy->strMonitor);

		}
		g_tDbCfgPlcy.tKeysPlcy->wMsg 	= NET_MSG_FILE_MON_PLY;
		g_tDbCfgPlcy.tKeysPlcy->wMsgLen = sizeof(DbWarnLvStr) + strlen(g_tDbCfgPlcy.tKeysPlcy->strMonitor);

		// update the global infor !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (g_tDbPlcyHnd[2].pEncodeGbl != g_tDbCfgPlcy.tKeysPlcy)
			g_tDbPlcyHnd[2].pEncodeGbl = g_tDbCfgPlcy.tKeysPlcy;
		// --------------------------------------------------------
	}
	if (bFlag & 0x02)
	{
		g_tDbCfgPlcy.strFileTypes[0] = 0;
		for (i=0;i<g_nDbFileType;i++)
		{
			if (g_tDbExtType[i].bGlobal && g_tDbExtType[i].bLevel == 1)
			{
				strcat(g_tDbCfgPlcy.strFileTypes, g_tDbExtType[i].strKey);
				strcat(g_tDbCfgPlcy.strFileTypes, ";");
				++nCount;
			}
		}
	}
	log_info(" UpdateGblPolicy Flag:%x Num:%d ", bFlag, nCount);
}

//将数据库'1;2;3' 形式的序列串转化为三级告警字符串
BOOL ConvertIntsToLevels( char *strNumber, DbCfgPlcy & plcy)
{
	if (strNumber == NULL )
		return FALSE;
	
	char *pSrch = NULL;
	char *p = strNumber;
	char strInt[10];
	INT  id = 0;
	UINT uIdLen = 0;
	while (*p)
	{
		pSrch = strchr(p, ';');
		//the last one may have  no ";"
		if (pSrch)
		{
			uIdLen = pSrch - p;
		}
		else
		{
			uIdLen = strlen(strNumber) - (p - strNumber);
		}
		
		{
			//if only one ';'
			if (uIdLen == 0)//(pSrch == p )
			{
				++p;
				continue;
			}
			//in case of strInt overflowing
			if (uIdLen >= sizeof(strInt)) //(pSrch - p >= sizeof(strInt))
			{
				break;//goto DbPlcyCnvrtLabel;
			}

			//get integer
			memcpy(&strInt[0], p, uIdLen); //pSrch - p);
			strInt[uIdLen] = 0;
			//++++++++++++++++++++++
			//exchange the search pointer, should not use p or pSrch later
			if (pSrch)
				p = pSrch+1;
			else
				*p = 0;
			//++++++++++++++++++++++
			
			try{id = atoi(strInt);}
			catch(...){id = -1; }

			if (id < 0 || id > g_nDbKeyWords || g_tDbKeyWords[id].bLevel > 2)
				continue;
			//if (strlen(plcy.tKeyPlcy[g_tDbKeyWords[id].bLevel].strKeys) + strlen(g_tDbKeyWords[id].strKey)
			//		>= DB_MAX_KEYWORDBUF_LEN)
			//	continue;
			
			//strcat(plcy.tKeyPlcy[g_tDbKeyWords[id].bLevel].strKeys, g_tDbKeyWords[id].strKey);

		}
			
	}

	return TRUE;
}


//把数据库中数字相隔的形式转化为本地内存字符串形式
//	eg: 1;2;3 ->key1;key2;key3;
//		uLen[in] - length of strString buffer
//		bType [in] - index conversion type (key words/file types/policies etc.)
BOOL ConvertDbIntsToStr( char *strNumber, char *strString, const UINT &uLen,
						const BYTE & bType)
{
	if (strNumber == NULL || strString == NULL)
		return FALSE;
	
	char *pSrch = NULL;
	char *p = strNumber;
	char *pDst = strString;
	char strInt[10];
	INT  id = 0;
	UINT uIdLen = 0;
	strString[0] = 0;
	while (*p)
	{
		pSrch = strchr(p, ';');
		//the last one may have  no ";"
		if (pSrch)
		{
			uIdLen = pSrch - p;
		}
		else
		{
			uIdLen = strlen(strNumber) - (p - strNumber);
		}
		
		{
			//if only one ';'
			if (uIdLen == 0)//(pSrch == p )
			{
				++p;
				continue;
			}
			//in case of strInt overflowing
			if (uIdLen >= sizeof(strInt)) //(pSrch - p >= sizeof(strInt))
			{
				goto DbPlcyCnvrtLabel;
			}

			//get integer
			memcpy(&strInt[0], p, uIdLen); //pSrch - p);
			strInt[uIdLen] = 0;
			try{id = atoi(strInt);}
			catch(...){id = -1; goto DbPlcyCnvrtLabel;}

			if (bType == DB_LOOKUP_KEYWORDS)
			{
				//get index string
				if (id < 0 || id >= g_nDbKeyWords)
					goto DbPlcyCnvrtLabel;

				//in case of strString overflowing
				if (strlen(strString) + 1 + strlen(g_tDbKeyWords[id].strKey) >= uLen)
				{
					break;
				}
				//Append string
				strcat(strString, g_tDbKeyWords[id].strKey);
			}
			else if (bType == DB_LOOKUP_FILETYPES)
			{
				//get index string
				if (id < 0 || id >= g_nDbFileType)
					goto DbPlcyCnvrtLabel;

				//in case of strString overflowing
				if (strlen(strString) + 1 + strlen(g_tDbExtType[id].strKey) >= uLen)
				{
					break;
				}
				//Append string
				strcat(strString, g_tDbExtType[id].strKey);
			}
		}
			
DbPlcyCnvrtLabel:
		//exchange the search pointer
		if (pSrch)
			p = pSrch+1;
		else
			*p = 0;
	}

	return TRUE;
}

