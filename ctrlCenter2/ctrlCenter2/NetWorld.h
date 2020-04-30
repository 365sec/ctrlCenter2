#ifndef  NETWORLD_H
#define NETWORLD_H
#include <map>
#include <string>
#include <time.h>
#include "NetUtlity.h"
#include "NetSession.h"

//���ػ�����û���Ϣ
typedef struct DB_DEVICE_INFO_T  
{
public:
	int			     nDbID;			//client_guid
	char			strHard[NET_MAX_USER_PID];		//Ӳ�����к�- ���ݿ� varchar(64)
	char			strDevCode[100];	//��������
	char			strDevName[100];	//hardware
	char         strHostResponsor[200];	//hostresponsor
	char         strHostDepart[200];	//hostdepart
	DWORD32	dwIpAddr[GR_AUTH_MAXIP_N];		
	BYTE	baMacAddr[GR_AUTH_MAXIP_N*6];	
}DbDeviceInfo;


typedef std::map<int, DbDeviceInfo*> DB_MAP_DEVINFO; 


class  NetWorld
{
public:
	NetWorld();
	~NetWorld();


	NetSession& GetSession(const int &nIdx);
	NetSession*  GetSessionPtr(const int &nIdx);

    static NetSession*  GetSessionByNetId(unsigned long long tcp_net_id);

	int GetSidByPid(const int &pid);
	
	static void	ClearMapUser();
	static void	InsertMapUser(std::string strwid, NetUserInfo *pUser);
	void UpdateMapUser( const NetUserInfo &User);
	static void UpdateMapUser(std::string &strwid , std::string &strNew, NetUserInfo *pUser);

	//�������ݰ�
	static bool  HandlePacket(unsigned long long tcp_net_id, byte * dat, size_t size1);

	static NetSession * HandleAuth(const NetMsgAuthEx2 &tAuth, NetUserInfo **pUser);
	static NetSession * HandleAuthEx(const NetMsgAuthEx2 &tAuth);
	static NetSession * HandleUpgradeAuth(char *strPid, char *strWid);

	static NetSession * FindSession(const char *strId);
	static NetSession * GetFreeSession(const char *strPid);
	static NetSession * GetUpdateFreeSession(const char *strPid);

	static bool   OnSocketClosed(unsigned long long tcp_net_id);

	static void	LockUsers(){};
	static void	UnLockUsers(){};

	static void	LockDbDev(){
		MY_LOCK(&NetWorld::m_DbDevLock);
	};
	static void	UnLockDbDev(){
		MY_UNLOCK(&NetWorld::m_DbDevLock);
	};

	static  int GetCheckLogIp(){return m_nCheckIpAddr;}

	BOOL	InitDbDeviceInfo();
	bool InitResource();
	DbDeviceInfo*	GetFreeDbDev();
public:	
	NetUserInfo*	m_pUserArray;

	//��ʼ���û���Ϣ
	bool	InitUserInfo();
	static std::map<std::string, NetUserInfo*> 	m_mapUser;

	int				m_nUserInfoN;
	static int	m_nCheckIpAddr;	//���Ƶ�¼ʱ�Ƿ�У��IP��ַ
	int				m_nValidUser;
	DbDeviceInfo*	m_pDbDevArr;
	static MY_CS_MUTEX m_DbDevLock;
	

	static DbWarnLvStr**	m_ppDbRegMonGbl;
	static DbWarnLvStr**	m_ppDbFileMonGbl;
	static DB_MAP_DEVINFO 						m_mapDbDev;

	BOOL UpdateDeviceInfo(const int &nID, const int &nPlcyId, const int &nAid, const char *strIp,const char* hostdepart,const char* hostresponsor);

	static void SetFRMonGbl(DbWarnLvStr **pReg, DbWarnLvStr **pFile)
	{
		m_ppDbRegMonGbl = pReg; m_ppDbFileMonGbl = pFile;
	}

public:	

#ifdef CFG_MDL_UPGRADE_SVR
	static WORD			m_wVer;
	static WORD			m_wVfiles;
	static DWORD32		m_dwBufSize;
	static INT32		m_nAllowNum;		//�����Զ���������Ŀ <=0 ������
	static INT32		m_nCurUpgN;			//��ǰ�Ѿ��ɹ��������û���
	static  WORD GetVersion(){return m_wVer;}
	static  void SetVersion(const WORD w){m_wVer=w;}
	static inline WORD GetVfileN(){return m_wVfiles;}
	static inline void SetVfileN(const WORD w){m_wVfiles=w;}	
	static void SetTransBuf(char *p, const DWORD32 &len);
	static inline INT32 GetAllowedN(){return m_nAllowNum;}
	static inline void SetAllowedN(const INT32 w){m_nAllowNum=w;}
	static inline INT32 GetCurAllowN(){return m_nCurUpgN;}
	static inline void IncreAllowedN(){++m_nCurUpgN;}	
	static axis::shared_buffer   m_pTransBufbuffer;
#endif

};

extern  NetWorld g_tWorld;

#endif


