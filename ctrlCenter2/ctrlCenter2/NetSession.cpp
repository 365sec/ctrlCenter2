#include"NetSession.h"
#include"FileTrans.h"
#include "log_utility.h"
#include"boost/atomic.hpp"

extern void send_to_user(unsigned long long tcp_net_id,byte* data,size_t size);
extern void send_to_user(unsigned long long tcp_net_id, axis::shared_buffer& sb);

NetSession::NetSession()
{
	memset(this->m_strHostDepart,0,GR_HOST_DEPART_N);
	memset(this->m_strHostResponsor,0,GR_HOST_RESPONSOR_N);
	m_hTransFile = NULL;
}


NetSession::~NetSession()
{

}

char NetSession::s_strTaskDir[256] ;//T_TaskList
char NetSession::s_strNetTaskDir[256];
char NetSession::s_strCheckDir[256];//T_CheckVice
char NetSession::s_strZiChaDir[256];

//T_CheckVice_Backup
//T_TaskList_Backup


DWORD NetSession::SendData(const char * szData, int nDataLen, const WORD &wAtch ) {
	NetEncryptData((char*)szData, nDataLen, NULL);
    send_to_user(this->m_tcp_net_id,(unsigned char*)szData,nDataLen);
	return 0;
}

DWORD NetSession::SendDataEx(const char * szData, const int &nDataLen, const WORD &wAtch ) {
	//��Ϣ����
	NetEncryptData((char*)szData, nDataLen, NULL);
	send_to_user(this->m_tcp_net_id,(unsigned char*)szData,nDataLen);
	return 0;
}

