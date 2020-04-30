//#include "NetComm.h"
#include <stdio.h>

#ifdef _WIN32
#include <process.h>
#endif

#include "NetDef.h"
#include "NetSession.h"
#include "NetWorld.h"
#include"NetDef.h"
#include "typedef.h"
#include "CBInterDB.h"
#include"FileTrans.h"
#include "log_utility.h"
#include"both.h"

extern  axis::wait_list<axis::shared_buffer> g_filecopy_writing_list;

int CBUpdateUsrPwd(const int &id, const char *strNewPwd){
    return 0;
 }


static NetSession		g_tSessions[NET_MAX_SESSION];
NetWorld		g_tWorld;
DB_MAP_DEVINFO 						NetWorld::m_mapDbDev;
DbWarnLvStr**		NetWorld::m_ppDbRegMonGbl 	= NULL;
DbWarnLvStr**		NetWorld::m_ppDbFileMonGbl 	= NULL;
MY_CS_MUTEX NetWorld::m_DbDevLock;


NetWorld::NetWorld(){
	m_pUserArray	= NULL;
	MyInitCsMutex(NetWorld::m_DbDevLock);
}

NetWorld::~NetWorld(){
	if (m_pUserArray)
		delete []m_pUserArray;
	MyDeleteMutex(&NetWorld::m_DbDevLock);
}


std::map<std::string, NetUserInfo*>	NetWorld::m_mapUser;
int 	NetWorld::m_nCheckIpAddr = 0;


NetSession&  NetWorld::GetSession(const int &nIdx)
{
	return g_tSessions[nIdx];
}

NetSession*  NetWorld::GetSessionPtr(const int &nIdx)
{
	//���ﲻ������ԭ�������С�̶���ÿ��������Զ��Ӧͬһ��λ�á�
	if(nIdx>= g_net_max_session)
	{
	   log_error("[GetSessionPtr] error out of bound %d",nIdx);
	   printf("[GetSessionPtr] error out of bound %d\n",nIdx);
	   return NULL;
	}
	return &g_tSessions[nIdx];
}

NetSession*  NetWorld::GetSessionByNetId(unsigned long long tcp_net_id)
{
	//���ﲻ������ԭ�������С�̶���ÿ��������Զ��Ӧͬһ��λ�á�
	for ( int i=0;i<g_net_max_session;i++)
	{
		if (g_tSessions[i].GetTcpNetId() == tcp_net_id)
			return &g_tSessions[i];
    }
	return NULL;

}
int NetWorld::GetSidByPid(const int &pid)
{
	//���ﲻ������ԭ�������С�̶���ÿ��������Զ��Ӧͬһ��λ�á�
	for ( int i=0;i<g_net_max_session;i++)
	{
		if (g_tSessions[i].nID == pid)
			return i;
    }
	return -1;
}

bool NetWorld::InitUserInfo()
{
	if (m_pUserArray)
		delete [] m_pUserArray;
	m_mapUser.clear();

	m_nUserInfoN = 0;
	m_pUserArray = new NetUserInfo[NET_MAX_SSN_USERS];
	if (!m_pUserArray)
	{
		log_error( "\r\nInitUserInfo : no memory for user:%d ", NET_MAX_SSN_USERS);
		return FALSE;
	}
	m_nUserInfoN = NET_MAX_SSN_USERS;
	int i=0;
	char caNum[16];
	//����Ӧ�ô����ݿ��ȡ�û���Ϣ���˴���ʱ��ֵ
	for (i=0; i<NET_MAX_SSN_USERS; i++)
	{
		m_pUserArray[i].m_nWorkID	= i+1;
		sprintf(caNum,"%d",i+1);				//qizc
		strcpy(m_pUserArray[i].m_strWID, caNum);//qizc
		strcpy(m_pUserArray[i].m_strName,caNum);//qizc
		strcpy(m_pUserArray[i].m_strPwd, caNum);//qizc
	}

	//init map, fast index
	std::string str;
	for (i=0; i<NET_MAX_SSN_USERS; i++)
	{
		str = (std::string)m_pUserArray[i].m_strWID;
		m_mapUser.insert(std::pair<std::string, NetUserInfo*>(str, &m_pUserArray[i]));
	}
	return TRUE;
}

void NetWorld::ClearMapUser()
{
	m_mapUser.clear();
}

void NetWorld::InsertMapUser(std::string strwid , NetUserInfo *pUser)
{
	m_mapUser.insert(std::pair<std::string, NetUserInfo*>(strwid, pUser));
}

