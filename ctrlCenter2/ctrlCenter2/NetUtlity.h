#pragma once	//qizc

#define CBUILDER_VER_6		//qizc.	
#define GA_STATIC_RELEASE	//qizc. ����Ϊ��̬��

#ifndef _NET_NET_UTLITY_H__
#define _NET_NET_UTLITY_H__

#ifdef GA_STATIC_RELEASE
#define GR_UTLITY_API 
#else
#ifdef GR_UTILITY_EXPORTS
#define GR_UTLITY_API __declspec(dllexport)
#else
#define GR_UTLITY_API __declspec(dllimport)
#endif
#endif


#include "NetDef.h"
#include <time.h>

#define NET_MSGTYPE_LEN		2
#define NET_CTNT_LEN		2
#define NET_TIME_LEN		4
#define GR_DB_WARN_LEVEL	3		//���ݿ��趨�ĸ澯�ȼ���Ŀ- �������ز�������Ķ�

#ifdef CBUILDER_VER_6
#define __time32_t 	time_t
#define _time32  	time

/*
   time_t timer;
   struct tm *tblock;
   // gets time of day 
   timer = time(NULL);
   // converts date/time to a structure 
   tblock = localtime(&timer);
   printf("Local time is: %s", asctime(tblock));
*/

#endif

//qizc
#ifndef MAX_PATH
#define MAX_PATH (260*2)
#endif

#ifndef DWORD32
typedef unsigned int DWORD32;
//#define DWORD32 unsigned int
#endif

// USB����״̬
enum CLIENT_USB_STATUS
{
	CLIENT_USB_INVALID	= 0x0,	//USB �����޷���(δ��װ)
	CLIENT_USB_ENABLE	= 0x1,	//USBʹ��״̬
	CLIENT_USB_DISABLE	= 0x2,	//
	CLIENT_USB_READONLY = 0x4,
	CLIENT_USB_IOERR	= 0x8	//���һ������ִ��ʧ��
};

// �������״̬
enum CLIENT_NET_STATUS
{
	CLIENT_NET_INVALID	= 0x0,	//NET �����޷���(δ��װ)
	CLIENT_NET_ENABLE 	= 0x1,	//NET����δ���ù��˵���������
	CLIENT_NET_DISABLE  = 0x2,	//��ֹ��������
	CLIENT_NET_SVRONLY  = 0x4,
	CLIENT_NET_IOERR	= 0x8	//���һ������ִ��ʧ��
};

// ����״̬
enum CLIENT_BLOCK_STATUS
{
	CLIENT_BLOCK_NOCMD	= 0x0,
	CLIENT_UNBLOCK_ALL	= 0x1,
	CLIENT_BLOCK_ALL	= 0x2,
	CLIENT_BLOCK_ERR	= 0x8	//���һ������ִ��ʧ��
};

// �ļ�����״ֻ̬��һ��״̬������״ֱ̬������NET_MSG_TYPE_E
enum CLIENT_TRANS_STATUS
{
	CLIENT_TRANS_NONE = 0x00
};

enum NET_MSG_TYPE_E
{
	NET_MSG_UNKNOWN = 0,
	NET_MSG_AUTH_REQ	 ,		//1�ͻ�����֤����	
	NET_MSG_AUTH_RSP	 ,		//2��֤��Ӧ	
	NET_MSG_TRANS_FILE	 ,		//3�����ļ�	
//	NET_MSG_KEY_POLICY	 ,		//4�ؼ��ʲ��Ա�
	NET_MSG_WARN_LEVEL 	 ,		//4�澯�ȼ����Ա�
	NET_MSG_KEY3_POLICY	 ,		//5�ؼ��ʲ��Ա�
	NET_MSG_EXT_POLICY	 ,		//6��׺���������Ա�	
	//ϵͳ��������
	NET_MSG_USB_INSERT	 ,		//07USB�����¼	
	NET_MSG_USB_REMOVE	 ,		//08USB�γ���¼	
	NET_MSG_USB_ENABLE	 ,		//09����USB	
	NET_MSG_USB_DISABLE	 ,		//10��ֹUSB	
	NET_MSG_USB_READONLY ,		//11ֻ��USB	
	NET_MSG_NET_ENABLE	 ,		//12�����������	
	NET_MSG_NET_SVRONLY	 ,		//13ֻ�����������ӷ�����	
	NET_MSG_NET_DISABLE	 ,		//14��ֹ����	
	NET_MSG_BLOCK_ALL	 ,		//15��ֹ�ն˵����в���(���̺����)
	NET_MSG_UNBLOCK_ALL	 ,		//16�ָ��ն˵����в���(���̺����)
	NET_MSG_SNAP_SCRN	 ,		//17�ϴ�����	
	