DWORD NetSession::SendCommand(const int &nMsgType, const int &nID, const int &nSubID , const char *pPara){
	DWORD dwRet = 0;
	int nDataLen = 0;
	log_info("[SendCommand] tcp_id = %d  nMsgType= %d  nID = %d \n ",this->m_tcp_net_id,nMsgType,nID);
	printf("[SendCommand] tcp_id = %d  nMsgType= %d  nID = %d \n ",this->m_tcp_net_id,nMsgType,nID);
	if (nMsgType == NET_MSG_REG_MON_PLY || nMsgType == NET_MSG_FILE_MON_PLY)
	{
		if (pPara == NULL)
			return GR_ERR_PARA_STATE;
	}
	char szData[NET_MAX_BUF_LEN];
	char *pBuf = NULL;

	//�󲿷ּ򵥵���Ϣ�߱� ID/SUBID �ĸ�ʽ, ����д��ǰ����
	//	���в�����Ƕ������Ϣ��ṹ������,����Ĵ���ᱻ����
	*(WORD*)&szData[0] = (WORD)nMsgType;
	*(WORD*)&szData[NET_MSGTYPE_LEN] = (WORD)(NET_MSGTYPE_LEN*2 + NET_TIME_LEN*2);
	
	memcpy(&szData[NET_MSGTYPE_LEN*2], &nID, NET_TIME_LEN); 
	memcpy(&szData[NET_MSGTYPE_LEN*2+NET_TIME_LEN], &nSubID, NET_TIME_LEN); 
	nDataLen = (NET_MSGTYPE_LEN*2 + NET_TIME_LEN*2);

	switch(nMsgType)
	{
	case NET_MSG_NET_SVRONLY:
	case NET_MSG_NET_ENABLE:
	case NET_MSG_USB_ENABLE:
	case NET_MSG_USB_READONLY:
	case NET_MSG_USB_DISABLE:
	case NET_MSG_BLOCK_ALL:
	case NET_MSG_UNBLOCK_ALL:
	case NET_MSG_UP_PROCESS:
	case NET_MSG_UP_SERVICES:
	case NET_MSG_UP_SOFTWARE:
		
	case NET_MSG_HEART_REQ:
	case NET_MSG_HEART_RSP:
		//Я������int32 ��Ŀǰ��ʹ��
		break;
	case  NET_MSG_AUTH_RSP:
		nDataLen = NetEncodeAuthRsp(szData, sizeof(szData), (NetMsgAuthRsp*)pPara);
		break;
	case NET_MSG_SVR_IP_PORT:
		if (pPara == NULL)
			nDataLen = 0;
		else
		{
			//Ŀǰ����벢���ǿ��������ֽ��������
			nDataLen += sizeof(NetSvrAddrInfo);
			memcpy(&szData[NET_MSGTYPE_LEN*2+NET_TIME_LEN*2], pPara, sizeof(NetSvrAddrInfo));
			*(WORD*)&szData[NET_MSGTYPE_LEN] = (WORD)nDataLen;
		}
		break;
	case NET_MSG_WARN_LEVEL:
		nDataLen = NetEncodeWarnLvl3Policy(szData, sizeof(szData), (DbWarnLevel*)pPara);
		break;
	case NET_MSG_KEY3_POLICY:
		{
			if (((DbWarnLvStr*)pPara)->wMsgLen > NET_MAX_BUF_LEN - sizeof(DbWarnLvStr) - 30)
			{
				pBuf = new char[((DbWarnLvStr*)pPara)->wMsgLen + sizeof(DbWarnLvStr) + 30];
				if (pBuf == NULL)
				{
					nDataLen = 0;
					break;
				}
				nDataLen = NetEncode3KeyPolicy(pBuf, sizeof(szData), (DbWarnLvStr*)pPara);
			}
			else
				nDataLen = NetEncode3KeyPolicy(szData, sizeof(szData), (DbWarnLvStr*)pPara);
		}
		break;
	case NET_MSG_EXT_POLICY:
		//����Ҫ���г����ж�,���򻺳������
		nDataLen = NetEncodeExtPolicy(szData, sizeof(szData), (char*)pPara);
		break;
	case NET_MSG_CFG_POLICY:
		{
			DbCfgPlcy *ply = (DbCfgPlcy*)pPara;
			if (ply->tKeysPlcy 
				&& ply->tKeysPlcy->wMsgLen + sizeof(DbCfgPlcy) + 100 > NET_MAX_BUF_LEN )
			{
				int nLen = ply->tKeysPlcy->wMsgLen + sizeof(DbCfgPlcy) + 100;
				pBuf = new char[nLen];
				if (pBuf == NULL)
				{
					nDataLen = 0;
					break;
				}
				nDataLen = NetEncodeCfgPlcy(pBuf, sizeof(szData), (DbCfgPlcy*)pPara);
			}
			else
				nDataLen = NetEncodeCfgPlcy(szData, sizeof(szData), (DbCfgPlcy*)pPara);
		}
		break;
	case NET_MSG_SNAP_SCRN:
		{
			if ((dwRet = StartTransRecv(NULL, nID, 0)) != 0)
				return dwRet;
			MsgTransFile tMsg;
			memset(&tMsg, 0, sizeof(tMsg));
			tMsg.nID = nID;
			tMsg.nSubID = nSubID;
			nDataLen = NetEncodeSnapScrn(szData, sizeof(szData), &tMsg);
		}
		break;
	case NET_MSG_UP_FILE:
	case NET_MSG_UP_LOGCUR:
		{
			if ((dwRet = StartTransRecv(NULL, nID, 0)) != 0)
				return dwRet;
			MsgTransFile tMsg;
			memset(&tMsg, 0, sizeof(tMsg));
			tMsg.nID = nID;
			tMsg.nSubID = nSubID;
			if (pPara)
				strcpy_s(tMsg.strUpFile, sizeof(tMsg.strUpFile)/sizeof(tMsg.strUpFile[0]), pPara);
			nDataLen = NetEncodeTransFile(szData, sizeof(szData), &tMsg);
			*(WORD*)&szData[0] = (WORD)nMsgType;
		}
		break;
	case NET_MSG_DISK_CHECK:
		{
			if ((dwRet = StartTransRecv(NULL, nID, 0)) != 0)
				return dwRet;
			MsgTransFile tMsg;
			memset(&tMsg, 0, sizeof(tMsg));
			tMsg.nID = nID;
			tMsg.nSubID = nSubID;
			if (pPara)
				strcpy_s(tMsg.strUpFile, sizeof(tMsg.strUpFile)/sizeof(tMsg.strUpFile[0]), pPara);
			nDataLen = NetEncodeTransFile(szData, sizeof(szData), &tMsg);
			*(WORD*)&szData[0] = (WORD)nMsgType;
			m_nTransMID = nID;
		}
		break;
	case NET_MSG_DISK_DIR_SCAN:
		{
			DbTaskScanDisk tMsg;
			memset(&tMsg, 0, sizeof(tMsg));
			tMsg.nID = nID;
			tMsg.nSubID = nSubID;
			if (pPara)
			{
				strcpy_s(tMsg.strKeys, sizeof(tMsg.strKeys)/sizeof(tMsg.strKeys[0]), ((DbTaskList*)pPara)->strExtPara);
				strcpy_s(&tMsg.strKeys[1024-256-1],sizeof( g_tDbCfgPlcy.strFileTypes),  g_tDbCfgPlcy.strFileTypes);
				printf("::->%s",&tMsg.strKeys[1024-256-1]);
				strcpy_s(tMsg.strDir, sizeof(tMsg.strDir)/sizeof(tMsg.strDir[0]), ((DbTaskList*)pPara)->strFile);
			}
			nDataLen = NetEncodeDskSc(szData, sizeof(szData), &tMsg);
			*(WORD*)&szData[0] = (WORD)nMsgType;
		}
		break;
	case NET_MSG_USR_GET_SYSINFO:
		
		if ((dwRet = StartTransRecv(NULL, nID, 0)) != 0)
			return dwRet;
		
		break;
//	case NET_MSG_REG_MON_PLY:
		//nDataLen = NetEncodeDbLvlStr(szData, NET_MSG_REG_MON_PLY, (DbWarnLvStr*)pPara);
//		nDataLen = ((DbWarnLvStr*)pPara)->wMsgLen;
//		break;
//	case NET_MSG_FILE_MON_PLY:
		//nDataLen = NetEncodeDbLvlStr(szData, NET_MSG_FILE_MON_PLY, (DbWarnLvStr*)pPara);
//		nDataLen = ((DbWarnLvStr*)pPara)->wMsgLen;
//		break;
	case NET_MSG_VERSION_FILES:
		//��16Ϊ�ļ���Ŀ,��16λ�汾��
		*(WORD*)&szData[NET_MSGTYPE_LEN] = NET_MSGTYPE_LEN*2+sizeof(DWORD32);
		memcpy(&szData[NET_MSGTYPE_LEN*2], pPara, sizeof(DWORD32)); 
		nDataLen = NET_MSGTYPE_LEN*2+sizeof(DWORD32);
		break;
	case NET_MSG_SGL_NOTICE:
		nDataLen = (NET_MSGTYPE_LEN*2 + NET_TIME_LEN*2);
		if (pPara == NULL || strlen((char*)pPara) > 500)
			nDataLen = 0;
		else
		{
			nDataLen = strlen((char*)pPara);
			//�ַ�������
			*(WORD*)&szData[NET_MSGTYPE_LEN*2 + NET_TIME_LEN*2] = (WORD)nDataLen;
			//�����ַ���Ϣ
			memcpy(&szData[NET_MSGTYPE_LEN*2 + NET_TIME_LEN*2 + sizeof(WORD)], pPara, nDataLen);
			nDataLen += NET_MSGTYPE_LEN*2 + NET_TIME_LEN*2 + sizeof(WORD);
			*(WORD*)&szData[NET_MSGTYPE_LEN] = (WORD)nDataLen;
		}
		break;

	default :
		log_info("NetSession::SendCommand,%d;",nMsgType);
	}
	if (pBuf)
	{
		dwRet = SendDataEx(pBuf, nDataLen);
		delete []pBuf;
		return dwRet;
	}
	else
		return SendDataEx(szData, nDataLen);
	
	return 0;
}