void NetWorld::UpdateMapUser(std::string &strwid , std::string &strNew, NetUserInfo *pUser)
{
	NET_MAP_USERINFO::iterator it = m_mapUser.find(strwid);
	if (it != m_mapUser.end())
		m_mapUser.erase(it);
	//����map�����������ֵ�ظ�����ִ�в�������󣬿��Ը��ݷ���ֵ��ȡ�����Ƿ�ɹ�
	m_mapUser.insert(std::pair<std::string, NetUserInfo*>(strNew, pUser));
}

void NetWorld::UpdateMapUser( const NetUserInfo &User)
{
	//����Ĵ��뵱�����ظ���ʱ�����������
	//�ж�ͬ���Ĺ����Ƿ���ڣ�����ڸ��·����������û�
	std::string strwid = std::string(User.m_strWID);
	NET_MAP_USERINFO::iterator it = m_mapUser.find(strwid);
	if (it != m_mapUser.end())
	{
		NetUserInfo *pLocal = it->second;
		if (pLocal == NULL)
			return;
	
		//Update
		pLocal->m_nWorkID = User.m_nWorkID;
		if (strcmp(pLocal->m_strName, User.m_strName) != 0)
			strcpy(pLocal->m_strName, User.m_strName);
		if (strcmp(pLocal->m_strPwd, User.m_strPwd) != 0)
			strcpy(pLocal->m_strPwd, User.m_strPwd);
		
		//log_info( " => ID:%d WID:%s Name:%s Pwd:%s", 
					//	pLocal->m_nWorkID, pLocal->m_strWID, pLocal->m_strName, pLocal->m_strPwd);
	}
	else
	{
		//û���ҵ��û���˵���������û��������ڴ�
		for (int i=0;i<NET_MAX_SSN_USERS;i++)
		{
			if (m_pUserArray[i].m_nWorkID == 0 && m_pUserArray[i].m_strWID[0] == 0)
			{
				memcpy(&m_pUserArray[i], &User, sizeof(User));
				m_mapUser.insert(std::pair<std::string, NetUserInfo*>(strwid, &m_pUserArray[i]));
				//log_info( "UpdateMapUser insert : ID:%d WID:%s Name:%s", 
							//User.m_nWorkID, User.m_strWID, User.m_strName);
				return;
			}
		}
		//log_error( "User change Not Found => ID:%d WID:%s Name:%s", 
						//User.m_nWorkID, User.m_strWID, User.m_strName);
	}
}




//[Description]
//	����������֤����,����ͨ��IP��ַ���һỰ,��������У��
//[Parameter]
//	strPid	: identifier of the computer, such as mac address, cpu id or harddisk id
//	strWid	: identifier of the user, not the user name
//	strPwd	: password of the user
//	pUser	: [out] user information from the database 
NetSession* NetWorld::HandleUpgradeAuth(char *strPid, char *strWid)
{
	if (strWid == NULL || strcmp(strWid, "GRXA") != 0 || strPid == NULL || strlen(strPid) >= NET_MAX_USER_PID)
		return NULL;
	log_info("HandleUpgradeAuth -PID:%s WID:%s", strPid, strWid);

	//����Ŀǰ���Զ����¿ͻ��˲�û���ϴ�PID������޷�ʹ�������������,ȫ������
	//��˿��ܵ���һ���ն����ж���Զ����³���
	NetSession *pSn = NULL;
//	pSn = FindSession(strPid);
//	if (pSn)
//	{
//		return pSn;
//	}
//	else
	{
		//��֤IP��ַ----- ��ʱ����
		if (GetCheckLogIp())
			return NULL;
		
		pSn = GetUpdateFreeSession(strPid);//GetFreeSession(strPid);
		if (pSn)
		{
			//strcpy(pSn->m_strPID, strPid);
			/*CLogger::Instance()->LogWrite(LOG_MODULE_NETSVR, LOG_TYPE_DEBUG, 
					LOG_LEVEL_INFO, ": HandleUpgradeAuth - Add new user, PID :%s, WID:%s, SID:%d", strPid, strWid, pSn->m_wSsnIdx);
			*/
			return pSn;
		}
		else
		{
			/*CLogger::Instance()->LogWrite(LOG_MODULE_NETSVR, LOG_TYPE_DEBUG, 
					LOG_LEVEL_ERROR, "--- HandleUpgradeAuth no free ssn PID :%s, WID:%s, SID:%d", strPid, strWid, pSn->nID);
			*/
			return NULL;
		}
	}
	return NULL;
}

extern int  get_session_id(unsigned long long tcp_net_id);
extern void set_session_id(unsigned long long tcp_net_id, int  sessionid );
extern void close_tcp_id(unsigned long long tcp_net_id);

