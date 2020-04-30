#pragma once	//qizc

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef _GREE_GUARD_COMM_DEF_H__
#define _GREE_GUARD_COMM_DEF_H__

//���ļ���������ģ�鹫�õ����ݽṹ�ͺ�

//////////////////////////////////////////////////////////
//���� GetLastError ������Զ����������
#define		GR_CUST_ERR_BEGIN			30000
#define		GR_ERR_NET_OVER_TIME		(GR_CUST_ERR_BEGIN + 1)			//����Ự����,��������Ӧ
#define		GR_ERR_FORCE_ENDSND			(GR_CUST_ERR_BEGIN + 10)		//ǿ�ƹرյ�ǰ����״̬
#define		GR_ERR_FORCE_ENDRCV			(GR_CUST_ERR_BEGIN + 11)		//ǿ�ƹرյ�ǰ����״̬
#define		GR_ERR_READ_DISMATCH		(GR_CUST_ERR_BEGIN + 12)		//��ȡ�����ļ���ʱ��״̬��ƥ��
#define		GR_ERR_RECV_EXCEED			(GR_CUST_ERR_BEGIN + 13)		//
#define		GR_ERR_RECV_OFFSET			(GR_CUST_ERR_BEGIN + 14)		//
#define		GR_ERR_AL_IN_SEND			(GR_CUST_ERR_BEGIN + 15)		//���ܵ�����ָ��Ѿ����ڷ���״̬
#define		GR_ERR_AL_IN_RECV			(GR_CUST_ERR_BEGIN + 16)		//���ܵ�����ָ��Ѿ����ڽ���״̬
#define		GR_ERR_TID_DISMATCH			(GR_CUST_ERR_BEGIN + 17)		//���ܵ�����ָ��Ѿ����ڽ���״̬

#define		GR_ERR_NO_IOCPMEM			(GR_CUST_ERR_BEGIN + 50)		//
#define		GR_ERR_OVER_PKT_SIZE		(GR_CUST_ERR_BEGIN + 51)		//
#define		GR_ERR_PARA_STATE			(GR_CUST_ERR_BEGIN + 60)		//����Ĳ�������״̬
#define		GR_ERR_SNAP_SCRN			(GR_CUST_ERR_BEGIN + 61)		//����ʧ��
#define		GR_ERR_ZERO_LAST_ERR		(GR_CUST_ERR_BEGIN + 100)		//����״̬,��GetLastError()=0
#define		GR_ERR_KICKOFF_OLD			(GR_CUST_ERR_BEGIN + 101)		//�µĻỰ�����ж�ԭ��δ�����Ͽ��ĻỰ
//#define		GR_ERR_NOHEARTRSP			(GR_CUST_ERR_BEGIN + 101)		//��������Ӧ���߷���˹涨ʱ����������
#define		GR_ERR_UNSPECIFIED			(GR_CUST_ERR_BEGIN + 200)		//
//#define DISKM_SRCH_SGL_KEY_LEN		32
//#define DISKM_SRCH_CONTENT_LEN		80
//#define DISKM_SRCH_MAXSAMEKEY		10

#define		COMM_SRCH_KEY_LEN			32					// �ؼ�������ֽ���������16����
#define		COMM_SRCH_CONTENT_LEN		80					// ��¼������ֽ���������40����
#define		COMM_SRCH_MAXSAMEKEY		10					// ��ͬ�ؼ��ֵ��������
#define		NF_ADDRESS_LENGTH			256					// �ϱ����ʵ�ַ�ĳ���
#define		COMM_SRCH_CONTEXT_SIZE		20 //COMM_SRCH_CONTENT_LEN				// �����ķ����ֽڣ�ǰ��10���ֽڣ���5��UNICODE��

#define		COMM_FM_MAX_FILE_EXT		8					//����������ļ���׺������

//qizc
#ifndef UINT16
typedef unsigned short UINT16;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef ULONG32
typedef unsigned int ULONG32;
//#define ULONG32 unsigned long
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif

#ifndef INT32
typedef int INT32;
#endif

#ifndef UINT32
typedef unsigned int UINT32;
//#define UINT32 unsigned long
#endif


#ifndef MAX_PATH
#define MAX_PATH (260*2)
#endif