//��������:������������Ӧ�Զ˵��ļ���������,�ϴ��Զ�ָ�����ļ�
//
//�����ļ����ͷ�Ϊ����;��: һ���ȷ�������Ϣ,����һ��ֱ�ӷ�������
//	Ŀǰ��ȡ��һ��
//[Return]
//	0	- sucess
//	>0	- error code
DWORD NetSession::StartTransSend(const char *strFile, const DWORD &dwFSize, const int &nTID)
{
	//StartTransFile - �����ļ�����
	//ԭ�������ֻ�ᵥ�̵߳��ã��ݲ�����ͬ�����⴦��
	if (strFile == NULL || !IsConnected())//|| dwFSize == 0 
		return GR_ERR_PARA_STATE;
	//ԭ���ϣ�Ӧ�õ�ǰ����ļ�������ϲ��ܽ��к�������
	//������߼�������Ҫ�޸�
	//�����ǰ�����ļ������У�����
	if (GetSendState() != FT_STATE_UNKNOWN)
	{
		
		log_info("StartTransSend AL IN SEND:%s State:%d size:%d TID:%d", 
					strFile, GetSendState(), dwFSize, nTID);
		return GR_ERR_AL_IN_SEND;
	}
	
	//???????????????????????????????????????????????????????????????????
	//EndTransFile();
	//AbortTrans(GR_ERR_FORCE_ENDSND, 1);
	//�����µĴ��������ַ�ʽ:һ����ǿ���ж�ǰ��Ĵ���
	//	һ���ǵȴ�ǰ����ļ��������
	//???????????????????????????????????????????????????????????????????

	DWORD32 nErr = 0;
	SetSendState(FT_STATE_TRANS_SEND);//m_nTransState = FT_STATE_TRANS;
	
	m_nTransMID = nTID;  //Ŀǰ��֧�����������ļ�ģʽ,ֻ��һ��TID
	m_dwSendPos = 0;
	m_hSendFile = CreateTransFile(strFile, nErr);
	if (m_hSendFile == NULL)
	{
		m_dwSendSize = 0;
		SetSendState(FT_STATE_UNKNOWN);
		if (nTID > 0)
			SendErrInd(nTID, ((ErrIndNetSend<<16)|(nErr&0x0ffff)), NULL);
		return nErr;
	}
		
	if (dwFSize > 0)
		m_dwSendSize =dwFSize;
	else{
		fseek(m_hSendFile, 0L, SEEK_END);   
		m_dwSendSize =ftell(m_hSendFile);  
	}
	
	MsgTransFile tMsg;
	memset(&tMsg, 0, sizeof(tMsg));
	tMsg.nID = nTID;
	tMsg.dwFileSize = m_dwSendSize;
	char strBuf[1600];
	memset(strBuf, 0, sizeof(strBuf));
	int nDataLen = NetEncodeTransFile(strBuf, sizeof(strBuf), &tMsg);
	log_info( "[StartTransSend] :%s Err:%d size:%d TID:%d", 
					strFile, nErr, m_dwSendSize, nTID);

	return SendData(strBuf, nDataLen,0 /*GR_NET_TRANS_FILE_M*/);
}


