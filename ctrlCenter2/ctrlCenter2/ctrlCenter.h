#ifndef _CTRLCENTER_H_
#define _CTRLCENTER_H_
#include "sql_manager.h"
#include "UtlList.h"
#include "NetUtlity.h"
#include "NetDef.h"
#include "NetSession.h"

void send_to_all_user(byte * data1, size_t size1, byte * data2, size_t size2, byte * data3, size_t size3);
void send_to_user(unsigned long long tcp_net_id, axis::shared_buffer& sb);
void send_to_user(unsigned long long tcp_net_id,byte* data,size_t size);

#define		GR_SVR_CMD_RESET_TIME	(60*60)         //������·���ָ������ڹ涨ʱ����û�л����Ӧ����-�´��ط�


class MainTask
	:public sql_manager{
public:
	DbTaskList		tDbTask_;
	UtlList<DbTaskList>		listCmd_;
	int s_nRefreshTimerN;
public:
	MainTask();
	/* �������ݿ� */
	bool ConnectDB();
	/* �����ݿ��ȡָ�� */
	void GetCmdTask(time_t tm,const char* sql);
	/* �����ݿ��ȡ������� */
	void GetDiskCheckTask(const time_t tm,const char* basesql);
	/* �������� */
	void HandleTask(const time_t tm);
	/* �����û���Ϣ */
	bool CBGetDbUserInfo();
	/* �����豸��Ϣ */
	bool CBGetDbDeviceInfo();
	/* ɨ���쳣������ �糬ʱ����*/
	void ScanCmdListTime(const time_t &dwCurTime);
	/* ��ѭ�� */
	void MainLoop();
	int CBWriteCmdAck(const int &nID, const int &nType, const char *pPara);

	bool InitDbUserInfo();
	bool InitDbDeviceInfo();
};

extern MainTask g_mt;

#endif