	NET_MSG_HEART_REQ	 ,		//18��������	
	NET_MSG_HEART_RSP	 ,		//19������Ӧ	
	NET_MSG_NTP_REQ	 	 ,		//20����ʱ������	
	NET_MSG_NTP_RSP	 	 ,		//21����ʱ����Ӧ	
	NET_MSG_CTRLSTATE_REQ,		//22��ȡ�ն˿���״̬����	
	NET_MSG_CTRLSTATE_RSP,		//23��ȡ�ն˿���״̬��Ӧ
	NET_MSG_SOFT_CLIVER	 ,		//24�ͻ�������汾	
	NET_MSG_SRCH_EXTSET	 ,		//25����������׺��	
	NET_MSG_SRCH_EXTGET	 ,		//26�����ȡ������׺��	
	NET_MSG_SRCH_EXTRPT	 ,		//27���浱ǰ������׺��	
	NET_MSG_UP_LOGCUR	 ,		//28�ϴ���ǰ��־	
	NET_MSG_UP_LOGDAY	 ,		//29�ϴ�ָ�������־	
	NET_MSG_UP_LOGALL	 ,		//30�ϴ�������־	
	NET_MSG_UP_LOG_EXT	 ,		//31�ϴ�ָ��Ҫ�����־	
	NET_MSG_UP_PROCESS	 ,		//32�ϴ������б�	
	NET_MSG_UP_SERVICES	 ,		//33�ϴ������б�	
	NET_MSG_UP_SOFTWARE	 ,		//34�ϴ����ذ�װ����б�	
	NET_MSG_UP_URLHIS	 ,		//35�ϴ���ҳ������ʷ	
	NET_MSG_UP_SCRNCUR	 ,		//36�ϴ���ǰ����	
	NET_MSG_UP_SCRNMEM	 ,		//37�ϴ��ڴ�����Ľ���	
	NET_MSG_UP_DIR		 ,		//38�ϴ�ָ���ļ���	
	NET_MSG_UP_FILE		 ,		//39�ϴ�ָ���ļ�	
	NET_MSG_TFILE_CHUNK	 ,		//40ָ���ļ���һ����	
	NET_MSG_TFILE_EOF	 ,		//41ָ���ļ���һ����	
	NET_MSG_ALARM_EVENT	 ,		//42�ϱ��澯�¼�
	NET_MSG_ALARM_LIST	 ,		//43�ϱ��澯�¼�
	NET_MSG_TFILE_CMD	 ,		//44�ļ�������صĿ�������
	NET_MSG_TFILE_CK_END ,		//45�ļ���������һ�����ݿ�	
	NET_MSG_TFILE_ACK	 ,		//46�ļ����������ȷ��,��Я��ԭ��ֵ
	NET_MSG_ALARM_NET	 ,		//47�ϱ�����澯�¼�
	NET_MSG_ALARM_REG	 ,		//48�ϱ�ע����޸ĸ澯�¼�
	NET_MSG_ALARM_FILECHG,		//49�ϱ��ļ��޸ĸ澯�¼�
	NET_MSG_DISK_CHECK	 ,		//50ר����
	NET_MSG_REG_MON_PLY	 ,		//51�·�ע����ؼ�ֵ�б�
	NET_MSG_FILE_MON_PLY ,		//52�·��ļ�����б� --- �����ļ�����б��ܳ�,����END��ʾ����
	NET_MSG_FILE_MON_END ,		//53
	NET_MSG_USR_SET_PWD  ,		//54�û�����������������
	NET_MSG_USR_GET_SYSINFO,	//55��������ն���Ϣ
	NET_MSG_DISK_DIR_SCAN,		//56�û��ն˴���/Ŀ¼�ؼ���ɨ��
	NET_MSG_LOGINOUT     ,		//57�û��ն˵�½�˳�,�����ϲ��¼
	NET_MSG_CTRL_STAS_REQ,		//58�·��ն˿���״̬����
	NET_MSG_CTRL_STAS_RSP,		//59�ն˿���״̬��Ӧ
	NET_MSG_TFILE_CHKEOF ,		//60ָ���ļ����һ����
	NET_MSG_VERSION_FILES,		//61�汾�ź��ļ���Ŀ
	NET_MSG_SEND_FILE	 ,		//62�����ļ�
	NET_MSG_TASK_OVER	 ,		//63����ͨ�����,�Ͽ�����
	NET_MSG_ERR_NTFY	 ,		//64�Զ˷��͵Ĵ���֪ͨ
	NET_MSG_SGL_NOTICE	 ,		//65�������Ϣ֪ͨ
	NET_MSG_BROADCAST	 ,		//66�㲥��Ϣ
	NET_MSG_FORCEUPDATE  ,		//67ǿ�ƿͻ�������
	NET_MSG_CUST_CMD     ,		//68�����Զ�������
	NET_MSG_SVR_IP_PORT  ,		//69���������IP��ַ�Ͷ˿ں�- ���Ʒ��������Զ����·�����
	NET_MSG_CFG_POLICY = 91,	//91 ȫ�ֲ�������(keyword filetypes usb net ctrl flag)
	