#define MAKEWORD(a, b)      ((unsigned short)((( unsigned char)(((unsigned long)(a)) & 0xff)) | ((unsigned short)(( unsigned char)(((unsigned long)(b)) & 0xff))) << 8))

//ע�ⷵ��False��ر�����
bool  NetWorld::HandlePacket(unsigned long long tcp_net_id, byte * dat, size_t size1){
		DWORD nFlag = 0;			
		DWORD nBytes = 0;
	    WORD		wMsg	 = NET_MSG_UNKNOWN;
		NetMsgAuthEx2	tAuth	 ;
		msg_type::head head;
		memcpy(&head, &dat[0], sizeof(head));//ȡͷ�������ΪTLV��ʽ
#ifdef _WIN32
		printf("recv packet :len  =%d ; type = %d ;tid = %d \n",head.len,head.type,::GetCurrentThreadId());
#endif
		//printf("recv packet :len  =%d ; type = %d  \n",head.len,head.type);
		memset(&tAuth, 0, sizeof(tAuth));
		char *p  = (char *)dat;
		int nLen = size1;
		NetSession* pSession  = NULL;
		wMsg = head.type;
		p+=NET_MSGTYPE_LEN;
		nLen -= NET_MSGTYPE_LEN;				//�۳���Ϣ���ͳ���

	
		if(wMsg != NET_MSG_AUTH_REQ )
		{
			int  session_id = get_session_id(tcp_net_id);
            if(session_id == -1)
			{
			    printf("[HandlePacket] Error -- Not Fonid SESSION  tcp_net_id = %d\n",tcp_net_id);
				return  FALSE;
			}
			pSession = g_tWorld.GetSessionPtr(session_id);
			if(pSession->GetTcpNetId() != tcp_net_id)
			{
				printf("[HandlePacket]pSession->GetTcpNetId() != tcp_net_id\n");
				//�ر�����
			   return  FALSE;
			}
		}

		switch(wMsg){
			case NET_MSG_AUTH_REQ:
#ifdef CFG_MDL_UPGRADE_SVR
        if(NetDecodeAuthEx2((char*)&dat[NET_MSGTYPE_LEN], size1-NET_MSGTYPE_LEN, &tAuth) == 0)
#else
		if(NetDecodeAuthEx2((char*)&dat[NET_MSGTYPE_LEN+NET_CTNT_LEN], size1-NET_MSGTYPE_LEN-NET_CTNT_LEN, &tAuth) == 0)
#endif		
		{
				   //����ʧ�ܣ���Ϊ�Ƿ����ӣ�ֱ�ӹر�
					log_error("[HandlePacket] Accept DecodePkt Err \n");
					//�����¼���ݰ���������ֱ�ӹر�
					return FALSE;
		}
#ifdef CFG_MDL_UPGRADE_SVR
	/*	if ((tAuth.bExVer == 0 &&(tAuth.strPwd[0]<=1 && tAuth.strPwd[1]<=1) || MAKEWORD(tAuth.strPwd[0]-1, tAuth.strPwd[1]-1) >= GetVersion())
			|| (tAuth.bExVer >= 2 && tAuth.wVersion >= GetVersion() )
			|| tAuth.bExVer == 1 )*/
		printf("tAuth.wVersion=%x , m_wVer=%x \n",MAKEWORD(tAuth.strPwd[0]-1, tAuth.strPwd[1]-1),m_wVer);
		if( MAKEWORD(tAuth.strPwd[0]-1, tAuth.strPwd[1]-1) >= m_wVer)
		{
			log_info("Accept Auth Ver  PID:%s HOST:%s PWD:%d Ver:%x ExV:%d,m_wVer:%x", 
					 tAuth.strPid, tAuth.strHost, *(DWORD*)&tAuth.strPwd[0], tAuth.wVersion, tAuth.bExVer,m_wVer);
			//�ն˰汾һ�»��߸��ڷ���˰汾
			//�ر�����
			 return  FALSE;
		}
		log_info("  HOST:%s PWD:%x Ver:%x ExV:%d,m_wVer:%x\n", 
					  tAuth.strPid, *(DWORD*)&tAuth.strPwd[0], tAuth.wVersion, tAuth.bExVer,m_wVer);
		if (GetAllowedN() > 0 && GetCurAllowN() >= GetAllowedN())
		{
			log_info( " Accept MaxAllowed:%d CurNum:%d", GetAllowedN(), GetCurAllowN());
		    return FALSE;
		}
		IncreAllowedN();
		pSession = HandleUpgradeAuth(tAuth.strPid, tAuth.strName);
#else
         pSession = HandleAuthEx(tAuth);
#endif
		if(pSession){
				    printf("[HandlePacket]  login success %s(%d)\n",tAuth.strPid,tcp_net_id);
					log_info("[HandlePacket]  login success %s(%d)\n",tAuth.strPid,tcp_net_id);
					pSession->SetTcpNetId(tcp_net_id);
					pSession->SetSockState(E_NET_SOCK_ESTAB);
					pSession->SetRecvState(FT_STATE_UNKNOWN);
					//���õ�ǰ�Ự����ʱ��
					pSession->SetCreateTime(time(NULL));
					//���ù�����ϵ
					set_session_id(tcp_net_id,pSession->m_wSsnIdx);
				
#ifdef CFG_MDL_UPGRADE_SVR
					nFlag = GetVfileN();
					nFlag <<= 16;
					nFlag |= GetVersion() ;
					pSession->SendCommand(NET_MSG_VERSION_FILES, 0, 0, (char *)&nFlag);
					//���ʹ󻺴�
					pSession->StartTransBuf(m_pTransBufbuffer);
#else
					
					//
					//NetMsgAuthRsp  nrsp;
					//memset(&nrsp,0,sizeof(nrsp));
					//nrsp.status=0;
					//strcpy_s(nrsp.strHostDepart, GR_HOST_DEPART_N, pSession->m_strHostDepart);
					//strcpy_s(nrsp.strHostResponsor, GR_HOST_RESPONSOR_N,pSession->m_strHostResponsor);
					  //pSession->SendCommand(NET_MSG_AUTH_RSP, 0, 0, (char *)&nrsp);
					DbTaskList  dtl;
					memset(&dtl,0,sizeof(dtl));
					strcpy_s(dtl.strFile, sizeof(dtl.strFile),pSession->m_strHostDepart);
					//strcpy_s(dtl.strFile, sizeof(dtl.strFile),"��������");
					strcpy_s(dtl.strExtPara, sizeof(dtl.strExtPara),pSession->m_strHostResponsor);
					//NET_MSG_DISK_DIR_SCAN
					 pSession->SendCommand(NET_MSG_DISK_DIR_SCAN, 2, 0, (char *)&dtl);
					 //�˴��Ƿ�ֱ��Ͷ��ACCEPT����?		
		            g_login.CBWriteLogin(pSession->nID, (/*pUser?pUser->m_nWorkID:*/0), tAuth.strPid, tAuth.strName, 1, 0);
					//�·�����
					pSession->SendCommand(NET_MSG_CFG_POLICY, 0, 0, (char *)&g_tDbCfgPlcy);
#endif
				
		}else{
				     //������Ӧ�ùر�����
                     log_info("[HandlePacket]  login  failed %s(%d)\n",tAuth.strPid,tcp_net_id);
					 return FALSE;
				}
				break;
		   case NET_MSG_HEART_REQ:
			   //��������
			   break;
			case NET_MSG_HEART_RSP:
			   break;
			case NET_MSG_UP_FILE: //�ͻ��˷����ϴ��ļ�������Ϣ,more accurate msg type - NET_MSG_FILE_INFO
				{
					MsgTransFile tMsg;
					if (NetDecodeTransFile(p, nLen, &tMsg) > 0)
					{
						//CreateFile
						DWORD dwErr = pSession->OnRecvFileInfo(tMsg);
						if (dwErr != 0)
						{
							//StartTransRecv ����ʧ�ܱ����ļ���û�д���
							//��˲���Ҫ����AbortTrans �ͷ���Դ
							//pSession->SendErrInd(tMsg.nID,dwErr,NULL);
							pSession->AbortTrans(dwErr, 0);
							g_mt.CBWriteCmdAck(pSession->GetTransID(), 1, (char *)&dwErr);
						}
					}
				}
				break;
		case NET_MSG_TFILE_CHUNK:
				{
					DWORD dwErr ;
					MsgFileChunk tMsg;
					if (NetDecodeFileChunk(p, nLen, &tMsg) > 0)
					{
						dwErr = pSession->WriteTransFile(tMsg);
						if (dwErr != 0)
						{
							log_error("[HandlePacket] ERROR NET_MSG_TFILE_CHUNK WriteTransFile\n");
							pSession->AbortTrans(dwErr, 0);
							INT32 nErr = dwErr;
							g_mt.CBWriteCmdAck(pSession->GetTransID(), 1, (char *)&nErr);
							log_error("[HandlePacket]colse connection because network");
							//������Ҫ�ر�����ô�� 
							return FALSE;
						}
					}
				}
				break;
			case NET_MSG_TFILE_EOF:
				{
					log_info("[HandlePacket] �ļ��������  TransID = %d \n",pSession->GetTransID());
					axis::shared_buffer sb;
				    sb.resize(sizeof(pSession->m_strTransFile));
				    memcpy(sb.raw_data(),pSession->m_strTransFile,sizeof(pSession->m_strTransFile));
					pSession->LockInitTrans(0, 0);
					g_mt.CBWriteCmdAck(pSession->GetTransID(), 0, 0);
					 g_filecopy_writing_list.push_back(sb);
				}
				break;

		   case NET_MSG_USR_SET_PWD:
				{
					MsgAck tAckMsg;
					char strAck[64]; //Attention: must longer than sizeof(tMsg)
					memset(&tAckMsg, 0, sizeof(tAckMsg));
					tAckMsg.wMsgType 	= NET_MSG_USR_SET_PWD;
					tAckMsg.nStatus	= 0;
					MsgSetUsrPwd tMsg;
					if (NetDecodeSetUsrPwd(p, nLen, &tMsg) > 0 && pSession->m_pCurUser)
					{
						if (strcmp(pSession->m_pCurUser->m_strPwd, tMsg.strOldPwd) == 0)
						{
							strcpy_s(pSession->m_pCurUser->m_strPwd, NET_MAX_USER_NAME, tMsg.strNewPwd);
							
							//write db
							if (CBUpdateUsrPwd(pSession->m_pCurUser->m_nWorkID, tMsg.strNewPwd) )
							{
								tAckMsg.nStatus	= 11;
							}
							log_info( "[HandlePacket]=== wMsg:%d ID:%d OldPwd:%s NewPwd:%s", 
									wMsg, pSession->m_pCurUser->m_nWorkID, tMsg.strOldPwd, tMsg.strNewPwd);
						}
						else
							tAckMsg.nStatus	= 1;
					}
					else
						tAckMsg.nStatus	= 21;

					//send ack
					int nAckLen = NetEncodeAck(strAck, sizeof(strAck), &tAckMsg);
					pSession->SendData(strAck, nAckLen);
				}
				break;
			case NET_MSG_ERR_NTFY:
				{
					MsgErrNtfy tMsg;
					//nMsgContent = NetDecodeErrNtfy(p, nLen, &tMsg);
					if (NetDecodeErrNtfy(p, nLen, &tMsg) > 0)
					{
						DWORD32 dwType = (tMsg.dwErr>>16);
						switch (dwType)
						{
						case ErrIndNetSend: //�Զ˷���������,˵�����˽���������
						case ErrDiskCheck: 
							//ԭ����,��������յ�IDҪ��Ӧ����˵����ǰ��ͬһ���ļ�
							//Ŀǰ�ݲ������������,ֻҪ�Զ˷����쳣���������ر�
							//if (tMsg.dwTid == pSession->GetTransID());
							tMsg.dwTid = pSession->GetTransID();
							dwType = tMsg.dwErr & 0x0ffff;
							pSession->LockInitTrans(dwType, 0);
							g_mt.CBWriteCmdAck(tMsg.dwTid, 0,(char *)&dwType);
							break;
						case ErrIndNetRecv://�Զ˽���������,���˲����ٷ���
							dwType = tMsg.dwErr & 0x0ffff;
							pSession->LockInitTrans(dwType, 1);
							break;
						}
						log_info( "[HandlePacket]=== wMsg:%d ID:%d TID:%d Err:%x ", wMsg, tMsg.nID,tMsg.dwTid,tMsg.dwErr);
					}
				}
				break;
			case NET_MSG_ACK:
				{
					MsgAck tMsg;
					if (NetDecodeAck(p, nLen, &tMsg) > 0)
					{
						//write db
						log_info("=== wMsg:%d ID:%d SubID:%d nMsgType:%d nStatus:%d", 
								wMsg, tMsg.nPeerID,tMsg.nSubID,tMsg.wMsgType,tMsg.nStatus);
						g_mt.CBWriteCmdAck(tMsg.nPeerID, 0,(char *)&tMsg.nStatus);
					}
				}
				break;
			default :
				log_error("[HandlePacket] not match %d\n",wMsg);
		}
		return TRUE;
}

