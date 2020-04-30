#pragma once	//qizc

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef _GREE_GUARD_COMM_DEF_H__
#define _GREE_GUARD_COMM_DEF_H__

//本文件定义所有模块公用的数据结构和宏

//////////////////////////////////////////////////////////
//定义 GetLastError 以外的自定义错误类型
#define		GR_CUST_ERR_BEGIN			30000
#define		GR_ERR_NET_OVER_TIME		(GR_CUST_ERR_BEGIN + 1)			//网络会话超期,心跳无响应
#define		GR_ERR_FORCE_ENDSND			(GR_CUST_ERR_BEGIN + 10)		//强制关闭当前发送状态
#define		GR_ERR_FORCE_ENDRCV			(GR_CUST_ERR_BEGIN + 11)		//强制关闭当前接收状态
#define		GR_ERR_READ_DISMATCH		(GR_CUST_ERR_BEGIN + 12)		//读取发送文件的时候状态不匹配
#define		GR_ERR_RECV_EXCEED			(GR_CUST_ERR_BEGIN + 13)		//
#define		GR_ERR_RECV_OFFSET			(GR_CUST_ERR_BEGIN + 14)		//
#define		GR_ERR_AL_IN_SEND			(GR_CUST_ERR_BEGIN + 15)		//接受到发送指令但已经处于发送状态
#define		GR_ERR_AL_IN_RECV			(GR_CUST_ERR_BEGIN + 16)		//接受到接收指令但已经处于接收状态
#define		GR_ERR_TID_DISMATCH			(GR_CUST_ERR_BEGIN + 17)		//接受到接收指令但已经处于接收状态

#define		GR_ERR_NO_IOCPMEM			(GR_CUST_ERR_BEGIN + 50)		//
#define		GR_ERR_OVER_PKT_SIZE		(GR_CUST_ERR_BEGIN + 51)		//
#define		GR_ERR_PARA_STATE			(GR_CUST_ERR_BEGIN + 60)		//错误的参数或者状态
#define		GR_ERR_SNAP_SCRN			(GR_CUST_ERR_BEGIN + 61)		//截屏失败
#define		GR_ERR_ZERO_LAST_ERR		(GR_CUST_ERR_BEGIN + 100)		//错误状态,但GetLastError()=0
#define		GR_ERR_KICKOFF_OLD			(GR_CUST_ERR_BEGIN + 101)		//新的会话连接中断原有未正常断开的会话
//#define		GR_ERR_NOHEARTRSP			(GR_CUST_ERR_BEGIN + 101)		//心跳无响应或者服务端规定时间内无心跳
#define		GR_ERR_UNSPECIFIED			(GR_CUST_ERR_BEGIN + 200)		//
//#define DISKM_SRCH_SGL_KEY_LEN		32
//#define DISKM_SRCH_CONTENT_LEN		80
//#define DISKM_SRCH_MAXSAMEKEY		10

#define		COMM_SRCH_KEY_LEN			32					// 关键字最大字节数（汉字16个）
#define		COMM_SRCH_CONTENT_LEN		80					// 记录的最大字节数（汉字40个）
#define		COMM_SRCH_MAXSAMEKEY		10					// 相同关键字的最大数量
#define		NF_ADDRESS_LENGTH			256					// 上报访问地址的长度
#define		COMM_SRCH_CONTEXT_SIZE		20 //COMM_SRCH_CONTENT_LEN				// 上下文访问字节（前后10个字节，即5个UNICODE）

#define		COMM_FM_MAX_FILE_EXT		8					//允许的最长监控文件后缀名长度

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
//定义USB操作记录上报结构
typedef struct _DB_USB_LOG_T
{
	//int		nPid;
	int		nWid;
	//盘符信息,描述某个盘符(C,D,E...)的属性
	BYTE 	cDrive;	//==0表示不存在 >0表示存在
	BYTE 	nType;		//磁盘类型，硬盘/U盘
	BYTE 	nActInsert;		//0 拔出 1插入
	
//	ULARGE_INTEGER unTotalSpace;	//总大小
//	ULARGE_INTEGER unFreeSpace;		//空闲空间

	//time_t 	tmEvent;		//信息获取时间
	int 	tmEvent;
	
	DWORD 	dwFSFlags;		//文件系统标志
	DWORD 	dwSerialNum;	//卷标序号

	char 	szFSName[64];		//文件系统名称
	char 	szVolume[64];		//卷标	
	char 	szFriendName[MAX_PATH];	//设备友好名

}DbUsbLog;

////////////////////////////////////
//定义事件上报类型
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
//定义事件上报结构
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

