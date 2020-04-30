#ifndef _NET_SESSION_H__
#define _NET_SESSION_H__

#include <map>
#include <string>
#include <time.h>
#include"NetDef.h"
#include"NetUtlity.h"
#include"shared_buffer.hpp"

//����ÿ����Ϣ�Ĵ�����
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
//���ػ�����û���Ϣ
class  NetUserInfo
{
public:
	int			m_nWorkID;			//���ݿ���е�ID��¼���Զ�����
	unsigned int	 m_dwFlag;			//�û�������Ϣ
	char			m_strWID[NET_MAX_USER_NAME];
	char			m_strName[NET_MAX_USER_NAME];
	char			m_strPwd[NET_MAX_USER_NAME];
	//���������������š�����Ⱥ�顢�ص��صȵ���Ϣ
};
typedef std::map<std::string, NetUserInfo*> NET_MAP_USERINFO; 

////////////////////////////////////////////////////////////
//���ݴ��䵥�������
class  NetSession /*: public NetWork*/
{
public:
	NetSession();
	~NetSession();
	int			nID;			//�ͻ��������ն�Ψһ��ʶ
	long long	m_tAliveTime;	//������ʱ��
	time_t	m_tCreateTime;	//�Ự����ʱ��
	char	m_strHostDepart[GR_HOST_DEPART_N];
	char	m_strHostResponsor[GR_HOST_RESPONSOR_N];

	char			m_strPID[NET_MAX_USER_PID];	//�ͻ��������ն�Ψһ��ʶ,�����һ��Ҫд��Ự?????
	int			m_nPlcyId;		//���ڵĲ���ID , if > 1 -> �ص��ز���
	int			m_nAreaId;		//��Ӧ�����ݿ��е�����ID
	NetUserInfo*	m_pCurUser;	//��ǰ�Ϸ��û���Ϣ-�����ݿ��Ӧ
	unsigned long	ulPeerIpAddr;	//Զ��IP��ַ - �ݲ�֧��IPV6 ?????
	int			nPeerPort;		//Զ��PORT
	unsigned long	ulLclIpAddr;	//����IP��ַ
	int			nLclPort;		//����PORT
	int			nPeerState;		//client state
	unsigned short			m_wCliVer;		//��Ӧ�Ŀͻ��˰汾
	char			m_bAliveTick;	//�����������,�������û����Ӧ�Ĵ���
	char			m_bReserved;
	unsigned int	m_nQueLen;
	char			m_strBufQue[5120];	//�������������Ϊһ��ָ������̬����?????
	//��ѯͳ�Ƶ�����
	//NetTransFile*	m_pTransF;
	//һ���Ự�����ڷ����ļ���ͬʱ�����ļ�,������Ҫ�ö�����״̬
	char			m_bSendState;
	char			m_bRecvState;
	unsigned short m_wSsnIdx;		//��ǰ�Ự��������-��������-�����ʼ��
    int 	    m_nTransMID;	//�����ļ�ʱ���õ�TID-�ᷢ�͸��Զ�(m_dwSendTid)һһ��Ӧ
	int			m_dwCurPos;
	unsigned int			m_dwFileSize;	//file size or sending buffer length
	unsigned int			m_dwFileSeq;
	FILE * /*HANDLE	*/			m_hTransFile;	//�����ļ��ľ��
	FILE  * /*HANDLE	*/		m_hSendFile;	//�����ļ��ľ��-֧��ͬʱ���ͺͽ����ļ�
	unsigned int			m_dwSendPos;
	unsigned int			m_dwSendSize;	//file size or sending buffer length
	unsigned int			m_dwSendTid;	//�����ļ�ʱ������TID�����ն˶�ӦΪm_nTransMID
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
		//�����ļ�����״̬
	inline	void 	SetSendState(const BYTE &nState){m_bSendState = nState;}
	//��ȡ�ļ�����״̬
	inline  BYTE	GetSendState(){return m_bSendState;}
	//�����ļ�����״̬
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
	UINT32			m_uSockState;					//SOCKET״̬

	unsigned long long m_tcp_net_id;  //��Ӧ��TCP���ӱ��

   

	static NetOpcHandler*	m_spOpcHandler;
	//static PFNET_STATE_HANDLER m_spStateHdlr;
};


#endif