////////////////////////////////////
//����USB������¼�ϱ��ṹ
typedef struct _DB_USB_LOG_T
{
	//int		nPid;
	int		nWid;
	//�̷���Ϣ,����ĳ���̷�(C,D,E...)������
	BYTE 	cDrive;	//==0��ʾ������ >0��ʾ����
	BYTE 	nType;		//�������ͣ�Ӳ��/U��
	BYTE 	nActInsert;		//0 �γ� 1����
	
//	ULARGE_INTEGER unTotalSpace;	//�ܴ�С
//	ULARGE_INTEGER unFreeSpace;		//���пռ�

	//time_t 	tmEvent;		//��Ϣ��ȡʱ��
	int 	tmEvent;
	
	DWORD 	dwFSFlags;		//�ļ�ϵͳ��־
	DWORD 	dwSerialNum;	//������

	char 	szFSName[64];		//�ļ�ϵͳ����
	char 	szVolume[64];		//���	
	char 	szFriendName[MAX_PATH];	//�豸�Ѻ���

}DbUsbLog;

////////////////////////////////////
//�����¼��ϱ�����
typedef enum _DB_ALARM_TYPE_E_
{
	DB_ALARM_CREATE = 1,
	DB_ALARM_DELETE,
	DB_ALARM_MODIFY,
	DB_ALARM_RENAME,
//	DB_ALARM_COPY,
//	DB_ALARM_DOWN,
//	DB_ALARM_UPLOAD,
}DbAlarmTypeE;
////////////////////////////////////
//�����¼��ϱ��ṹ
//ID
//SubID
//AlarmTime
//PID
//WID
//AlarmType
//AlarmLevel
//AlarmFile
//KeyWord
//Content
//Description
//DiskType
//DocType
typedef struct _DB_ALARM_EVENT_T_
{
	int			nWid;
	//time_t		nTime;
	int			nTime;
	BYTE		nAlarmType;
	BYTE		nAlarmLevel;
	BYTE		bDiskType;
	char		strDocExt[16];
	char		strKey[COMM_SRCH_KEY_LEN];
	char		strFile[MAX_PATH];
	char		strContent[90];
	char		strDes[250];
}DbAlarmEvent;

//�ؼ��ʶ���
typedef struct _FILEMON_SRCH_KEY_T
{
//	int		nLevel;
	BYTE	nLevel;
	BYTE	bKeyLen;
	char	strKey[COMM_SRCH_KEY_LEN];
}FmSrchKey;

//�ļ������ؼ���
typedef struct _FILEMON_SRCH_ITEM_T
{
	int			nLevel;
	char		strKey[COMM_SRCH_KEY_LEN];
	int			nFilePos;		//���ļ��е�λ��
	char		strContent[COMM_SRCH_CONTENT_LEN];
	//FmSrchItem*	pNext;	
}FmSrchItem;

#ifndef _WIN32
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#endif

typedef struct _FILEMON_SRCH_RPT_T
{
	DWORD		dwChangeTime;				//���ʱ��
	DWORD		dwFileSize;
	FILETIME	tFileCreate;
	WORD		nFileChangeType;			//�ļ�������ͣ��������ͣ�����/�޸�/����/ɾ��
	BYTE		bDiskType;					//��������
	BYTE		bReserved;
	char		szScanFile[MAX_PATH];		//������ļ���
	char		szSourceFile[MAX_PATH];		//����Ǹ����������ľ��ļ���
	//key words, level, content
	//FmSrchItem*	pItem;
	//FmSrchItem* pLast;
	int			nItemN;
	//char		strDocExt[12];
	FmSrchItem	tItems[COMM_SRCH_MAXSAMEKEY];
}FmSrchRpt;

typedef struct _FILEMON_SYS
{
	DWORD orisizeL;					// �ļ�ԭʼ��С
	DWORD orisizeH;					// �ļ�ԭʼ��С
	DWORD nowsizeL;					// �ļ����ڵĴ�С
	DWORD nowsizeH;					// �ļ����ڵĴ�С
	FILETIME  oricreatedtime;			// ԭʼ�ļ��Ĵ���ʱ��
	FILETIME  nowcreatedtime;			// �����ļ��Ĵ���ʱ��
	FILETIME  orimodifiedtime;			// ԭʼ�ļ����޸�ʱ��
	FILETIME  nowmodifiedtime;			// �����ļ����޸�ʱ��
	WORD	nFileChangeType;			//�ļ�������ͣ��������ͣ�����/�޸�/����/ɾ��// ����Ӧ����
	WORD	wLevel;
	char orimd5[34];				// ԭʼ�ļ���MD5У��ֵ
	char nowmd5[34];				// �����ļ���MD5У��ֵ
	char szKeywords[100];				//Ŀǰ�������100���ֽ�
	char szFilePath[MAX_PATH];		// �ļ�·��
}FileMonSys, *PFILEMONSYS;

