#ifndef _NET_SESSION_H__
#define _NET_SESSION_H__

#include <map>
#include <string>
#include <time.h>
#include"NetDef.h"
#include"NetUtlity.h"
#include"shared_buffer.hpp"

//定义每个消息的处理函数
class NetSession;

typedef struct NET_OPCODE_HANDLER_T
{
	unsigned short	wMsg;
	unsigned short	wState;
    int		(*handler)(const NetSession *, void *);
}NetOpcHandler;
/*
typedef INT (__stdcall *PFNET_STATE_HANDLER)(const NetSession *, void *, void *);
*/
//本地缓存的用户信息
class  NetUserInfo
{
public:
	int			m_nWorkID;			//数据库表中的ID记录，自动增长
	unsigned int	 m_dwFlag;			//用户控制信息
	char			m_strWID[NET_MAX_USER_NAME];
	char			m_strName[NET_MAX_USER_NAME];
	char			m_strPwd[NET_MAX_USER_NAME];
	//还有其它包括部门、级别、群组、重点监控等的信息
};
typedef std::map<std::string, NetUserInfo*> NET_MAP_USERINFO; 

////////////////////////////////////////////////////////////
//数据传输单句柄数据
class  NetSession /*: public NetWork*/
{
public:
	NetSession();
	~NetSession();
	int			nID;			//客户端物理终端唯一标识
	long long	m_tAliveTime;	//最近存活时戳
	time_t	m_tCreateTime;	//会话激活时间
	char	m_strHostDepart[GR_HOST_DEPART_N];
	char	m_strHostResponsor[GR_HOST_RESPONSOR_N];

	char			m_strPID[NET_MAX_USER_PID];	//客户端物理终端唯一标识,这个不一定要写入会话?????
	int			m_nPlcyId;		//对于的策略ID , if > 1 -> 重点监控策略
	int			m_nAreaId;		//对应于数据库中的区域ID
	NetUserInfo*	m_pCurUser;	//当前合法用户信息-与数据库对应
	unsigned long	ulPeerIpAddr;	//远端IP地址 - 暂不支持IPV6 ?????
	int			nPeerPort;		//远端PORT
	unsigned long	ulLclIpAddr;	//本端IP地址
	int			nLclPort;		//本端PORT
	int			nPeerState;		//client state
	unsigned short			m_wCliVer;		//对应的客户端版本
	char			m_bAliveTick;	//最近心跳次数,保存最近没有响应的次数
	char			m_bReserved;
	unsigned int	m_nQueLen;
	char			m_strBufQue[5120];	//这个将来可以作为一个指针来动态申请?????
	//查询统计调试用
	//NetTransFile*	m_pTransF;
	//一个会话可以在发送文件的同时接受文件,所以需要用独立的状态
	char			m_bSendState;
	char			m_bRecvState;
	unsigned short m_wSsnIdx;		//当前会话所在索引-快速锁用-必须初始化
    int 	    m_nTransMID;	//接收文件时采用的TID-会发送给对端(m_dwSendTid)一一对应
	int			m_dwCurPos;
	unsigned int			m_dwFileSize;	//file size or sending buffer length
	unsigned int			m_dwFileSeq;
	FILE * /*HANDLE	*/			m_hTransFile;	//接收文件的句柄
	FILE  * /*HANDLE	*/		m_hSendFile;	//发送文件的句柄-支持同时发送和接收文件
	unsigned int			m_dwSendPos;
	unsigned int			m_dwSendSize;	//file size or sending buffer length
	unsigned int			m_dwSendTid;	//发送文件时产生的TID，接收端对应为m_nTransMID
	char*			m_pSndBuf;
	static char		s_strTaskDir[256];
	static char		s_strNetTaskDir[256];
	static char		s_strCheckDir[256];
	static char     s_strZiChaDir[256];

	inline const UINT32& GetSockState(){return m_uSockState;}
	inline void SetSockState(const UINT32 &uState){m_uSockState = uState;}

	DWORD SendData(const char * szData, int nDataLen, const WORD &wAtch = 0) ;
	DWORD SendDataEx(const char * szData, const int &nDataLen, const WORD &wAtch= 0 ) ;
	DWORD SendCommand(const int &nMsgType, const int &nID=0, const int &nSubID = 0, const char *pPara=NULL);

	FILE * 	    CreateTransFile(const char *strFile, DWORD32 &nErr);
	DWORD 	StartTransSend(const char *strFile, const DWORD &dwFSize, const int &nTID=0);
	DWORD	StartTransRecv(const char *strFile, const INT32 &nCmdID, const DWORD32 &dwSize);
	BOOL 	    AbortTrans(const DWORD32 &nLastErr, const int &nDirection=0);
	DWORD	SendErrInd(const DWORD32 &dwTid, const DWORD32 &dwErr, const char *strMsg);

	DWORD 	WriteTransFile(MsgFileChunk &tMsg);
	DWORD	OnRecvFileInfo(const MsgTransFile & tMsg);

	BOOL 	LockInitTrans(const DWORD32 &nLastErr = 0, const int &nDirection=0);
		//设置文件发送状态
	inline	void 	SetSendState(const BYTE &nState){m_bSendState = nState;}
	//获取文件发送状态
	inline  BYTE	GetSendState(){return m_bSendState;}
	//设置文件接收状态
	inline	void 	SetRecvState(const BYTE &nState){m_bRecvState = nState;}
	
	inline  BYTE	GetRecvState(){return m_bRecvState;}
	inline  int		GetTransID(){return m_nTransMID;}
	inline  int		GetSendTid(){return m_dwSendTid;}

	inline BOOL IsConnected(){ return (GetSockState() == E_NET_SOCK_ESTAB);}

	static void		GetTransFileByTid(const INT32 &nCmdID, char *strFile, const DWORD32 &dwLen);
	inline  void SetTcpNetId(unsigned long long   tcp_net_id ){m_tcp_net_id =tcp_net_id; }
	inline  unsigned long long GetTcpNetId(  ){return m_tcp_net_id; }
	char m_strTransFile[256];

	inline  time_t GetCreateTime(){return m_tCreateTime;}
	inline  void SetCreateTime(const time_t &dwTime){m_tCreateTime = dwTime;}
	DWORD StartTransBuf( axis::shared_buffer& buffer);

protected :
	UINT32			m_uSockState;					//SOCKET状态

	unsigned long long m_tcp_net_id;  //对应的TCP连接编号

   

	static NetOpcHandler*	m_spOpcHandler;
	//static PFNET_STATE_HANDLER m_spStateHdlr;
};


#endif