	NET_MSG_CUST_CMDREQ = 98 ,	//98�ն��Զ�����������
	NET_MSG_CUST_CMDRSP = 99  ,	//99�Զ���������Ӧ
	NET_MSG_ACK = 100	 ,		//100�ϱ��澯�¼�
	NET_MSG_TYPE_ERR	
};

// �������״̬����������ִ�����뼶��������״̬
enum CLIENT_OPT_STATUS
{
	CLIENT_OPT_NONE = 0x00,
	CLIENT_OPT_DISK_CHECK = NET_MSG_DISK_CHECK, // ר����
	CLIENT_OPT_GET_SYSINFO = NET_MSG_USR_GET_SYSINFO, // ���ϵͳ��Ϣ
	CLIENT_OPT_DISK_DIR_SCAN = NET_MSG_DISK_DIR_SCAN // �û��ն˴���/Ŀ¼�ؼ���ɨ��
};

enum DRIVER_STATUS
{
	DRIVER_NOT_EXITED = 0, DRIVER_INITIALIZED, DRIVER_STARTED, DRIVER_LOADED, DRIVER_REMOVED
};
typedef struct NET_MSG_DECODE_T
{
	WORD	wMsgType;
	int		(*decode)(char *pData, int &nLen, void *pPara);
}NetMsgDecode;
//NetMsgHandler
typedef struct NET_MSG_AUTH_T
{
//	BYTE	bPidLen;
//	BYTE	bNameLen;
//	BYTE	bPwdLen;
//	BYTE	bReserved;	// 4 bytes align boudary
	char	strPid[NET_MAX_USER_PID];
	char	strName[NET_MAX_USER_NAME];	//Ŀǰ���湤��
	char	strPwd[NET_MAX_USER_NAME];
	WORD	wVersion;
	BYTE	bPidType;
}NetMsgAuth;

#define		GR_AUTH_MAXIP_N	4		//��¼��֤��ʱ��Я�������IP��ַ��Ŀ
#define		GR_AUTH_FLAG_N	4		//GRXA
typedef struct NET_MSG_AUTH_EX_T
{
	char	strPid[NET_MAX_USER_PID];	//Ŀǰ����Ӳ�����кŻ���GUID
	char	strName[NET_MAX_USER_NAME];	//Ŀǰ���湤��
	char	strPwd[NET_MAX_USER_NAME];
	WORD	wVersion;
	BYTE	bPidType;
	BYTE	bExVer;							//��������İ汾
	BYTE	baFlag[GR_AUTH_FLAG_N];		//��Ϣ���ı�ʶ,��ֹ���⹥��
	BYTE	baAddrType[GR_AUTH_MAXIP_N];	//IPV4 ��ַ����-
	DWORD32	dwIpAddr[GR_AUTH_MAXIP_N];		//���Я��4��IPV4�ĵ�ַ
	BYTE	baMacAddr[GR_AUTH_MAXIP_N*6];	//4*6=24 -> 4��IP��ַ��Ӧ��MAC��ַ
}NetMsgAuthEx;

///////////////////////////////////////////////////////////////////////////////////////
//add by cy
//��֤��ӦЭ��
#define GR_HOST_DEPART_N  200
#define GR_HOST_RESPONSOR_N  200
typedef struct NET_MSG_AUTH_RSP_T
{
	WORD	status;
	char	strHostDepart[GR_HOST_DEPART_N];	//Ŀǰ���湤��
	char	strHostResponsor[GR_HOST_RESPONSOR_N];
}NetMsgAuthRsp;
//////////////////////////////////////////////////////////////////////////////////////////

