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

#define		GR_SVR_CMD_RESET_TIME	(60*60)         //服务端下发的指令如果在规定时间内没有获得响应重置-下次重发


class MainTask
	:public sql_manager{
public:
	DbTaskList		tDbTask_;
	UtlList<DbTaskList>		listCmd_;
	int s_nRefreshTimerN;
public:
	MainTask();
	/* 连接数据库 */
	bool ConnectDB();
	/* 从数据库获取指令 */
	void GetCmdTask(time_t tm,const char* sql);
	/* 从数据库获取检查任务 */
	void GetDiskCheckTask(const time_t tm,const char* basesql);
	/* 任务处理函数 */
	void HandleTask(const time_t tm);
	/* 更新用户信息 */
	bool CBGetDbUserInfo();
	/* 更新设备信息 */
	bool CBGetDbDeviceInfo();
	/* 扫描异常的任务 如超时任务*/
	void ScanCmdListTime(const time_t &dwCurTime);
	/* 主循环 */
	void MainLoop();
	int CBWriteCmdAck(const int &nID, const int &nType, const char *pPara);

	bool InitDbUserInfo();
	bool InitDbDeviceInfo();
};

extern MainTask g_mt;

#endif