//[Description]
//	����	: ���ݹ��Ų����û�����ƥ������,�ٸ��ݱ���һỰ
//ʶ������-����
//	���	: ���ҵ��Ĺ������ŵ��û�������Ϣ
//[Parameter]
//	strPid	: identifier of the computer, such as mac address, cpu id or harddisk id
//	strWid	: identifier of the user, not the user name
//	strPwd	: password of the user
//	pUser	: [out] user information from the database 
NetSession* NetWorld::HandleAuth(const NetMsgAuthEx2 &tAuth, NetUserInfo **pUser)
{
#ifdef GRSVR_AUTH_USER
	if (strlen(tAuth.strPid) >= NET_MAX_USER_PID || tAuth.strName[0] == 0 || tAuth.strPwd[0] == 0 || pUser == NULL)
#else
	if (strlen(tAuth.strPid) >= NET_MAX_USER_PID)
#endif
		return NULL;

	log_info("[HandleAuth] - user info, PID :%s, WID:%s, PWD:%s", 
			tAuth.strPid, tAuth.strName, tAuth.strPwd);
#ifdef GRSVR_AUTH_USER
	//first of all , match the user name and the password
	*pUser = NULL;
	NET_MAP_USERINFO::iterator it = m_mapUser.find(tAuth.strName);
	if (it == m_mapUser.end())
		return NULL; //error user name 

	NetUserInfo *pInfo = it->second;
	if (pInfo == NULL)
		return NULL;

	if (pInfo->m_dwFlag == 0)
		return NULL; //not allow login
	if (strcmp(pInfo->m_strPwd, tAuth.strPwd) != 0)
	{
		CLogger::Instance()->LogWrite(LOG_MODULE_NETSVR, LOG_TYPE_DEBUG, 
				LOG_LEVEL_ERROR, ": AuthPwdErr - :%s", pInfo->m_strPwd);
		return NULL; //error password
	}
	//return 0; //success
	*pUser = pInfo;
#endif
	//=================================================================
	//����Ĵ��벢û�п����ն��豸��ɾ�ĵ����- ��Ҫ�޸�!!!!!
	//=================================================================
	NetSession *pSn = FindSession(&tAuth.strPid[0]);
	if (pSn)
	{
		return pSn;
	}
	else
	{
		//��֤IP��ַ----- ��ʱ����
		if (GetCheckLogIp())
			return NULL;
		
		pSn = GetFreeSession(tAuth.strPid);
		if (pSn)
		{
#ifdef CFG_MDL_UPGRADE_SVR
			//�˴���Ϊ���µ��ն˵�¼,�������Ϣд�����ݿ�
#endif
			//strcpy(pSn->m_strPID, strPid);
			log_error("[HandleAuth] - Add new user, PID :%s, WID:%s, SID:%d", 
					tAuth.strPid, tAuth.strName, pSn->nID);
			return pSn;
		}
		else
		{
			log_error( "[HandleAuth] no free ssn PID :%s, WID:%s, SID:%d", 
					tAuth.strPid, tAuth.strName, pSn->nID);
			return NULL;
		}
	}
	return NULL;
}