//[Parameter]
//	dwSize	- ����Զ˷����ļ���ʱ����ܱ�����֪���ļ��Ĵ�С
//[Return]
//	0	- sucess
//	>0	- error code
DWORD NetSession::StartTransRecv(const char *strFile, const INT32 &nCmdID, const DWORD32 &dwSize)
{
	if (!IsConnected())
	{
		log_info("[StartTransRecv] Err State:%d Err - ReqID:%d Size:%d TID:%d", 
					GetRecvState(), nCmdID, dwSize, GetTransID());
		return GR_ERR_PARA_STATE;
	}
	if (GetRecvState() != FT_STATE_UNKNOWN )
	{
		log_info("[StartTransRecv] AL IN RCV:%d ReqID:%d Size:%d TID:%d", 
					GetRecvState(), nCmdID, dwSize, GetTransID());
		return GR_ERR_AL_IN_RECV;
	}

	SetRecvState(FT_STATE_TRANS_RECV);

	if (strFile == NULL)
		GetTransFileByTid(nCmdID, m_strTransFile, 256);
	else
		strcpy_s(m_strTransFile, 256, strFile);
	
	//����ط�Ҫʵ��һ����ƽ̨�ļ���д����
	m_hTransFile  = fopen(m_strTransFile, "wb");  
	log_info("[StartTransRecv]info:RECVFileinfo   %s\n",m_strTransFile);
	if (m_hTransFile == NULL)
	{
		m_hTransFile = NULL;
		SetRecvState(FT_STATE_UNKNOWN);
		log_error("StartTransRecv ID:%d SubID:%x Size:%d File:%s ", 
					nCmdID, m_nTransMID, m_dwFileSize, m_strTransFile);
		return  GR_ERR_ZERO_LAST_ERR;
	}
	else
	{
		m_dwFileSize 	= dwSize;
		m_dwCurPos		= 0;
		m_dwFileSeq		= 0;
		m_nTransMID		= nCmdID;

	}
	return 0;
}



