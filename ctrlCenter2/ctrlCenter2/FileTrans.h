#ifndef _GREE_GUARD_FILE_TRANS_H__
#define _GREE_GUARD_FILE_TRANS_H__

/////////////////////////////////////////////////////////////////////
//	�ļ�����
/////////////////////////////////////////////////////////////////////
//�ļ�����״̬
enum FT_TRANS_STATE_E
{
	FT_STATE_UNKNOWN 	= 0,
	FT_STATE_SENDREQ	= 1,	//�����ļ�����һ��Я�������ļ��ľ�����Ϣ
	FT_STATE_RECVAUTH	= 2,	//�յ�SENDREQ��ȷ���ܷ�����ļ�
	FT_STATE_RECVREQ	= 3,	//�����ļ�����һ��Я����Ҫ���յ��ļ���
	FT_STATE_SENDAUTH	= 4,	//�յ�RECVREQ��auth if can send (create file ... )
	FT_STATE_WAITDATA	= 5,	//�����ļ���ȷ�����󣬵ȴ��Է�����
	FT_STATE_TRANS		= 6,		//���ݴ��������
	FT_STATE_TRANS_SEND,		//���ݴ��������
	FT_STATE_TRANS_RECV,		//���ݴ��������
	FT_STATE_SEND_BUF ,			//buffer���������
	FT_STATE_CRT_ERR,
	FT_STATE_SENDEND,	//in send end state, but not receive end ack
};

//�ļ���������
enum FT_TRANS_E
{
	FT_TRANS_DOWNREQ,
	FT_TRANS_UPREQ,
	FT_TRANS_CHUNK,		//�����ļ���-FilePos Len Content
	FT_TRANS_DFCREQ,	//����ָ���ļ���ĳ��- FileName FilePos Len Content
	FT_TRANS_DCHUNKREQ,	//���ص�ǰ�ļ���ĳ��- FilePos Len Content - �������ش�
	FT_TRANS_END,
	FT_TRANS_ABORT,
	FT_TRANS_APPEND,	//��ͬ��FTP �е�APPEND
	FT_TRANS_RESTART,	//��ͬ��FTP �е�REST���ϵ�������
	FT_TRANS_MODE,		//����ģʽ������ѹ�������ܵ�
};

//�ļ���������-> Create/Delete/Rename/Copy(Move) - File(Dir)
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
//�ļ����Ʒ���
//�������Ӧ��

//�ļ�����ṹ
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
	//��ʱ���յĹ��ܿ��Էŵ�����״̬ģ��ͳһ����
	//DWORD			m_dwRcntTime;	//���һ����������͵�ʱ�䣬���ڳ�ʱ����
	//char			m_strCurPath[MAX_PATH];
	//char 			szSrcFile[MAX_PATH];	//�ϴ����ļ���
	//char			szDstFile[MAX_PATH];	//���ر�����ļ���,Ŀ¼��ȫ������
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