typedef struct NET_MSG_AUTH_EX_2_T
{
	char	strPid[NET_MAX_USER_PID];	//Ŀǰ����Ӳ�����кŻ���GUID
	char	strName[NET_MAX_USER_NAME];	//Ŀǰ���湤��
	char	strPwd[NET_MAX_USER_NAME];
	WORD	wVersion;
	BYTE	bPidType;
	BYTE	bExVer;							//��������İ汾
	BYTE	baFlag[GR_AUTH_FLAG_N];		//��Ϣ���ı�ʶ,��ֹ���⹥��
	BYTE	baAddrType[GR_AUTH_MAXIP_N];	//IPV4 ��ַ����-
	DWORD32	dwIpAddr[GR_AUTH_MAXIP_N];		//���Я��4��IPV4�ĵ�ַ
	BYTE	baMacAddr[GR_AUTH_MAXIP_N*6];	//4*6=24 -> 4��IP��ַ��Ӧ��MAC��ַ
	char	strHost[256];					//������-gethostname
}NetMsgAuthEx2;
typedef struct NET_MSG_USR_SET_PWD_T
{
	INT32	nSeq;							//�ն˷��͵���Ϣ���,����ȷ�϶�λ
	char	strOldPwd[NET_MAX_USER_NAME];	//Я����������
	char	strNewPwd[NET_MAX_USER_NAME];	//Я������Ҫ�޸ĵ�����
}NetMsgSetPwd;

typedef struct NET_MSG_USB_ACT_T
{
	time_t tmEvent;					//��Ϣ��ȡʱ��
	char szFSName[64];				//�ļ�ϵͳ����
	char szVolume[64];				//���	
	char szFriendName[MAX_PATH];	//�豸�Ѻ���
	char cDrive;					//==0��ʾ������ >0��ʾ���� 'A'
	BYTE nType;						//�������ͣ�0-Ӳ��/1-U��
	BYTE nActInsert;				//0 �γ� 1����
}MsgUsbAct;

#define NET_IP_LEN				20
typedef struct NET_LOGIN_INFO
{
	NetMsgAuth	tAuth;				// ��¼��Ϣ
	char		szServerIP[20];		// ������IP��ַ
	int			iPort;				// �������˿ں�
}NetLoginInfo, *PNETLOGININFO;

typedef struct NET_SVRADDR_INFO_T
{
	DWORD32		dwSvrIp;			//���Ʒ�������ַ- Ŀǰֻ֧��IPV4
	DWORD32		dwUpdtIp;			//�Զ����·�������ַ
	WORD		wSvrPort;
	WORD		wUpdtPort;
}NetSvrAddrInfo;

#define WARN_KEY_LEVEL_1		1	//��ɫ�澯
#define WARN_KEY_LEVEL_2		1	//��ɫ�澯
#define WARN_KEY_LEVEL_3		1	//��ɫ�澯

//�ؼ��ʸ澯����
//typedef struct _MSG_KEYWORD_POLICY_T
//{
//	int		nID;				//���ݿ��ʶ
//	int		nLevel;				//�澯�ȼ�
//	int		nVer;				//�汾
//	int		nType;				//�澯ģ������
//	char	strKeys[128];		//�ؼ����б� ; ����
//}MsgKeyPolicy;
//��׺������
//typedef struct _MSG_EXTNAME_POLICY_T
//{
//	int		nID;				//���ݿ��ʶ
//	int		nVer;				//�汾
//	int		nType;				//�澯ģ������
//	char	strName[128];		//extension names�б� ; ����
//}MsgExtPolicy;

//File Transfer
typedef struct _MSG_SEND_FILE_INFO_T
{
	INT32	nID;				//���˵����
	INT32	nPeerTid;			//�Զ˵Ĵ����ļ����-����еĻ�
	DWORD32	dwFileSize;
	char	strUpFile[256]; 	//ָ���ķ��͵��ļ���(���Ի������·��)
}MsgSendFile;
typedef struct _MSG_TRANS_FILE_INFO_T
{
	INT32		nID;
	INT32		nSubID;
	DWORD32		dwFileSize;
	char	strUpFile[256]; 	//ָ������Ҫ�ϴ����ļ���(absolute path)
}MsgTransFile;
typedef struct _MSG_TRANS_COMMAND_T
{
	int		nID;
	int		nSubID;
	WORD	wType;	//file transferring control command
}MsgTransCmd;
typedef struct _MSG_TRANS_FILE_CHUNK_T
{
	INT32		nID;
	INT32		nSubID;
	DWORD32		nSeq;
	DWORD32		nFilePos;
	DWORD32		nFileLen;
	char	strContent[NET_MAX_PERDATA_N];
}MsgFileChunk;
typedef struct _MSG_SET_USR_PWD_T
{
	char	strOldPwd[NET_MAX_USER_NAME];
	char	strNewPwd[NET_MAX_USER_NAME];
}MsgSetUsrPwd;
typedef enum _ERR_IND_E
{
	ErrIndNetSend = 1,
	ErrIndNetRecv	,
	ErrDiskCheck	,
}MsgErrIndE;
typedef struct _MSG_NET_ERR_NTFY_T
{
	INT32	nID;				//��Ϣ���
	DWORD32 dwTid;				//�ļ���������еĶ�ӦID
//	WORD	wOprType;			//�������
//	WORD	wLastErr;			//GetLastError() etc
	DWORD32 dwErr;				//error code - ��16λΪ��������,��16λ������GetLastError()�õ�
	char	strMsg[250];		//error messge
}MsgErrNtfy;
typedef struct _MSG_ACK_T
{
	INT32		nPeerID;			//���ݿ��е�ָ��ID
	INT32		nSubID;
	INT32		nMsgSeq;
	INT32		nStatus;			//ִ�еĽ��
	WORD		wMsgType;			//��Ӧ�ķ��͵����������Ϣ
}MsgAck;