//TRUE- success - FALSE -0
//�������ֵΪ0˵��Ϊ�������ļ�����;����Ϊ�쳣
BOOL NetSession::AbortTrans(const DWORD32 &nLastErr, const int &nDirection)
{
	//�жϴ���- �ر�Handle - ����EOF - �ͷ���Դ
	if (LockInitTrans(nLastErr, nDirection))
	{
		char strBuf[128];
		int nLen = 0;
		if (nLastErr == 0)
		{
			nLen = NetEncodeCommand(strBuf, sizeof(strBuf), NET_MSG_TFILE_EOF, GetTransID(), 0) ;
		}
		else
		{
			MsgErrNtfy tMsg;
			memset(&tMsg, 0, sizeof(tMsg));
			tMsg.dwTid = GetTransID(); 
			tMsg.dwErr = (nDirection?ErrIndNetSend:ErrIndNetRecv);
			tMsg.dwErr <<= 16 ;
			tMsg.dwErr |= (nLastErr&0X0FFFF);
			sprintf(tMsg.strMsg, "AbortTrans Ret Err:%d Dir:%d.", nLastErr, nDirection);
			nLen = NetEncodeErrNtfy(strBuf, sizeof(strBuf), &tMsg);
		}
		log_warning( "AbortTrans ErrCode:%d Len:%d", nLastErr, nLen);
		if (nLen > 0)
		{
			SendData(strBuf, nLen);
			return TRUE;
		}
	}
	return FALSE;
}


DWORD	NetSession::SendErrInd(const DWORD32 &dwTid, const DWORD32 &dwErr, const char *strMsg)
{
	MsgErrNtfy tMsg;
	memset(&tMsg, 0, sizeof(tMsg));
	tMsg.dwTid = dwTid; 
	tMsg.dwErr = dwErr; 
	if (strMsg)
		strcpy_s(tMsg.strMsg, sizeof(tMsg.strMsg), strMsg);
	char strBuf[300];
	int nLen = NetEncodeErrNtfy(strBuf, sizeof(strBuf), &tMsg) ;
	return SendData(strBuf, nLen, 0);
}

BOOL NetSession::LockInitTrans(const DWORD32 &nLastErr, const int &nDirection){
    this->SetRecvState(FT_STATE_UNKNOWN);
	m_dwCurPos = 0;
	if(m_hTransFile)
	{
	   fclose(m_hTransFile);
	   m_hTransFile= NULL;
	}
	return true;
}

FILE *  NetSession::CreateTransFile(const char *strFile, DWORD32 &nErr){
	if (strFile == NULL)
		return NULL;
	FILE * hFile = fopen(strFile,"rb");
	if (hFile == NULL)
	{
		log_error("CreateTransFile  :%s", strFile);
		return NULL;
	}
	return hFile;
}



