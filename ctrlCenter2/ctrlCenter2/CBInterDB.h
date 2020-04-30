#ifndef _CBINTERDB_H_
#define _CBINTERDB_H_
#include "sql_manager.h"
#include "log_utility.h"
#include "global.h"
#include "ctrlCenter.h"
#include "NetUtlity.h"
#include "NetWorld.h"
#define GR_FN_UPDT_DEVNAME
//�����������û���Ϣ���豸��Ϣ���µ�ʱ���
extern time_t	g_tCurLocTime;
//------------------------------------------------
// �ⲿ����
extern DbWarnLvStr *g_tDbFilesMon;
extern DbWarnLvStr *g_tDbRegstyMon;

void UpdateGblPolicy(const BYTE &bFlag);
void CBRefreshChangeTbl();
BOOL ConvertIntsToLevels( char *strNumber, DbCfgPlcy & plcy);
BOOL ConvertDbIntsToStr( char *strNumber, char *strString, const UINT &uLen, const BYTE & bType);

class LoginManage
	:public sql_manager{

public:
	/* ��¼�̺߳��� */
	void DbLoginsProc();

	/* �̺߳�����ʼ������ */
	bool InitLogInfoProc(void *param);

	/* �ͷ���Դ */
	bool UnInitLogInfoProc();

	/* �����豸����ʱ���ô˺��� */
	bool CBWriteInsrtDevice();
	
	/*������豸��Ϣ������*/
	void CBPushNewDevice(INT32 &nid, const NetMsgAuthEx2 &tAuth);

	void PushLogInfo(const INT32 &nPid, const INT32 &nWid, const INT32 &nType, const INT32 &nExcp);
	/* ����ص� */
	void CBWriteLogin(const INT32 &nPid, const INT32 &nWid, char *strPid, char *strWidName, 
						const INT32 &nType, const INT32 &nRet);
	/* �ǳ��ص� */
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