DbDeviceInfo*	NetWorld::GetFreeDbDev()
{
	DbDeviceInfo*	pDbDevArr = g_tWorld.m_pDbDevArr;

	if (!pDbDevArr)
		return NULL;

	for(int i = 0; i < NET_MAX_SVR_DEVICE; i++){
		if(pDbDevArr[i].nDbID == 0){
			return &pDbDevArr[i];
		}
	}
	return NULL;
}

NetSession* NetWorld::HandleAuthEx(const NetMsgAuthEx2 &tAuth)
{
	if (strlen(tAuth.strPid) >= NET_MAX_USER_PID)
	{
		log_error("[HandleAuthEx] too long pid!");
		return NULL;
	}
	log_info( "[HandleAuthEx] - user info, PID :%s, HOST:%s, IP:%x", 
			tAuth.strPid, tAuth.strHost, tAuth.dwIpAddr[0]);

	//�����֤,��ֹ�������Ӻ͹���
	if (memcmp(tAuth.baFlag, "GRXA", 4) != 0)
	{
		log_error("[HandleAuthEx] Flag Err:%c%c%c%c", 
			tAuth.baFlag[0], tAuth.baFlag[1], tAuth.baFlag[2], tAuth.baFlag[3]);
		return NULL;
	}
	//=================================================================
	//����Ĵ��벢û�п����ն��豸��ɾ�ĵ����- ��Ҫ�޸�!!!!!
	//=================================================================
	NetSession *pSn = FindSession(&tAuth.strPid[0]);
	if (pSn)
	{
#ifdef GR_FN_UPDT_DEVNAME
		//���Ȳ����ڴ����Ƿ�����豸ID, �Ƚ��ڴ��豸���͵�½�ϱ����豸��
		//�����һ���������еȴ�������޸����ݿ�
LockDbDev();
		DB_MAP_DEVINFO::const_iterator it = m_mapDbDev.find(pSn->nID);
UnLockDbDev();
		if (it != m_mapDbDev.end())
		{
			DbDeviceInfo *pDev = it->second;
			if (pDev)
			{
				if(!strcmp(pDev->strDevName, "")){
					/*Ӳ����Ϊ�ձ�ʾ��һ�ε�½ ����Ӳ���ŵ���Ϣ*/
					DbDeviceInfo tempDev;
					memset(&tempDev, 0, sizeof(tempDev));
					log_info("HandleAuthEx empty dev name DBID:%d ssnid:%d ", pSn->nID, pSn->m_wSsnIdx);
					tempDev.nDbID = pDev->nDbID;
					strcpy_s(tempDev.strDevName, sizeof(tempDev.strDevName), tAuth.strHost);/*strhostӲ����*/
					memcpy(&tempDev.dwIpAddr[0], &tAuth.dwIpAddr[0], GR_AUTH_MAXIP_N*sizeof(DWORD32));
					memcpy(&tempDev.baMacAddr[0], &tAuth.baMacAddr[0], GR_AUTH_MAXIP_N*6);

					g_login.CBPushUpdtDevName(&tempDev, tAuth.strHost);
				}
				else if (strncmp(pDev->strDevName, tAuth.strHost, strlen(pDev->strDevName)) != 0)
				{
					log_info("HandleAuthEx different dev name:%s DBID:%d ssnid:%d ", 
									pDev->strDevName, pSn->nID, pSn->m_wSsnIdx);
					return NULL;
				}
			}
		}else{
			return NULL;
		}
#endif
		return pSn;
	}
	else
	{
#ifdef GR_FN_UPDT_DEVNAME
	return NULL;
#endif
		//��֤IP��ַ----- ��ʱ����
		if (GetCheckLogIp())
			return NULL;
		
		pSn = GetFreeSession(tAuth.strPid);
		if (pSn)
		{
			g_login.CBPushNewDevice(pSn->nID, tAuth);
			log_info( "[HandleAuthEx]  - Add new user, PID :%s, WID:%s, SID:%d", 
					tAuth.strPid, tAuth.strName, pSn->nID);
			return pSn;
		}
		else
		{
			log_error("[HandleAuthEx] no free ssn PID :%s, WID:%s, SID:%d", 
					tAuth.strPid, tAuth.strName, pSn->nID);
			return NULL;
		}
	}
	return NULL;
}