void NetSession::GetTransFileByTid(const INT32 &nCmdID, char *strFile, const DWORD32 &dwLen)
{
	static boost::atomic<long long >  nstart(0);

	printf("[GetTransFileByTid]  nCmdID = %d \n",nCmdID);
	if(nCmdID == 100){
		time_t  now = time(NULL);//����Ψһֵ������ط�Ҫ�޸�
		 //�Բ�
		sprintf(strFile,   
			"%s%lld%lld.zicha", s_strZiChaDir,now,nstart.fetch_add(1));
	}else if ((nCmdID >> 29) == 0 )	//T_TaskList
		sprintf(strFile,   
					"%s%d.rizhi", s_strTaskDir, (nCmdID & DB_TASK_ID_MASK));
	else if ((nCmdID >> 29) == 1 )	//T_NetTaskList
		sprintf(strFile,  
					"%s%d.fileViewer", s_strNetTaskDir, (nCmdID & DB_TASK_ID_MASK));
	else	//T_CheckVice
		sprintf(strFile,  
					"%s%d.checkResult", s_strCheckDir, (nCmdID & DB_TASK_ID_MASK));

}

//[Parameter]
//	strBuf [in] - data buffer
//	dwOffset [in] - offset of data block in the source
//	dwLen [in] - length of the strBuf
//	nErr [out] - error code if error
//(char *strBuf, const DWORD &dwOffset, const DWORD &dwLen, int &nErr)
DWORD NetSession::WriteTransFile(MsgFileChunk &tMsg)
{
	if (GetRecvState() != FT_STATE_TRANS_RECV || m_hTransFile == NULL || tMsg.nFileLen == 0)
	{
		log_error( "[WriteTransFile]ERROR [strpid=%d]  WriteTransFile RecvState:%d Handle:%x,  tMsg.nFileLen:%d tMsg.nFilePos:%d \n ", this->nID,GetRecvState(), m_hTransFile, tMsg.nFileLen, tMsg.nFilePos);
		return GR_ERR_PARA_STATE;
	}

	//�������ID��ƥ������Ϊ��ͬһ����������
	//��ʵ�������ID��ƥ��,��������Ѿ��ǳ��쳣��,ԭ����Ӧ���жϴ���Ự��
	if (tMsg.nID != GetTransID())
	{
		log_error("[WriteTransFile]  Eror WriteTransFile TID:%d nID:%d dismatch\n",GetTransID(), tMsg.nID);
		return GR_ERR_TID_DISMATCH;
	}
	//�����ǵ��߳�д�ļ������Բ����ڻ��������
	//ͬ��������Ҫ�����ڶ��ļ�����Ĺرղ�����(���û����µ�¼�����쳣�رջ����ϴ��µ��ļ�)
	//�ڹرջ��ߴ��ļ������ʱ����Ҫ����(�������ļ�����仯��ʱ����л���)
	//????????????????????????????
	DWORD  nErr = 0;
	size_t	nRet = 0;
	//NetWork::LockFile();
    do{
		// 1.�������ݿ��ƫ��λ���Ƿ��뵱ǰʵ�ڵ�ƫ��һ��
		// 2.���������Ƿ�Խ��
		// 3.ԭ����Ҫ���Ƿ������ݿ���������֤�Ƿ��ж���
		log_info("[WriteTransFile] INFO strpid=%d RecvState:%d dwLen:%d tMsg.nFilePos:%d  m_dwCurPos = %d,tMsg.nID=%d,tMsg.nSeq=%d,tMsg.nSubID=%d\n", this->nID,GetRecvState(), tMsg.nFileLen, tMsg.nFilePos,m_dwCurPos,tMsg.nID,tMsg.nSeq,tMsg.nSubID);
		if (tMsg.nFilePos >= m_dwFileSize || m_dwFileSize == 0)
		{
			nErr = GR_ERR_RECV_EXCEED;
			log_error("[WriteTransFile]Error GR_ERR_RECV_EXCEED \n");
			break;
		}
		
		//����ע�ͣ����Կͻ����Ƿ��������
		// compare the block offset with the file offset
		if (tMsg.nFilePos == m_dwCurPos)
		{

		}else
		{
			//���뱣֤���̣߳�ƫ�Ʋ�һ�¾���Ϊ����
			nErr = GR_ERR_RECV_OFFSET;
			log_error("[WriteTransFile] [pid=%d]OffErr RecvState:%d dwLen:%d tMsg.nFilePos:%d  m_dwCurPos = %d\n", this->nID,GetRecvState(), tMsg.nFileLen, tMsg.nFilePos,m_dwCurPos);
			break;
		}
		
		if (m_dwCurPos+tMsg.nFileLen> m_dwFileSize)
		{
			//���ϴ���־��ʱ����ߵ��������,��Ϊ��־һֱ������
			nRet = fwrite( tMsg.strContent,  1 , m_dwFileSize - m_dwCurPos, m_hTransFile) ;
			if(nRet !=  m_dwFileSize - m_dwCurPos)
			{
			   printf("[WriteTransFile] nRet !=  m_dwFileSize - m_dwCurPos \n");
			   log_error("[WriteTransFile] nRet !=  m_dwFileSize - m_dwCurPos \n");
			}
		}
		else{
		
			nRet = fwrite( tMsg.strContent, 1,  tMsg.nFileLen, m_hTransFile) ;
			if(nRet !=  tMsg.nFileLen)
			{
			   printf("[WriteTransFile] nRet !=  tMsg.nFileLen \n");
			   log_error("[WriteTransFile] nRet !=  tMsg.nFileLen");
			}
		}
	
		//writer over! Maybe there are some chunks unreceived or undealed!!!!!!!!!!!!! ????????????????
		if (nRet <= 0 )
		{
			nErr = GR_ERR_ZERO_LAST_ERR;
			break;
		}
		else
		{
			m_dwCurPos += nRet;
			++m_dwFileSeq;
		}
	
		return 0;//dwWrite;

    }while(0);
	return nErr;
}


