#ifndef _CBINTERDB_H_
#define _CBINTERDB_H_
#include "sql_manager.h"
#include "log_utility.h"
#include "global.h"
#include "ctrlCenter.h"
#include "NetUtlity.h"
#include "NetWorld.h"
#define GR_FN_UPDT_DEVNAME
//用于周期性用户信息和设备信息更新的时间戳
extern time_t	g_tCurLocTime;
//------------------------------------------------
// 外部变量
extern DbWarnLvStr *g_tDbFilesMon;
extern DbWarnLvStr *g_tDbRegstyMon;

void UpdateGblPolicy(const BYTE &bFlag);
void CBRefreshChangeTbl();
BOOL ConvertIntsToLevels( char *strNumber, DbCfgPlcy & plcy);
BOOL ConvertDbIntsToStr( char *strNumber, char *strString, const UINT &uLen, const BYTE & bType);

class LoginManage
	:public sql_manager{

public:
	/* 登录线程函数 */
	void DbLoginsProc();

	/* 线程函数初始化操作 */
	bool InitLogInfoProc(void *param);

	/* 释放资源 */
	bool UnInitLogInfoProc();

	/* 有新设备接入时调用此函数 */
	bool CBWriteInsrtDevice();
	
	/*添加新设备信息到队列*/
	void CBPushNewDevice(INT32 &nid, const NetMsgAuthEx2 &tAuth);

	void PushLogInfo(const INT32 &nPid, const INT32 &nWid, const INT32 &nType, const INT32 &nExcp);
	/* 登入回调 */
	void CBWriteLogin(const INT32 &nPid, const INT32 &nWid, char *strPid, char *strWidName, 
						const INT32 &nType, const INT32 &nRet);
	/* 登出回调 */
	int CBWriteLogOut(const NetSession *pSession, void *pErr, void *p2);
	void CBPushUpdtDevName(DbDeviceInfo *pDev, const char *strName);
	bool CBUpdtDbDevName();
};

void CBUpldateLocalPolicy();
void CBRefreshChangeTbl();
bool CBGetWarnPol(const int &nType);
bool CBGetSrchExtPol(const int &nRefresh);
bool CBGetKeyPolicy(const INT &nRefresh);
bool CBGetTmpltPolicy(const int &nRefresh);
void UpdateGblPolicy(const BYTE &bFlag);
BOOL ConvertIntsToLevels( char *strNumber, DbCfgPlcy & plcy);
BOOL ConvertDbIntsToStr( char *strNumber, char *strString, const UINT &uLen, const BYTE & bType);
int CBWriteCmdAck(const int &nID, const int &nType, const char *pPara);

extern LoginManage g_login ;
#endif