//==================================================
// DB ��ص����ݽṹ
//==================================================

//�ն˿���״̬
typedef struct _DB_CTRL_STATUS_T
{
	INT32		nID;
	INT32		nSubID;
	WORD		wRegDrvVer;		//������������汾
	WORD		wUsbDrvVer;		//USB���������汾
	WORD		wNetDrvVer;		//���������汾
	WORD		wOtherVer;		//���������汾-�ֽڶ���
	DWORD32		dwNetIpAddr;	//������������˵�ַ

	BYTE		bRcntUsbCmd;	//����·���USB��������
	BYTE		bUsbState;		//USB��������״̬
	BYTE		bRcntNetCmd;	//����·���NET��������
	BYTE		bNetState;		//������������״̬
	BYTE		bRcntBlckCmd;	//����·���BLOCK��������
	BYTE		bBlockState;	//�ն�����״̬
	BYTE		bTranSend;		//�ն��ļ�����send״̬
	BYTE		bTranRecv;		//�ն��ļ�����recv״̬
	
	BYTE		bCkState;		//���̼������,�ն���Ϣ�����������״̬
	BYTE		bReserved[3];	//�����ֶ� - 4bytes align
}DbCtrlStatus;

// һ��ͨ�õ���澯�ȼ���ص��ַ����ṹ
// DB -> T_RegistrySettings
typedef struct _DB_WARN_LEVEL_STR_T
{
//	WORD			wOffset1;		//�澯����1ƫ�����̶�Ϊ0, ���������ʶ
//	WORD			wOffset2;		//�澯����2 ��strMonitor �е�ƫ��
//	WORD			wOffset3;		//�澯����3 ��strMonitor �е�ƫ��
	WORD			wMsg;
	WORD			wMsgLen;		//Ϊ�˽�Լ���±��������ڴ�,��Encode ��Ӧ-�����ṹ��ʵ��ռ�õ��ڴ�
	WORD			waOffset[GR_DB_WARN_LEVEL-1];	//0->level2 | 1->level3
	WORD			wLen;			//���ڸı�Ϊ�����ṹ��ʵ��������ڴ� //strMonitor �ĳ���
	char 			strMonitor[0];
}DbWarnLvStr;
typedef union _DB_WARN_LVL_STR_U
{
	DbWarnLvStr		tLvl;
	BYTE*			baStr;
}DbWarnLvlU;

//��Ӧ��DB- T_KeyWordsSet T_FileType
//#define		DB_MAX_KEYWORDBUF_LEN	256
typedef struct DB_KEY_WORD_T
{
	INT32 	nIndex;			//��Ӧ�����ݿ��е�ID
	BYTE	bLevel;			//��Ӧ�ڵȼ�0-1-2; ������ĵ��������Ӧclassification 1-file 2-network
	BYTE	bGlobal;		//��Ӧ�����ݿ��е� isNoGlobal -  0(disable)  1(enable)
	//BYTE	bType;
	//BYTE	bReserved;
	//int nLevel;
	//DWORD dwTime
	char strKey[COMM_SRCH_KEY_LEN];
}DbKeyWord;

// DB -> T_WarnLevel
//�ؼ��ʸ澯����
typedef struct _DB_WARN_LEVEL_POLICY_T
{
	BYTE		bLevel;				//�澯�ȼ� 0-1-2
	BYTE		bNetCtrl;			// if disable network 0-false 1-true
	BYTE		bUsbCtrl;			// if disable usb 0-false 1-true
	BYTE		bAlarm;				// if send the alarm message to server
	BYTE		bShowWarn;			// field informationRemind -> if show warning message
	BYTE		bLogKept;			// field logKept
	BYTE		baReserved[6];
}DbWarnLevel;