DWORD NetSession::OnRecvFileInfo(const MsgTransFile & tMsg)
{
	//�����ǰ�����ڽ����ļ�״̬,�������µĽ�������
	if (m_bRecvState == FT_STATE_UNKNOWN && tMsg.nID != 0)
	{
		StartTransRecv(NULL, tMsg.nID, tMsg.dwFileSize);
	}
	
	if (GetRecvState() != FT_STATE_TRANS_RECV || m_hTransFile == NULL || tMsg.dwFileSize == 0)
	{
		log_error("[OnRecvFileInfo] Error OnRecvFileInfo RecvState:%d Handle:%x, dwLen:%d nID:%d \n ", 
					GetRecvState(), m_hTransFile, tMsg.dwFileSize, tMsg.nID);
		return GR_ERR_PARA_STATE;
	}
	//�������ID��ƥ������Ϊ��ͬһ����������
	//��ʵ�������ID��ƥ��,��������Ѿ��ǳ��쳣��,ԭ����Ӧ���жϴ���Ự��
	if (tMsg.nID != GetTransID())
	{
		log_error("[OnRecvFileInfo] Error +++OnRecvFileInfo TID:%d nID:%d dismatch \n", GetTransID(), tMsg.nID);
		return GR_ERR_TID_DISMATCH;
	}
	m_dwCurPos = 0;
	log_info("[OnRecvFileInfo] OldLen:%d CurLen:%d nID:%d",m_dwFileSize, tMsg.dwFileSize, tMsg.nID);
	if (m_dwFileSize == 0 || m_dwFileSize == -1)
		m_dwFileSize = tMsg.dwFileSize;

	return 0;
}


DWORD NetSession::StartTransBuf(axis::shared_buffer& buffer)
{
	 send_to_user(this->m_tcp_net_id,buffer);	
	 return 0;
}