//���������ṹ

typedef struct _COMM_SRCH_ITEM_T
{
	UINT16		uLevel;										// �ؼ��ֵ�Ȩ�ؼ���
	char		szKey[COMM_SRCH_KEY_LEN];					// �ؼ������ִ���50��UNICODE��
	char		szContent[COMM_SRCH_CONTENT_LEN];			// ���ؼ������ֵ������� ��ͬ�ϣ�
	UINT32		nPos;										// ���������е�λ��
}CommSrchItem, *PCommSrchItem;

#ifndef _WIN32
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
#endif

typedef struct _NETMON_SRCH_RPT_T
{
	SYSTEMTIME	stTime;										// �ؼ����ݷ�����ʱ��
	UINT16		uCmdType;									// �����ؼ��ֵ���Ϊ����
	UINT16		uCmdFlag;									// �����ؼ��ֵ���������
	UINT16		uDstIpPort;									// �����ؼ�����Ϊ������˿�
	UINT16		uSrcIpPort;									// �����ؼ�����Ϊ������˿�
	ULONG32		ulDstIpAddress;								// �����ؼ��ֵ�Ŀ��IP��ַ 
	ULONG32		ulSrcIpAddress;								// �����ؼ��ֵı���IP��ַ
	char		szDstAddress[NF_ADDRESS_LENGTH];					// URL�����ܴﵽ2000�ֽ�
	char		szSrcAddress[NF_ADDRESS_LENGTH];				// msn�����У����ܵļ���email��΢�����ʺ�
	UINT16		nItemSum;									// ���ҵ��Ĺؼ��ֵ�����
	CommSrchItem	tItems[COMM_SRCH_MAXSAMEKEY];			// 
}NetmSrchRpt, *PNETM_SRCH_RPT;


///////////////////////////////////////////////////////////
// ������صĺ궨��START
///////////////////////////////////////////////////////////

//----------------------------------------------------
// REGMON.SYS
/************************************************************************
ע�����ƥ��궨��ͽṹ
************************************************************************/
//
// Max len of any error string
//
#define REGM_MAXERRORLEN    32

//
// Maximum length of data that will be copied to the "other" field in the display
//
#define REGM_MAXDATALEN     64

//
// Length of process name buffer. Process names are at most 16 characters so
// we take into account a trailing NULL.
//
#define REGM_MAXPROCNAMELEN  32

//
// Maximum length of NT process name
//
#define REGM_NT_PROCNAMELEN  16


#define RM_CMDNUM			8
/*
#define RM_KEY_CREATE		0
#define RM_KEY_OPEN			1
#define RM_KEY_ENUMERATE	2
#define RM_KEY_QUERY		3
#define RM_KEY_CLOSE		4
#define RM_VALUE_QUERY		5
#define RM_VALUE_ENUMERATE	6
#define RM_VALUE_SET		7
*/

#define RM_REQUEST_SUCCESS		1
#define RM_REQUEST_ERROR		0

typedef struct _REGMON_RPT_T
{
	SYSTEMTIME	stTime;										// ע����޸ķ�����ʱ��
	USHORT		uLevel;										// �澯�Ǽ�
	USHORT		uCmdType;									// ��������Ϊ
	USHORT		uCmdRet;									// ��Ϊ���
	char		szExePath[REGM_MAXPROCNAMELEN];					// Ӧ�ó���
	char		szRegPath[MAX_PATH];						// ע���� - MAXPATHLEN
	char		szRegValue[REGM_MAXDATALEN];						// ��ֵ
}RegMonRpt, *PREGMON_RPT;

// Variables/definitions for the driver that performs the actual monitoring.
#define	SYS_FILE			"GrRegMon.SYS"
#define	GR_DRV_REGSYS_NAME			"GrRegMon"

// Number of recent filters we keep
#define NUMRECENTFILTERS	5


// ������صĺ궨��END!!!

#endif