// DB -> T_Strategy
typedef struct _DB_CFG_POLICY_T
{
	INT32			nDbId;
	INT32			nVersion;
	DWORD32			dwTime;
//	DbKeywordPlcy	tKeyPlcy[3];
	DbWarnLevel		tWarnLvl[GR_DB_WARN_LEVEL];		//�澯�ȼ�����
	DbWarnLvStr		*tKeysPlcy;						//�ؼ����б�
	char			strFileTypes[256];	//�ļ���׺�б�,��; ����
}DbCfgPlcy;

// DB -> T_Area
typedef struct _DB_TBL_AREA_T
{
	WORD			nDbId;				//����������65535����λ
	WORD			nFID;
	BYTE			bGrade;
	BYTE			bType;
	char			strFDNCode[32];	//�ļ���׺�б�,��; ����
}DbTblArea;

//////////////////////////////////////////////////
// ����ָ��ṹ
enum DB_TASK_STATUS_E
{
	EDB_TASK_INIT 		= 0,
	EDB_TASK_SEND 		= 2,
	EDB_TASK_SND_ERR 	= 3,
	EDB_TASK_OVER 		= 4,
	EDB_TASK_NO_RSP 	= 5,
	EDB_TASK_CLI_ERR 	= 6,	//Ŀǰ�ͻ����쳣����Ϊ�����޷�ִ��,����������ϸ��������
	EDB_TASK_SVR_ERR 	= 7,	//������쳣,����������ϸ��������
	EDB_TASK_RESND_ERR	= 8,	//������ط�һ����������Ȼ����Ӧ
};

#define		DB_TASK_ID_MASK			0x1fffffff

#ifndef __time32_t
typedef long __time32_t;//qizc
#endif

// DB -> T_TaskList
typedef struct DB_TASK_LIST_T
{
	INT32			nItemState;	//��ǰ���״̬
	INT32			nID;		//DB�е����к�	- 1~29�����������������,30~31Ϊ��ͬ���������,32bit��
	INT32			nPID;		//Device ID
	INT32			nSubAlarm;	//Alarm �ӱ� �����к�
	INT32			nType;		//��������
	INT32			nTableType;	//������Դ : 0-��ͳT_TaskList  1-T_NetTaskList 2-T_CheckVice 
	INT32			nStatus;	//����ִ��״̬
	DWORD32			nErrCode;	//�쳣ԭ��ֵ- Generally from GetLastError
	INT32			nSndNum;	//ָ���ط�����- �ط�ָ�������Ҫ�ҽӵ�����Ķ���
	INT32			nSsnID;		//��ǰָ���Ӧ��ִ������ĻỰID -> Session ID
	__time32_t	dwRcvTime;	//����ָ���ʱ��
	__time32_t	dwSndTime;	//ָ���·���ʱ�䣬������Դ���պͳ�ʱ�ط�
	__time32_t	dwCmpTime;	//ָ�����ʱ��
	__time32_t  dwRcntTime; //��ָ��������Ծʱ��
	char		strFile[256];	//�ϴ��ļ���
	char		strExtPara[256];	//��չ����- �����Ƕ���������ڴ�ָ��,��Ҫ�ͷ�
}DbTaskList;

//������·����ն˴����ļ�Ŀ¼�������
//��ʱ����,������չ- �ؼ��ʵĳ����ǲ�ȷ����,���������ַ�ָ�뷽ʽ
typedef struct DB_TASK_SCANDISK_T
{
	int			nID;			//�ڷ��ص�ʱ��Я��
	int			nSubID;
	char		strDir[280];	//���Ϊ��,��Ϊȫ��ɨ��;����ָ��Ŀ¼ɨ��
	char		strKeys[1024];	//��; �����ֵĹؼ����б�
}DbTaskScanDisk;

// ����ָ��ṹ����
//////////////////////////////////////////////////


//extern GR_UTLITY_API 
//{
//extern GR_UTLITY_API MsgKeyPolicy	g_tKeyPolicy[];	//��Ӧ�����ȼ��Ĺؼ��ʲ��� 0-level1 1-level2 2-level3
//extern GR_UTLITY_API MsgExtPolicy	g_tExtPolicy;

extern GR_UTLITY_API DbCfgPlcy	g_tDbCfgPlcy;