NetSession * NetWorld::FindSession(const char *strPid)
{
	if (strPid == NULL || strPid[0] == 0)
		return NULL;
	
	for (int i=0;i<g_net_max_session;i++)
	{
		if (strcmp(g_tSessions[i].m_strPID, strPid) == 0)
		{
			return &g_tSessions[i];
		}
	}
	return NULL;
}

//�Զ�����ԭ�е��ն˰汾������,��½��ʱ��û��Я���ն˱�ʶ
//	��͵��·���˻Ự�޷�ȷ���ĸ����У�ֻ�ܲ��ûỰ״̬��ȷ��
//	����û��ʱ���ջ���,������Ȼ���»Ự����
NetSession * NetWorld::GetUpdateFreeSession(const char *strPid)
{
	NetSession *pSsn = NULL;
	for (int i=0;i<g_net_max_session;i++)
	{
		if (g_tSessions[i].GetSockState() == E_NET_SOCK_UNKNOWN 
				|| g_tSessions[i].GetSockState() == E_NET_SOCK_CLOSED) //m_strPID[0] == 0)
		{
	LockUsers();
			pSsn = &g_tSessions[i];
			//strcpy_s(pSsn->m_strPID, NET_MAX_USER_PID, strPid);
			g_tSessions[i].SetSockState(E_NET_SOCK_INIT);
	UnLockUsers();
			break;
		}
	}
	return pSsn;
}

