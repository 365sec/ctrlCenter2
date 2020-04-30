#ifndef _GREE_GUARD_FILE_TRANS_H__
#define _GREE_GUARD_FILE_TRANS_H__

/////////////////////////////////////////////////////////////////////
//	文件传输
/////////////////////////////////////////////////////////////////////
//文件传输状态
enum FT_TRANS_STATE_E
{
	FT_STATE_UNKNOWN 	= 0,
	FT_STATE_SENDREQ	= 1,	//发送文件请求，一般携带发送文件的具体信息
	FT_STATE_RECVAUTH	= 2,	//收到SENDREQ，确认能否接受文件
	FT_STATE_RECVREQ	= 3,	//接收文件请求，一般携带需要接收的文件名
	FT_STATE_SENDAUTH	= 4,	//收到RECVREQ，auth if can send (create file ... )
	FT_STATE_WAITDATA	= 5,	//接收文件方确认请求，等待对方数据
	FT_STATE_TRANS		= 6,		//数据传输过程中
	FT_STATE_TRANS_SEND,		//数据传输过程中
	FT_STATE_TRANS_RECV,		//数据传输过程中
	FT_STATE_SEND_BUF ,			//buffer传输过程中
	FT_STATE_CRT_ERR,
	FT_STATE_SENDEND,	//in send end state, but not receive end ack
};

//文件传输命令
enum FT_TRANS_E
{
	FT_TRANS_DOWNREQ,
	FT_TRANS_UPREQ,
	FT_TRANS_CHUNK,		//包含文件块-FilePos Len Content
	FT_TRANS_DFCREQ,	//下载指定文件的某块- FileName FilePos Len Content
	FT_TRANS_DCHUNKREQ,	//下载当前文件的某块- FilePos Len Content - 适用于重传
	FT_TRANS_END,
	FT_TRANS_ABORT,
	FT_TRANS_APPEND,	//类同于FTP 中的APPEND
	FT_TRANS_RESTART,	//类同于FTP 中的REST，断点续传用
	FT_TRANS_MODE,		//传输模式，比如压缩、加密等
};

//文件控制名称-> Create/Delete/Rename/Copy(Move) - File(Dir)
enum FT_COMMAND_E
{
	FT_CTRL_UNKNOWN,
	FT_CTRL_VER,
	FT_CTRL_CHECKSUM,
	FT_CTRL_CREATE_FILE,
	FT_CTRL_CREATE_DIR,
	FT_CTRL_DEL_FILE,
	FT_CTRL_DEL_DIR,
	FT_CTRL_RENAME_FILE,
	FT_CTRL_RENAME_DIR,
	FT_CTRL_MOVE_FILE,
	FT_CTRL_MOVE_DIR,
	FT_CTRL_COPY_FILE,
	FT_CTRL_COPY_DIR,
	FT_CTRL_RDRIVELIST,
	FT_CTRL_ADRIVELIST,
	FT_CTRL_RDIRLIST,
	FT_CTRL_ADIRLIST,
	FT_CTRL_RFILEINFO,
	FT_CTRL_AFILEINFO,
	FT_CTRL_ABORT,
	FT_CTRL_COMMAND,
};
//文件控制反馈
//命令的响应有

//文件传输结构
//typedef struct _NET_TRANS_FILE_T
/*
class NetSession;

class NetTransFile
{
	int				m_nID;
	int				m_nSubID;
	DWORD			m_dwCurPos;
	DWORD			m_dwFileSize;
	DWORD			m_dwFileSeq;
	HANDLE			m_hTransFile;
	char			szExt[16];
	INT32			m_nTransMode;
	NetSession*		m_pSession;
	//INT32			m_nDirection;  //0 - recv  1-send
	//超时回收的功能可以放到命令状态模块统一管理
	//DWORD			m_dwRcntTime;	//最近一个控制命令发送的时间，用于超时回收
	//char			m_strCurPath[MAX_PATH];
	//char 			szSrcFile[MAX_PATH];	//上传的文件名
	//char			szDstFile[MAX_PATH];	//本地保存的文件名,目录由全局设置
	//INT				bClient;
	//int				nBufLen;
	//char			szBuffer[5120];
	//INT32			m_nTransState;
public:	
	NetTransFile(NetSession *p);
	~NetTransFile();
	BOOL 	SendTransFile(MsgTransFile &tTransFile);
	BOOL 	CreateTransFile(MsgTransFile &tTransFile);
	int  	ReadTransFile(NetDatAndContext *pContext);
	BOOL 	EndTransFile();
	inline	void 	SetTransState(const INT32 &nState){m_nTransMode = nState;}
	inline  INT32 	GetTransState(){return m_nTransMode;}

	INT32	ProcTransMsg(const INT32 &nMsg, const char *pBuf, const INT32 &nLen);
	GetDriverList();
	GetDirContent();
	GetFileInfo();
	GetDirInfo();//size-file num folder num
	CreateDir();
	MoveDir();
	DelDir();
	CopyDir();
	CreateFile();
	MoveFile();
	CopyFile();
	DelFile();
	
	
};
*/
//NetTransFile;

#endif