extern GR_UTLITY_API int NetDecodeAuth(char *pBuf, const int &nLen, NetMsgAuth *pPara);
extern GR_UTLITY_API int NetEncodeAuth(char *pBuf, const int &nLen, NetMsgAuth *pPara);
extern GR_UTLITY_API int NetDecodeAuthEx(char *pBuf, const int &nLen, NetMsgAuthEx *pPara);
extern GR_UTLITY_API int NetEncodeAuthEx(char *pBuf, const int &nLen, NetMsgAuthEx *pPara);
extern GR_UTLITY_API int NetDecodeAuthEx2(char *pBuf, const int &nLen, NetMsgAuthEx2 *pPara);
extern GR_UTLITY_API int NetEncodeAuthEx2(char *pBuf, const int &nLen, NetMsgAuthEx2 *pPara);
/////////////////////////////////
//add by cy
int NetEncodeAuthRsp(char *pBuf, const int &nLen, NetMsgAuthRsp *pPara);
int NetDecodeAuthRsp(char *pBuf, const int &nLen, NetMsgAuthRsp *pPara);
/////////////////////////////////////
extern GR_UTLITY_API int NetEncodePacket(const int &nMsgType, char *pBuf, const int &nLen, void *pPara);
extern GR_UTLITY_API int NetDecodePacket(char *pBuf, const int &nLen, WORD &wMsg, void *pPara);
extern GR_UTLITY_API int NetDecodeWarnLvl3Policy(char *pBuf, const int &nLen, DbWarnLevel *pPara);
extern GR_UTLITY_API int NetEncodeWarnLvl3Policy(char *pBuf, const int &nLen, DbWarnLevel *pPara);
extern GR_UTLITY_API int NetDecode3KeyPolicy(char *pBuf, const int &nLen, DbWarnLvStr *pPara);
extern GR_UTLITY_API int NetEncode3KeyPolicy(char *pBuf, const int &nLen, DbWarnLvStr *pPara);
extern GR_UTLITY_API int NetEncodeExtPolicy(char *pBuf, const int &nLen, char *pPara);
extern GR_UTLITY_API int NetDecodeExtPolicy(char *pBuf, const int &nLen, char *pPara);
extern GR_UTLITY_API int NetEncodeCfgPlcy(char *pBuf, const int &nLen, DbCfgPlcy *pPara);
extern GR_UTLITY_API int NetDecodeCfgPlcy(char *pBuf, const int &nLen, DbCfgPlcy *pPara);
extern GR_UTLITY_API int NetEncodeUsbLog(char *pBuf, const int &nLen, DbUsbLog *pPara);
extern GR_UTLITY_API int NetDecodeUsbLog(char *pBuf, const int &nLen, DbUsbLog *pPara);
extern GR_UTLITY_API int NetEncodeAlarmEvent(char *pBuf, const int &nLen, DbAlarmEvent *pPara);
extern GR_UTLITY_API int NetDecodeAlarmEvent(char *pBuf, const int &nLen, DbAlarmEvent *pPara);
extern GR_UTLITY_API int NetEncodeAlarmList(char *pBuf, const int &nLen, FmSrchRpt *pPara);
extern GR_UTLITY_API int NetDecodeAlarmList(char *pBuf, const int &nLen, FmSrchRpt *pPara);
extern GR_UTLITY_API int NetEncodeNetAlarm(char *pBuf, const int &nLen, NetmSrchRpt *pPara);
extern GR_UTLITY_API int NetDecodeNetAlarm(char *pBuf, const int &nLen, NetmSrchRpt *pPara);
extern GR_UTLITY_API int NetEncodeRegAlarm(char *pBuf, const int &nLen, RegMonRpt *pPara, const int &nItemN);
extern GR_UTLITY_API int NetDecodeRegAlarm(char *pBuf, const int &nLen, RegMonRpt *pPara);
extern GR_UTLITY_API int NetEncodeFlchgAlarm(char *pBuf, const int &nLen, FileMonSys *pPara);
extern GR_UTLITY_API int NetDecodeFlchgAlarm(char *pBuf, const int &nLen, FileMonSys *pPara);
extern GR_UTLITY_API int NetEncodeFRegPolicy(char *pBuf, const int &nLen, DbWarnLvStr *pPara);
extern GR_UTLITY_API int NetDecodeFRegPolicy(char *pBuf, const int &nLen, DbWarnLvStr *pPara);
extern GR_UTLITY_API int NetEncodeTransFile(char *pBuf, const int &nLen, MsgTransFile *pPara);
extern GR_UTLITY_API int NetDecodeTransFile(char *pBuf, const int &nLen, MsgTransFile *pPara);
extern GR_UTLITY_API int NetEncodeSendFile(char *pBuf, const int &nLen, MsgSendFile *pPara);
extern GR_UTLITY_API int NetDecodeSendFile(char *pBuf, const int &nLen, MsgSendFile *pPara);
extern GR_UTLITY_API int NetEncodeCustMsg(char *pBuf, const DWORD32 &nLen, const WORD &wMsg, 
		const char *pCustMsg, const DWORD32 & dwCustLen );