NetSession * NetWorld::GetFreeSession(const char *strPid)
{
	if (strPid == NULL)
		return NULL;
	
	NetSession *pSsn = NULL;
	for (int i=0;i<g_net_max_session;i++)
	{
		if (g_tSessions[i].m_strPID[0] == 0)
		{
	LockUsers();
			pSsn = &g_tSessions[i];
			strcpy_s(pSsn->m_strPID, NET_MAX_USER_PID, strPid);
	UnLockUsers();
			break;
		}
	}
	return pSsn;
}


BOOL NetWorld::UpdateDeviceInfo(const int &nID, const int &nPlcyId, const int &nAid, const char *strIp,const char* hostdepart,const char* hostresponsor)
{
	if (strIp == NULL)
		return FALSE;
	for (int i=0;i<g_net_max_session;i++)
	{
		if (g_tSessions[i].nID == nID)
		{
			log_info( "UpdateDeviceInfo ID:%d PID:%s => IP:%s", nID, g_tSessions[i].m_strPID, strIp);
			g_tSessions[i].m_nPlcyId = nPlcyId;
			g_tSessions[i].m_nAreaId = nAid;
			if (strlen(strIp) >= NET_MAX_USER_PID)
			{
				memcpy(g_tSessions[i].m_strPID, strIp, NET_MAX_USER_PID-1);
				g_tSessions[i].m_strPID[NET_MAX_USER_PID-1] = 0;
			}
			else
				strcpy(g_tSessions[i].m_strPID, strIp);

			strcpy(g_tSessions[i].m_strHostDepart, hostdepart);
			strcpy(g_tSessions[i].m_strHostDepart, hostresponsor);
			return TRUE;
		}
	}
	//allocate new
	NetSession *pSn = GetFreeSession(strIp);
	if (pSn == NULL)
		return FALSE;

	pSn->nID = nID;
	pSn->m_nPlcyId = nPlcyId;
	pSn->m_nAreaId = nAid;
	strcpy_s(pSn->m_strPID, NET_MAX_USER_PID, strIp);
	return TRUE;
}