//关键词定义
typedef struct _FILEMON_SRCH_KEY_T
{
//	int		nLevel;
	BYTE	nLevel;
	BYTE	bKeyLen;
	char	strKey[COMM_SRCH_KEY_LEN];
}FmSrchKey;

//文件搜索关键字
typedef struct _FILEMON_SRCH_ITEM_T
{
	int			nLevel;
	char		strKey[COMM_SRCH_KEY_LEN];
	int			nFilePos;		//在文件中的位置
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
	DWORD		dwChangeTime;				//变更时间
	DWORD		dwFileSize;
	FILETIME	tFileCreate;
	WORD		nFileChangeType;			//文件变更类型，四种类型：新增/修改/改名/删除
	BYTE		bDiskType;					//磁盘类型
	BYTE		bReserved;
	char		szScanFile[MAX_PATH];		//变更的文件名
	char		szSourceFile[MAX_PATH];		//如果是改名操作，的旧文件名
	//key words, level, content
	//FmSrchItem*	pItem;
	//FmSrchItem* pLast;
	int			nItemN;
	//char		strDocExt[12];
	FmSrchItem	tItems[COMM_SRCH_MAXSAMEKEY];
}FmSrchRpt;

typedef struct _FILEMON_SYS
{
	DWORD orisizeL;					// 文件原始大小
	DWORD orisizeH;					// 文件原始大小
	DWORD nowsizeL;					// 文件现在的大小
	DWORD nowsizeH;					// 文件现在的大小
	FILETIME  oricreatedtime;			// 原始文件的创建时间
	FILETIME  nowcreatedtime;			// 现在文件的创建时间
	FILETIME  orimodifiedtime;			// 原始文件的修改时间
	FILETIME  nowmodifiedtime;			// 现在文件的修改时间
	WORD	nFileChangeType;			//文件变更类型，四种类型：新增/修改/改名/删除// 此项应无用
	WORD	wLevel;
	char orimd5[34];				// 原始文件的MD5校验值
	char nowmd5[34];				// 现在文件的MD5校验值
	char szKeywords[100];				//目前最多容纳100个字节
	char szFilePath[MAX_PATH];		// 文件路径
}FileMonSys, *PFILEMONSYS;

//网络搜索结构

typedef struct _COMM_SRCH_ITEM_T
{
	UINT16		uLevel;										// 关键字的权重级别
	char		szKey[COMM_SRCH_KEY_LEN];					// 关键字文字串（50个UNICODE）
	char		szContent[COMM_SRCH_CONTENT_LEN];			// 含关键字文字的上下文 （同上）
	UINT32		nPos;										// 在数据流中的位置
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
	SYSTEMTIME	stTime;										// 关键数据发生的时间
	UINT16		uCmdType;									// 发生关键字的行为类型
	UINT16		uCmdFlag;									// 发生关键字的数据流向
	UINT16		uDstIpPort;									// 发生关键字行为的网络端口
	UINT16		uSrcIpPort;									// 发生关键字行为的网络端口
	ULONG32		ulDstIpAddress;								// 发生关键字的目的IP地址 
	ULONG32		ulSrcIpAddress;								// 发生关键字的本机IP地址
	char		szDstAddress[NF_ADDRESS_LENGTH];					// URL最大可能达到2000字节
	char		szSrcAddress[NF_ADDRESS_LENGTH];				// msn聊天中，可能的己方email或微博的帐号
	UINT16		nItemSum;									// 查找到的关键字的数量
	CommSrchItem	tItems[COMM_SRCH_MAXSAMEKEY];			// 
}NetmSrchRpt, *PNETM_SRCH_RPT;


///////////////////////////////////////////////////////////
// 驱动相关的宏定义START
///////////////////////////////////////////////////////////

//----------------------------------------------------
// REGMON.SYS
/************************************************************************
注册表监控匹配宏定义和结构
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
	SYSTEMTIME	stTime;										// 注册表修改发生的时间
	USHORT		uLevel;										// 告警登记
	USHORT		uCmdType;									// 发生的行为
	USHORT		uCmdRet;									// 行为结果
	char		szExePath[REGM_MAXPROCNAMELEN];					// 应用程序
	char		szRegPath[MAX_PATH];						// 注册表键 - MAXPATHLEN
	char		szRegValue[REGM_MAXDATALEN];						// 键值
}RegMonRpt, *PREGMON_RPT;

// Variables/definitions for the driver that performs the actual monitoring.
#define	SYS_FILE			"GrRegMon.SYS"
#define	GR_DRV_REGSYS_NAME			"GrRegMon"

// Number of recent filters we keep
#define NUMRECENTFILTERS	5


// 驱动相关的宏定义END!!!

#endif