extern GR_UTLITY_API int NetDecodeCustMsg(char *pBuf, const int &nLen, void *pPara, const DWORD32 &dwLen);
extern GR_UTLITY_API int NetEncodeCommand(char *pBuf, const int &nLen, const WORD &wMsg, const DWORD32 &dwMID, const int & wAttach );
extern GR_UTLITY_API int NetEncodeFileChunk(char *pBuf, const int &nLen, MsgFileChunk *pPara);
extern GR_UTLITY_API int NetDecodeFileChunk(char *pBuf, const int &nLen, MsgFileChunk *pPara);
extern GR_UTLITY_API int NetEncodeSnapScrn(char *pBuf, const int &nLen, MsgTransFile *pPara);
extern GR_UTLITY_API int NetDecodeSnapScrn(char *pBuf, const int &nLen, MsgTransFile *pPara);
extern GR_UTLITY_API int NetEncodeSetUsrPwd(char *pBuf, const int &nLen, MsgSetUsrPwd *pPara);
extern GR_UTLITY_API int NetDecodeSetUsrPwd(char *pBuf, const int &nLen, MsgSetUsrPwd *pPara);
extern GR_UTLITY_API int NetEncodeDskSc(char *pBuf, const int &nLen, DbTaskScanDisk *pPara);
extern GR_UTLITY_API int NetDecodeDskSc(char *pBuf, const int &nLen, DbTaskScanDisk *pPara);
extern GR_UTLITY_API int NetEncryptData(char *pBuf, const int &nLen, char *pOut);
extern GR_UTLITY_API int NetDecryptData(char *pBuf, const int &nLen, char *pOut);
extern GR_UTLITY_API int NetEncodeErrNtfy(char *pBuf, const int &nLen, MsgErrNtfy *pPara);
extern GR_UTLITY_API int NetDecodeErrNtfy(char *pBuf, const int &nLen, MsgErrNtfy *pPara);
extern GR_UTLITY_API int NetEncodeAck(char *pBuf, const int &nLen, MsgAck *pPara);
extern GR_UTLITY_API int NetDecodeAck(char *pBuf, const int &nLen, MsgAck *pPara);
extern GR_UTLITY_API int UtlAllocMem(DbWarnLvStr **ptr,  WORD wAllocLen);
extern GR_UTLITY_API void UtlFreeMem(DbWarnLvStr *ptr);
//};

#if !defined(_WIN32) || _MSC_VER<=1200
//extern int strcpy_s(char * _Dst, const UINT & _SizeInBytes, const char * _Src);
extern GR_UTLITY_API int strcpy_s(char *_DEST, size_t _SIZE, const char *_SRC);
extern GR_UTLITY_API int strcat_s(char *_DEST, size_t _SIZE, const char *_SRC);
extern GR_UTLITY_API int memcpy_s(char *Dst, int nDstSize, const char *Src, int nCpyLen);
#endif
extern GR_UTLITY_API int memcpy_ex_s(char *Dst, const DWORD &nDstSize, const char *Src, const DWORD &nCpyLen);
/*
#ifndef CBUILDER_VER_6
extern int CBWriteLogin(int nPid, int nWid, char *strPid, char *strName, const int &nType, const int &nRet);
extern int CBWriteUsbLog(int &nPid, DbUsbLog &tUlog);
extern int CBWriteAlarmEvent(int &nPid, DbAlarmEvent &tAlarm);
extern int CBWriteAlarmList(const INT32 &nPid, const INT32 &nWid, const INT32 &nAid, FmSrchRpt &tAlarm);
extern int CBWriteNetAlarmList(const INT32 &nPid, const INT32 &nWid, const INT32 &nAid, NetmSrchRpt &tAlarm);
extern int CBWriteRegAlarm(const INT32 &nPid, const INT32 &nWid, const INT32 &nAid, RegMonRpt &tAlarm);
extern int CBWriteFlchgAlarm(const INT32 &nPid, const INT32 &nWid, const INT32 &nAid, FileMonSys &tAlarm);
extern int CBWriteCmdAck(const int &nID, const char *pPara);
extern int CBUpdateUsrPwd(const int &id, const char *strNewPwd);
#else
#define CBWriteLogin( nPid,  nWid, strPid, strName, nType, nRet) 
#define  CBWriteUsbLog(a, b) 
#define  CBWriteAlarmEvent(nPid, tAlarm) 
#define  CBWriteAlarmList(nPid, nWid, nAid, tAlarm) 
#define  CBWriteNetAlarmList(nPid, nWid, nAid, tAlarm) 
#define  CBWriteRegAlarm(nPid, nWid, nAid, tAlarm) 
#define  CBWriteFlchgAlarm(nPid, nWid, nAid, tAlarm) 
#define  CBWriteCmdAck(nid,pPara) 
#define  CBUpdateUsrPwd(nid,pPara) 
#endif
*/
#endif