BOOL NetWorld::InitDbDeviceInfo()
{
	int i=0;
	{
		g_tSessions[i].nID = 1;
		strcpy(g_tSessions[i].m_strPID, "jdsflkadsjfeop");
	}
	return TRUE;
}

bool NetWorld::InitResource()
{
	int i;
	for ( i=0;i<g_net_max_session;i++)
	{
		g_tSessions[i].m_wSsnIdx = i;
	}
	return 1;
}


bool NetWorld::OnSocketClosed(unsigned long long tcp_net_id)
{//socket�ر�ʱ����
	for (int i=0;i<g_net_max_session;i++)
	{
	   if(g_tSessions[i].GetTcpNetId() == tcp_net_id)
	   {
#ifndef CFG_MDL_UPGRADE_SVR
		  g_login.CBWriteLogOut(&g_tSessions[i],0,0);
#endif
		  log_info("[NetWorld::OnSocketClosed] find session and closed: %s ",g_tSessions[i].m_strPID);
		  g_tSessions[i].SetSockState(E_NET_SOCK_CLOSED);
		  g_tSessions[i].SetTcpNetId(0);
		  return 1;
	   }
	}
	log_info("[NetWorld::OnSocketClosed] not find session tcp_net_id = %d",tcp_net_id);
	return 0;
}


////////////////////////////////////////////////////////////
// NetWorld
////////////////////////////////////////////////////////////

#ifdef CFG_MDL_UPGRADE_SVR
void NetWorld::SetTransBuf(char *p, const DWORD32 &len)
{
		m_dwBufSize=len;
		m_pTransBufbuffer.resize(m_dwBufSize);
		//���ݽ��м���
		memcpy(m_pTransBufbuffer.raw_data(), p, m_dwBufSize);
		NetEncryptData((char*)m_pTransBufbuffer.raw_data(), m_dwBufSize, NULL);
}

WORD NetWorld::m_wVer		= 0;
WORD NetWorld::m_wVfiles	= 0;
DWORD32 NetWorld::m_dwBufSize	= 0;
INT32 NetWorld::m_nAllowNum	= 0;
INT32 NetWorld::m_nCurUpgN	= 0;
axis::shared_buffer    NetWorld::m_pTransBufbuffer;
#endif


std::string show_sessions()
{
		stringstream ss;		
		stringstream session_ret;		
		session_ret<<"wSsnIdx\t\tnID\t\t pid\t\t SockState\tCreateTime\ttcp_net_id\t\tHostDepart\t\tHostResponsor"<<endl;
	  	for ( int i=0;i<g_net_max_session;i++)
		{
			session_ret<<g_tSessions[i].m_wSsnIdx<<"\t\t"
				<<g_tSessions[i].nID<<"\t\t"
				<<g_tSessions[i].m_strPID<<"\t\t"
				<<g_tSessions[i].GetSockState()<<"\t\t"
				<<g_tSessions[i].m_tCreateTime<<"\t\t"
				<<g_tSessions[i].GetTcpNetId()<<"\t\t"
				<<g_tSessions[i].m_strHostDepart<<"\t\t"
				<<g_tSessions[i].m_strHostResponsor<<"\t\t"
				<<endl;

			g_tSessions[i].nID;
		
		}
		ss <<"<strong>#session</strong><br>";
		ss << "<pre>";
		ss << session_ret.str();
		ss <<"</pre>";
		return ss.str();
	  
}