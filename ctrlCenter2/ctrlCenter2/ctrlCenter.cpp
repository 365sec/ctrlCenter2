// ctrlCenter.cpp : 定义控制台应用程序的入口点。
//
#ifdef WIN32
#include "stdafx.h"
#else
#define _tmain main
#endif
#include "myNetService.h"
#include "boost/thread.hpp"
#include "boost/exception/all.hpp"
#include "configure.h"
#include "sql_manager.h"
#include "log_utility.h"
#include "global.h"
#include "CBInterDB.h"
#include "NetWorld.h"
#include "FileTrans.h"
#include "stdlib.h"
#include"utils.h"
#include"dir.h"
#include"key.h"
#ifndef WIN32
#define CHECK_KEY
#endif 
myNetServ *g_m;
MainTask g_mt;

extern int   CheckAndReadKey(KeyContent&  keyCon);

int  get_session_id(unsigned long long tcp_net_id)
{
	return g_m->get_session_id(tcp_net_id);
}

void set_session_id(unsigned long long tcp_net_id, int sessionid )
{
	g_m->set_session_id(tcp_net_id,sessionid);
}


void send_to_all_user(byte * data1, size_t size1, byte * data2, size_t size2, byte * data3, size_t size3)
{
	g_m->send_to_all_user(data1, size1, data2, size2, data3, size3);
};

void send_to_user(unsigned long long tcp_net_id, axis::shared_buffer& sb)
{
	g_m->send_to_user(tcp_net_id, sb);
};

void send_to_user(unsigned long long tcp_net_id,byte* data,size_t size){
	g_m->send_to_user(tcp_net_id,data,size);
};

//void close_tcp_id(unsigned long long tcp_net_id)
void close_tcp_id(unsigned long long tcp_net_id)
{
	 g_m->close_tcp_id(tcp_net_id);
}

MainTask::MainTask()
{
	s_nRefreshTimerN = 1;
	if (listCmd_.Init(5000) == false)
	{
		log_error("g_listCmd InitResource Error!");
	}
	if (g_tWorld.InitResource() == false)
	{
		log_error("g_tWorld InitResource Error!");
	}
}

bool MainTask::ConnectDB()
{
	if(!connect_db()){
		log_error("Connect DB error.");
		return false;
	}
	SACommand cmd(getSAConnection(),"SET NAMES UTF8");
	cmd.Execute();
	return true;
}

bool MainTask::InitDbUserInfo()
{
	g_tCurLocTime = time(NULL);
	int i=0;
	//m_mapUser.clear();
	NetWorld::ClearMapUser();
	if (g_tWorld.m_pUserArray)
		delete []g_tWorld.m_pUserArray;
	g_tWorld.m_pUserArray = new NetUserInfo[NET_MAX_SSN_USERS];
	if (g_tWorld.m_pUserArray == NULL)
		return FALSE;
	memset(g_tWorld.m_pUserArray, 0, sizeof(NetUserInfo)*NET_MAX_SSN_USERS);
	g_tWorld.m_nUserInfoN = NET_MAX_SSN_USERS;
	std::string str;
	int nAllowLogin = 0;
	//这里应该从数据库获取用户信息，此处临时赋值
	str = "select ID,userCode,userName,userPass,Status from T_User";
	SACommand cmd(getSAConnection(), str.c_str());
	cmd.Execute();

	while (cmd.FetchNext())
	{
		g_tWorld.m_pUserArray[i].m_nWorkID = cmd[1].asLong();
		str = cmd[2].asString().GetMultiByteChars();
		strcpy(g_tWorld.m_pUserArray[i].m_strWID, str.c_str());
		str = cmd[3].asString().GetMultiByteChars();
		strcpy(g_tWorld.m_pUserArray[i].m_strName, str.c_str());
		str = cmd[4].asString().GetMultiByteChars();
		strcpy(g_tWorld.m_pUserArray[i].m_strPwd, str.c_str());
		nAllowLogin = cmd[5].asLong();
		if (nAllowLogin)
			g_tWorld.m_pUserArray[i].m_dwFlag = 1;
        ++i;
	}
	if(i <=0 ) return 0;

	g_tWorld.m_nValidUser = i;
	
	//init map, fast index
	std::string strwid;
	for (i=0; (i<g_tWorld.m_nValidUser && i<g_tWorld.m_nUserInfoN); i++)
	{
		strwid = (std::string)g_tWorld.m_pUserArray[i].m_strWID;
		//m_mapUser.insert(std::pair<std::string, NetUserInfo*>(strwid, &m_pUserArray[i]));
		NetWorld::InsertMapUser( strwid, &g_tWorld.m_pUserArray[i]);
	}
	return TRUE;
}

bool MainTask::InitDbDeviceInfo()
{
	DbDeviceInfo*	pDbDevArr = g_tWorld.m_pDbDevArr;

	if (pDbDevArr)
		delete []pDbDevArr;
	pDbDevArr = new DbDeviceInfo[NET_MAX_SVR_DEVICE];
	if (pDbDevArr == NULL)
		return false;
	memset(pDbDevArr, 0, sizeof(DbDeviceInfo)*NET_MAX_SVR_DEVICE);
	g_tWorld.m_pDbDevArr = pDbDevArr;


	int i=0;
	std::string str;
	NetSession* pSession = NULL;
	//这里应该从数据库获取用户信息，此处临时赋值
    str = "select id, gip, client_guid, hardware,name,hostdepart,hostresponsor from t_client  WHERE isdel =0 OR isdel IS NULL ";
	
	SACommand cmd(getSAConnection(), str.c_str());
	cmd.Execute();

	while (cmd.FetchNext())
	{
		//小心越界
		pSession = g_tWorld.GetSessionPtr(i);
		if(pSession == NULL)
			break;
		pSession->nID = cmd[1].asLong();  /*id*/

		str = cmd[3].asString().GetMultiByteChars();
		strcpy_s(pSession->m_strPID, NET_MAX_USER_PID, str.c_str());  /*guid*/


		//init the device array
		pDbDevArr[i].nDbID =  pSession->nID;/*id*/
		str = cmd[4].asString().GetMultiByteChars();
		strcpy_s(pDbDevArr[i].strHard, NET_MAX_USER_PID, str.c_str()); /*hardware*/
		strcpy_s(pDbDevArr[i].strDevName, NET_MAX_USER_PID, str.c_str());/*hardware*/
		str = cmd[5].asString().GetMultiByteChars();
		strcpy_s(pDbDevArr[i].strDevCode, 100, str.c_str()); /*name*/
		str = cmd[6].asString().GetMultiByteChars();
		strcpy_s(pDbDevArr[i].strHostDepart, 200, str.c_str()); /*hostdepart*/
		strcpy_s(pSession->m_strHostDepart, GR_HOST_DEPART_N, str.c_str());  /*DEPART*/
		str = cmd[7].asString().GetMultiByteChars();
		strcpy_s(pDbDevArr[i].strHostResponsor, 200, str.c_str()); /*hostresponsor*/
		strcpy_s(pSession->m_strHostResponsor, GR_HOST_RESPONSOR_N, str.c_str());  /*HostResponsor*/
		++i;
	}

	if(i <=0)
		return false;

	if (i >= NET_MAX_SVR_DEVICE)
	{
		log_warning("InitDbDeviceInfo Db Device Num:%d MaxAllowed:%d ", i, NET_MAX_SVR_DEVICE);
	}
	for (int j=0;j<i;j++)
	{
		NetWorld::m_mapDbDev.insert(std::pair<INT32, DbDeviceInfo*>(pDbDevArr[j].nDbID, &pDbDevArr[j]));
	}
	return TRUE;
}

void MainTask::GetCmdTask(time_t tm,const char* sql)
{
	static int		m_nMaxTaskID = 0;
	try{
		std::string strsql;

 #ifdef CHECK_KEY
		strsql = sql;
#else
		strsql = "select id, client_guid, command_type, file from t_command where state=1 and ID>";
#endif
		strsql += boost::lexical_cast<std::string>(m_nMaxTaskID); 
		SACommand cmd(getSAConnection(), strsql.c_str());
		cmd.Execute();

		int count=0;
		long id=0;

		log_debug("[GetCmdTask] sql= %s", strsql.c_str());

		while (cmd.FetchNext())
		{
			count++;

			memset(&tDbTask_, 0, sizeof(tDbTask_));
			tDbTask_.nID		= cmd[1].asLong();
			tDbTask_.nPID		= cmd[2].asLong();
			/*根据session的pid找到数组的下标index*/
			tDbTask_.nSsnID	= g_tWorld.GetSidByPid(tDbTask_.nPID);
			tDbTask_.nType		= cmd[3].asLong();

			//合法性校验,如果参数不符合跳过
			if (tDbTask_.nSsnID < 0 || tDbTask_.nType <= 0
				|| tDbTask_.nType >= sizeof(g_sDbNetMsgPair)/sizeof(g_sDbNetMsgPair[0]))
			{
				log_debug("T_TaskList Invalid Para ID:%d exeComp:%d Type:%d", 
					tDbTask_.nID, tDbTask_.nPID, tDbTask_.nType);
			}
			else
			{
				//	s_tDbTask.nSubAlarm	= db->QueryTask->FieldByName("monCode")->AsInteger;
				tDbTask_.dwRcvTime = tm;

				// 1~29个比特是真正的序号,30~31为不同命令的区分,32bit空
				//s_tDbTask.nID |= (s_tDbTask.nTableType << 29);

				std::string strfile = cmd[4].asString().GetMultiByteChars();
				if (!strfile.empty())
					strcpy_s(tDbTask_.strFile, sizeof(tDbTask_.strFile), strfile.c_str());

				//add strExtPara here cy

				log_debug("@@@ GetTask: ID:%x Type:%d  SubID:%d File:%s Ssnid:%d", 
					tDbTask_.nID, tDbTask_.nType , tDbTask_.nSubAlarm, tDbTask_.strFile,tDbTask_.nSsnID);

				if (listCmd_.Push(tDbTask_.nID, tDbTask_) == false)
				{
					log_debug("g_listCmd.Push FALSE!");
					break;
				}
			}

			if ((tDbTask_.nID & DB_TASK_ID_MASK) > m_nMaxTaskID)
			{
				m_nMaxTaskID = (tDbTask_.nID & DB_TASK_ID_MASK);
			}
		} //while
		if(count <= 0){
			log_debug("GetCmdTask has no task.");
		}

	}catch (SAException &x)
	{
		log_error("GetCmdTask Exp Msg:%s", x.ErrText().GetMultiByteChars());
		handle_database_exception(x);
	}
	catch(...)
	{
		log_error("GetCmdTask Exception!");
	}
}

void MainTask::GetDiskCheckTask(const time_t tm,const char* basesql){
	std::string  strFile;
	int nChkType = 0;
	int i = 0;
	static int		m_nMaxDiskTaskID = 0;
	//----------------------------------------------
	//获取指令列表
	try{
		//获取专项检查指令列表
		std::string strsql;
#ifdef CHECK_KEY
		strsql = basesql;
#else
	    strsql = "select id,client_guid,keywords,config from t_aqjc_result where state=1 and ID>";
#endif

		strsql += boost::lexical_cast<std::string>(m_nMaxDiskTaskID); 

		log_debug("GetDiskCheckTask %s", strsql.c_str());

		SACommand cmd(getSAConnection(), strsql.c_str());
		cmd.Execute();

		//即使没有新的指令也应该执行后续语句否则数据库状态永远无法更新

		while (cmd.FetchNext())
		{
			i++;
			memset(&tDbTask_, 0, sizeof(tDbTask_));
			//====================================
			tDbTask_.nTableType= 2; //T_CheckVice
			//====================================
			tDbTask_.nID		= cmd[1].asLong();
			tDbTask_.nPID		= cmd[2].asLong();
			tDbTask_.nSsnID	= g_tWorld.GetSidByPid(tDbTask_.nPID);
			if (tDbTask_.nSsnID < 0)
			{
				log_debug("T_CheckVice ID:%d exeComp:%d Err!", tDbTask_.nID, tDbTask_.nPID);
			}
			else
			{
				// 1~29个比特是真正的序号,30~31为不同命令的区分,32bit空
				tDbTask_.nID |= (tDbTask_.nTableType << 29);

				//tDbTask_.nPID		= db->QueryTask->FieldByName("byComputer")->AsInteger;
				//tDbTask_.nType		= db->QueryTask->FieldByName("taskType")->AsInteger;
				tDbTask_.dwRcvTime = tm;

				//检查关键词可能很长,达到1000bytes, 但目前的结构只支持256bytes ,需改进!!!!!!!!!
				strFile = cmd[3].asBLob();

				//wp  检查类型 guid
				//strcpy_s(s_tDbTask.sGuid, 256, db->QueryTask->FieldByName("result_guid")->AsString.Trim().c_str());
				strcpy_s(tDbTask_.strFile, 256, cmd[4].asString().GetMultiByteChars());
				///s_tDbTask.nType =  db->QueryTask->FieldByName("config")->AsString.Trim();

				// 1 自定义检查
				// 2 系统检查
				// 3 快速检查
				//nChkType = db->QueryTask->FieldByName("type")->AsInteger;
				nChkType=1;
				if (nChkType == 1)
				{
					tDbTask_.nType		= NET_MSG_DISK_DIR_SCAN;

					//检查关键词可能很长,达到1000bytes, 但目前的结构只支持256bytes ,需改进!!!!!!!!!
					if (!strFile.empty())
						strcpy_s(tDbTask_.strExtPara, sizeof(tDbTask_.strExtPara), strFile.c_str());
					//文件目录
					//	strFile = db->QueryTask->FieldByName("")->AsString.Trim();
					//	if (!strFile.IsEmpty())
					//		strcpy_s(s_tDbTask.strFile, sizeof(s_tDbTask.strFile), strFile.c_str());
				}
				else if (nChkType == 2)
				{
					tDbTask_.nType		= NET_MSG_USR_GET_SYSINFO;
				}
				else
				{
					tDbTask_.nType		= NET_MSG_DISK_CHECK;
					if (!strFile.empty())
						strcpy_s(tDbTask_.strFile, sizeof(tDbTask_.strFile), strFile.c_str());
				}

				log_debug("@@@ GetTask: ID:%x Type:%d nPID:%d Key:%s Dir:%s MaxID:%d Sid:%d config:%s",
					 tDbTask_.nID, tDbTask_.nType, tDbTask_.nPID,
					tDbTask_.strFile, tDbTask_.strExtPara, m_nMaxDiskTaskID, tDbTask_.nSsnID, tDbTask_.strFile);

				if (listCmd_.Push(tDbTask_.nID, tDbTask_) == false)
				{
					log_debug("g_listCmd.Push FALSE!");
					break;
				}
				log_debug("g_listCmd.Push TRUE!");

				if ((tDbTask_.nID & DB_TASK_ID_MASK)> m_nMaxDiskTaskID)
				{
					m_nMaxDiskTaskID = (tDbTask_.nID & DB_TASK_ID_MASK);
				}
			}
		}
		if(i <= 0){
			log_debug("GetDiskCheckTask has no task.");
		}
	}catch (SAException &x)
	{
		log_error("GetDiskCheckTask Exp Msg:%s", x.ErrText().GetMultiByteChars());
		handle_database_exception(x);
	}
	catch(...)
	{
		log_error("GetDiskCheckTask Exception!");
	}
}

void MainTask::HandleTask(const time_t tm)
{
	//执行指令列表
	DbTaskList *pList = NULL;
	bool bFind = false;
	int nCmdType = 0;
	int dwErr = 0;
	std::map<int, DbTaskList *>::iterator itr;

	//记录NET_MSG_DISK_DIR_SCAN 是否已经发送过
	static BYTE s_tSsnDirCkState[NET_MAX_SESSION];
	memset(s_tSsnDirCkState, 0, sizeof(s_tSsnDirCkState));

	for(itr = listCmd_.m_mapUsed.begin();itr != listCmd_.m_mapUsed.end();itr++)
	{
		pList = itr->second;
	
		if (pList == NULL || pList->nSsnID<0 || pList->nSsnID>=g_net_max_session)
		{
		    if(pList)
			{
			   log_debug("[HandleTask] error pList->nSsnID = %d\n",pList->nSsnID);
			}else{
			   log_debug("[HandleTask] error  pList == NULL  \n");
			}
			continue;
		}
		//Find the device id
		//g_sDbNetMsgPair[nCmdType] 与 pList->nType 在这个消息上是一致的
		if (pList->nType == NET_MSG_DISK_DIR_SCAN && pList->nStatus == EDB_TASK_SEND)
		{
			s_tSsnDirCkState[pList->nSsnID] = 1;
		}
		if (g_tWorld.GetSession(pList->nSsnID).GetSockState() != E_NET_SOCK_ESTAB)
		{
			log_debug("[HandleTask] (ID=%s) not online  ",g_tWorld.GetSession(pList->nSsnID).m_strPID);
			continue;
		}

		//if (!(pList->nStatus == EDB_TASK_INIT || pList->nStatus == EDB_TASK_SND_ERR))
		//if (pList->nStatus == EDB_TASK_SEND || pList->nStatus == EDB_TASK_OVER)
		if (pList->nStatus != EDB_TASK_INIT)
		{
			log_debug("pList->nStatus != EDB_TASK_INIT");
			continue;
		}

		nCmdType = pList->nType;
		if (nCmdType <= 0 || nCmdType >= sizeof(g_sDbNetMsgPair)/sizeof(g_sDbNetMsgPair[0]) 
			|| g_sDbNetMsgPair[nCmdType] == 0)
		{
			log_debug("RefreshTimer CmdType:%d err! ID:%x PID:%d SID:%d",
				nCmdType, pList->nID, pList->nPID, pList->nSsnID);
			continue;
		}

		//NET_MSG_DISK_DIR_SCAN 消息对于每个会话不能重复发送
		if (g_sDbNetMsgPair[nCmdType] == NET_MSG_DISK_DIR_SCAN && s_tSsnDirCkState[pList->nSsnID])
		{
			//已经下发了类似命令，不能重复下发
			log_debug("NET_MSG_DISK_DIR_SCAN  not repead");
			continue;
		}
		dwErr = g_tWorld.GetSession(pList->nSsnID).SendCommand(g_sDbNetMsgPair[nCmdType],
					pList->nID, pList->nSubAlarm,
					(g_sDbNetMsgPair[nCmdType] == (int)NET_MSG_DISK_DIR_SCAN ? (char*)pList :(char*)pList->strFile));
		if (dwErr == 0)
		{
			//update command status
			pList->nStatus = EDB_TASK_SEND;
			pList->dwSndTime = time(NULL);
			if (g_sDbNetMsgPair[nCmdType] == (int)NET_MSG_DISK_DIR_SCAN)
			{
				s_tSsnDirCkState[pList->nSsnID] = 1;
			}
			log_debug("+++@@ SendCmd:%d DbID:%x Sid:%d NetMsg:%d keyword:%s config:%s",
				nCmdType, pList->nID, pList->nSsnID, g_sDbNetMsgPair[nCmdType], pList->strExtPara, pList->strFile);
		}
		else
		{
			//如果内部错误，当前命令设置服务端异常
			if (dwErr < GR_CUST_ERR_BEGIN && dwErr != GR_ERR_ZERO_LAST_ERR)
			{
				pList->nStatus = EDB_TASK_SVR_ERR;
				pList->nErrCode= dwErr;
				log_debug("UpdateTimer SendCmd:%d Err:%d nID:%d nPID:%d nSub:%d", 
					g_sDbNetMsgPair[nCmdType], dwErr, pList->nID, pList->nPID, pList->nSubAlarm);
			}
		}
	}

	//更新数据库指令状态
	//可以使用SQL IN 语句进行批量更新
	//sql = "select * from [abc] where chkname in ('"& join(a,"','") &"')" 
	char *p=NULL, strExt[32];
	static char s_strUpdateSql[512];
	struct tm *pltm = NULL;
	char sql[1024] = {0};
	try
	{
		for(itr = listCmd_.m_mapUsed.begin();itr != listCmd_.m_mapUsed.end();)
		{
			pList = itr->second;
			/* 任务已经完成 */
			if (pList->nStatus > EDB_TASK_SEND)// == EDB_TASK_OVER || pList->nStatus == EDB_TASK_CLI_ERR)
			{
				if (pList->dwCmpTime == 0)
					pList->dwCmpTime = time(NULL);
				pltm = localtime(&pList->dwCmpTime);
				assert(pltm != NULL);

				if (pList->nTableType == 0 || pList->nTableType == 1)
				{
					//获取文件后缀名
					strExt[0] = 0;
					p = &strExt[0];
					
					sprintf(strExt,"%d",(pList->nID&DB_TASK_ID_MASK));//qizc

					nCmdType = pList->nType;
					if (g_sDbNetMsgPair[nCmdType] == NET_MSG_SNAP_SCRN)
					{
						strcat(strExt, ".jpg");
					}
					else if (g_sDbNetMsgPair[nCmdType] == NET_MSG_UP_LOGCUR )
					{
						strcat(strExt, ".txt");
					}
					else if (g_sDbNetMsgPair[nCmdType] == NET_MSG_UP_FILE )
					{
						if (pList->strFile[0])
						{
							//p = strrchr(pList->strFile, '.');
							p = strrchr(pList->strFile, '\\');
							if (p == NULL)
								p = strrchr(pList->strFile, '.');
							else 
								++p;
						}
						else
							p = NULL;
					}
					else
					{
						p = NULL;
					}
					//如果客户端异常,认为命令无法执行,不写入额外信息
					if (pList->nStatus == EDB_TASK_CLI_ERR || pList->nStatus == EDB_TASK_SVR_ERR)
						p = NULL;

					if (pList->nTableType == 0)
					{
						if (p && strlen(p) > 0 && strlen(p)<256)
							sprintf(sql, "%s'%d-%d-%d %d:%d:%d', file='%s' where ID=%d", "update t_command set t_command.state=4, t_command.finish_time= ",
							pltm->tm_year+1900, pltm->tm_mon+1, pltm->tm_mday, 
							pltm->tm_hour, pltm->tm_min, pltm->tm_sec, p, (pList->nID&DB_TASK_ID_MASK));//pList->nID-10000);
						else
							sprintf(sql, "%s%d %s'%d-%d-%d %d:%d:%d' where ID=%d", "update t_command set t_command.state=",
							pList->nStatus, ",t_command.finish_time= ",
							pltm->tm_year+1900, pltm->tm_mon+1, pltm->tm_mday, 
							pltm->tm_hour, pltm->tm_min, pltm->tm_sec, (pList->nID&DB_TASK_ID_MASK));//pList->nID-10000);
					}
					else
					{
					}
				}
				else	//disk check
				{
					//如果终端报错误响应如何处理暂未定???????
					if (pList->nType == NET_MSG_DISK_CHECK)
						sprintf(sql, "%s'%d-%d-%d %d:%d:%d', file='%s' where ID=%d", "update t_aqjc_result set t_aqjc_result.state=4, t_aqjc_result.finish_time= ",
						pltm->tm_year+1900, pltm->tm_mon+1, pltm->tm_mday, 
						pltm->tm_hour, pltm->tm_min, pltm->tm_sec, "ksjc.txt", (pList->nID&DB_TASK_ID_MASK));//pList->nID-10000);
					else //if (pList->nType == NET_MSG_USR_GET_SYSINFO)
						sprintf(sql, "%s'%d-%d-%d %d:%d:%d', file='%s' where ID=%d", "update t_aqjc_result set t_aqjc_result.state=4, t_aqjc_result.finish_time= ",
						pltm->tm_year+1900, pltm->tm_mon+1, pltm->tm_mday, 
						pltm->tm_hour, pltm->tm_min, pltm->tm_sec, "jcjg.html", (pList->nID&DB_TASK_ID_MASK));//pList->nID-10000);
				}

				strcpy_s(s_strUpdateSql, sizeof(s_strUpdateSql), sql);
				log_debug("   @@ BEFORE UPDATE :%s", s_strUpdateSql);
				SACommand cmd(getSAConnection(), sql);
				cmd.Execute();
				//释放当前节点
				listCmd_.Free(itr);
				log_debug("   @@ Ok:%s", s_strUpdateSql);
				continue;
			}
			else
			{
				//超时回收
			}
			++itr;
		}
	}
	catch (SAException &x)
	{
		log_error("DBTask Update Exp:Cmd:%d ID:%x SubID:%d Msg:%s", 
			nCmdType, (pList?pList->nID:0), (pList?pList->nSubAlarm:0),x.ErrText().GetMultiByteChars());
	}
	catch(...)
	{
		log_error("-DBTask Update Err :Cmd:%d ID:%x SubID:%d", 
			nCmdType, (pList?pList->nID:0), (pList?pList->nSubAlarm:0));
	}

	//超时扫描	//定时器 - 3秒 
	if (s_nRefreshTimerN % 10 == 0) // 30秒扫描周期
	{
		//有一种情况- 终端异常退出,在扫描周期内又登录
		ScanCmdListTime(tm);
	}

	//刷新策略
	try
	{
		CBRefreshChangeTbl();
	}
	catch (const boost::exception &e)
	{
		//	log_debug("CBRefreshChangeTbl Exception Msg:%s", boost::diagnostic_information(e));//qizc
	}
	catch(...)
	{
		log_error("-HandleTask Err !!!");
	}

	//定时扫描心跳
	//待实现
	//g_tWorld.CheckAliveTime(tm);
	log_debug("----- HandleTask exit -----");
}

void MainTask::ScanCmdListTime(const time_t &dwCurTime)
{
	std::map<int, DbTaskList *>::iterator itr;
	DbTaskList *pList = NULL;
	for(itr = listCmd_.m_mapUsed.begin();itr != listCmd_.m_mapUsed.end();++itr)
	{
		pList = itr->second;
		if (pList == NULL)
			continue;
		//如果当前会话终止，则改变状态
		if (pList->nSsnID < 0 || pList->nSsnID >= g_net_max_session)
		{
			log_error("ScanCmdListTime Sid:%d nID:%d PID:%d Table:%d ERR!",
							pList->nSsnID,pList->nID, pList->nPID, pList->nTableType);
			continue;
		}
		if (pList->nStatus == EDB_TASK_SEND)
		{
			//如果当前命令已经发送但客户端断线应当重发
			if (g_tWorld.GetSession(pList->nSsnID).GetSockState() != E_NET_SOCK_ESTAB)
			{
				log_info("ScanCmdListTime Sid:%d nID:%d PID:%d State Change:%d->>>0!",
							pList->nSsnID,pList->nID, pList->nPID, pList->nStatus);
				pList->nStatus = EDB_TASK_INIT;
				continue;
			}
			//如果当前会话的创建时间在命令发送时间之后,显然要重新下发命令
			if (pList->dwSndTime != 0 && pList->dwSndTime<g_tWorld.GetSession(pList->nSsnID).GetCreateTime())
			{
				pList->nStatus = EDB_TASK_INIT;
				log_info("ScanCmdList ssn break Sid:%d nID:%d PID:%d SndTime:%x CreateTime:%x",
							pList->nSsnID,pList->nID, pList->nPID, pList->dwSndTime,
							g_tWorld.GetSession(pList->nSsnID).GetCreateTime());
				continue;
			}

			continue; //后面的暂不执行
					
			//如果超过10分钟无响应,则置状态位
			//如果超过25分钟无响应,则删除这个消息
			//if (pList->nStatus == EDB_TASK_SEND && dwCurTime > pList->dwSndTime)
			//{
			//!!!!! 文件目录扫描可能会占用很长时间!!!!
			if (pList->dwSndTime != 0 && dwCurTime > pList->dwSndTime && dwCurTime - pList->dwSndTime > GR_SVR_CMD_RESET_TIME
 				&& pList->nType != NET_MSG_DISK_DIR_SCAN )
			{
				//释放当前节点->此处不释放-后面统一释放
				//g_listCmd.Free(itr);
				//文件传输类要参考当前传输状态
				//最准确的必须参考最近传输时间
				//文件传输可能会消耗较长时间,但也可能导致?????
				if (g_sDbNetMsgPair[pList->nType] == NET_MSG_UP_FILE
					||g_sDbNetMsgPair[pList->nType] == NET_MSG_UP_LOGCUR
					|| g_sDbNetMsgPair[pList->nType] == NET_MSG_SNAP_SCRN
					|| g_sDbNetMsgPair[pList->nType] == NET_MSG_DISK_CHECK
					|| g_sDbNetMsgPair[pList->nType] == NET_MSG_USR_GET_SYSINFO)
				{
					if (g_tWorld.GetSession(pList->nSsnID).GetRecvState() != FT_STATE_UNKNOWN)
					{
						continue;
					}
				}
				//?????
				//设置命令无响应,是否需要把同步修改会话命令状态?????
				pList->nStatus = EDB_TASK_NO_RSP;
				log_info("ScanCmdListTime Sid:%d nID:%d PID:%d overtime Cur:%d Send:%d!",
								pList->nSsnID,pList->nID, pList->nPID, dwCurTime, pList->dwSndTime);
			}
			//else if (dwCurTime - pList->dwSndTime > 10 * 60)
			//{
			//	pList->nStatus = EDB_TASK_NO_RSP;
			//}
			else
			{
			}
		}
		//++itr;
	}	
}

bool MainTask::CBGetDbUserInfo()
{
	char stmp[1024] = {0};

	tm* t = localtime(&g_tCurLocTime);
	sprintf(stmp, "select ID,userCode,userName,userPass,Status from T_User where STime>='%d-%02d-%02d %d:%d:0'", 
	t->tm_year+1900, t->tm_mon+1, t->tm_mday, 
	t->tm_hour, t->tm_min);

	SACommand cmd(getSAConnection(), stmp);
	cmd.Execute();
	log_debug("[CBGetDbUserInfo] %s",stmp);
	int affect_line = 0; 
	NetUserInfo tUser ;
	//这里应该从数据库获取用户信息，此处临时赋值
	while (cmd.FetchNext()){
		affect_line++;
		tUser.m_nWorkID = cmd[1].asLong();
		
		strcpy(tUser.m_strWID, cmd[2].asString().GetMultiByteChars());

		strcpy(tUser.m_strName, cmd[3].asString().GetMultiByteChars());
		
		strcpy(tUser.m_strPwd, cmd[4].asString().GetMultiByteChars());
		
		tUser.m_dwFlag = cmd[5].asLong();

		//判断同样的工号是否存在，如存在更新否则增加新用户
		g_tWorld.UpdateMapUser(tUser); 
	}
	
	if(!affect_line) return false;
	return true;

}

bool MainTask::CBGetDbDeviceInfo()
{
	char stmp[2048] = {0};
	tm* t = localtime(&g_tCurLocTime);

	sprintf(stmp, "select id,gip, client_guid, hardware, name,hostdepart,hostresponsor from t_client where create_time>='%d-%02d-%02d %d:%d:0'",
		t->tm_year+1900, t->tm_mon+1, t->tm_mday, 
		t->tm_hour, t->tm_min);
	
	log_info("[CBGetDbDeviceInfo]  %s",stmp);
	SACommand cmd(getSAConnection(), stmp);
	cmd.Execute();

	std::string str,str1,str2;
	int nid, nplid, naid;
	int affect_line = 0; 
	//这里应该从数据库获取用户信息，此处临时赋值
	// 先根据设备ID查询是否存在,如果有修改,如没有增加
	while (cmd.FetchNext()){
		affect_line++;
		nid = cmd[1].asLong();
		str = cmd[3].asString().GetMultiByteChars();
		str1 =  cmd[6].asString().GetMultiByteChars();
		str2 = cmd[7].asString().GetMultiByteChars();
		g_tWorld.UpdateDeviceInfo(nid, nplid, naid, str.c_str(),str1.c_str(),str2.c_str());

		DB_MAP_DEVINFO::const_iterator it = NetWorld::m_mapDbDev.find(nid);
		if (it == NetWorld::m_mapDbDev.end())
		{
			DbDeviceInfo*	pDbDev = g_tWorld.GetFreeDbDev();
			if(pDbDev == NULL)
				break;

			//init the device array
			pDbDev->nDbID =  nid;/*id*/
			str = cmd[4].asString().GetMultiByteChars();
			strcpy_s(pDbDev->strHard, NET_MAX_USER_PID, str.c_str()); /*hardware*/
			strcpy_s(pDbDev->strDevName, NET_MAX_USER_PID, str.c_str());/*hardware*/
			str = cmd[5].asString().GetMultiByteChars();
			strcpy_s(pDbDev->strDevCode, 100, str.c_str()); /*name*/
			str = cmd[6].asString().GetMultiByteChars();
			strcpy_s(pDbDev->strHostDepart, 200, str.c_str()); /*hostdepart*/
			str = cmd[7].asString().GetMultiByteChars();
			strcpy_s(pDbDev->strHostResponsor, 200, str.c_str()); /*hostresponsor*/

NetWorld::LockDbDev();
			NetWorld::m_mapDbDev.insert(std::pair<INT32, DbDeviceInfo*>(pDbDev->nDbID, pDbDev));
NetWorld::UnLockDbDev();
		}


	}
	if(!affect_line) return false;
	return true;

}

void MainTask::MainLoop()
{
	int dwRet = 0, dwErr = 0;

	KeyContent  keyCon;
	memset(&keyCon,0,sizeof(keyCon));
	//	log_trace("----- RefreshUpdateTimer start:%d -----", boost::this_thread::get_id());//qizc
	for(;;){
		boost::this_thread::sleep(boost::posix_time::seconds(3));
		//printf("loop %d\n",s_nRefreshTimerN);
#ifdef CHECK_KEY
		//从key中读取sql语句；
		if (s_nRefreshTimerN == 1 || s_nRefreshTimerN % 200 == 0)
		{
			memset(&keyCon,0,sizeof(keyCon));
			if(CheckAndReadKey(keyCon) == 0)
			{
		       printf("this time need read key %d\n",s_nRefreshTimerN);
			}else{
			   printf("[MainLoop]read  key error\n");
			   log_error("[MainLoop]read  key error");
			   exit(0);
			}
		}
#endif 
		//更新用户信息和设备信息至本地缓存,暂不支持删除
		if (s_nRefreshTimerN++ % 10 == 0)
		{
			time_t sys = time(NULL);
			sys -= 5*60;
			g_tCurLocTime = sys;

			try
			{
				log_debug("@@@Update DbUserInfo DbDeviceInfo.");
				CBGetDbUserInfo();
				CBGetDbDeviceInfo();
			}
			catch (SAException &x)
			{
				log_error("MainLoop Exp Msg:%s", x.ErrText().GetMultiByteChars());
				handle_database_exception(x);
			}
			catch(...)
			{
				log_debug("CBGetDbUserInfo Exception!");
			}
		}

		//仅仅获取第一个存活用户
		int i=0;
		for ( i=0;i<g_net_max_session;i++)
		{
			if (g_tWorld.GetSession(i).GetSockState() == E_NET_SOCK_ESTAB)
				break;
		}
		if (i == g_net_max_session)
		{
			continue;
		}

		time_t tm = time(NULL);
		//读取指令列表
		GetCmdTask(tm,keyCon.commandSql);

		//获取专项检查指令列表
		GetDiskCheckTask(tm,keyCon.resultSql);

		HandleTask(tm);
	}
	close_db();
}

//写入指令执行后反馈的结果
//对于文件传输状态，通过pSession->GetTransID()对应;对于其他指令,通过 tMsg.nID 对应
int MainTask::CBWriteCmdAck(const int &nID, const int &nType, const char *pPara)
{
	log_info("-CBWriteCmdAck:%d", nID);
	if (nID <= 0)
		return 0;
	//查找指令列表缓存更新状态
	std::list<DbTaskList *>::iterator iterator;
	DbTaskList *pList = listCmd_.GetObj(nID);//NULL;
	if (pList)
	{
		try{
		if (pList->nID == nID)
		{
			pList->dwCmpTime = time(NULL);
			if (nType == 1)
				pList->nStatus = EDB_TASK_SVR_ERR;
			else
				pList->nStatus = ((pPara != NULL && *(INT32 *)pPara != 0) ? EDB_TASK_CLI_ERR : EDB_TASK_OVER);
			log_info("-CBWriteCmdAck- ID:%x SubID:%d Msg:%d Dir:%d State:%d Err:%d", 
								pList->nID, pList->nSubAlarm, pList->nType, pList->nStatus, nType,
								((pPara != NULL)?*(INT32*)pPara:0));
		}
		else
		{
			//如果两个ID不匹配，说明队列发生错误了
			log_error("-CBWriteCmdAck- QueueErr- PARAID:%x ID:%x SubID:%d Msg:%d State:%d pList:%x", 
								nID, pList->nID, pList->nSubAlarm, pList->nType, pList->nStatus, pList);
		}
		}
		catch(...)
		{
			log_error("-CBWriteCmdAck- QueueErr Catch...- PARAID:%x  pList:%x", 
								nID,  pList);
		}
	}	
	else
	{
		log_error("-CBWriteCmdAck- Not Found-ID:%x QueCount:%x", nID, listCmd_.Count());
	}
    return 1;
}


 axis::wait_list<axis::shared_buffer> g_filecopy_writing_list;


void FileCopyProc()
{
	string curr_dir= current_dir();
	printf("---FileCopyProc  start  %s--- \n",curr_dir.c_str());
	while(1)
	{
		 char src_path[512];
		 char dest_path[512];
		 char filename[256];
		 char cache_path[512];
         char* ptr;
		 memset(filename,0,sizeof(filename));
		 memset(dest_path,0,sizeof(dest_path));
		 memset(src_path,0,sizeof(src_path));
		 memset(cache_path,0,sizeof(cache_path));
	     axis::shared_buffer sb_ret = g_filecopy_writing_list.wait_front();

		 memcpy(src_path, sb_ret.raw_data(), sb_ret.size());
		 ptr = src_path;

		 while(*ptr++){
		    if(*ptr == '\\')
                  *ptr = '/';
		 }
	
		 ptr =  strrchr(src_path,'/');
		 if(!ptr)
		     continue;
		 
		 if(WildCharMatch(src_path,"*.zicha")){
			 sprintf(dest_path,"%s/%s/%s",curr_dir.c_str(),g_T_ZICHA_Backup.c_str(),&ptr[1]);

		 }else if(WildCharMatch(src_path,"*.checkResult")){
			 sprintf(dest_path,"%s/%s/%s",curr_dir.c_str(),g_T_CheckVice_Backup.c_str(),&ptr[1]);
		 }else{
		     //not use
			 printf("=====%s===\n",src_path);
			 continue;
		 }
		 printf("copy src_path = %s, dest_path = %s\n",src_path,dest_path);
		 //开始拷贝
		 sprintf(cache_path,"%s.cache",dest_path);
		 //拷贝成cache文件
		 copyFile(src_path,  cache_path);
		 //重命名文件
		 if (rename(cache_path, dest_path) == 0)
		 {
              printf("rename  cache_path=%s dest_path=%s\n", cache_path, dest_path);
		 }
         else
		 {
              printf("重命名失败，可能原因以前的文件存在\n");
		 }
	}
}
int g_net_max_session=NET_MAX_SESSION;

#ifdef CHECK_KEY

#define VENDORID  "D5C68E77"
#define VENDORPIN  "1qaz3edc$RFV^YHN$RFV^YHN"
extern int ReadKey(char szVendorID[16], char szVendorPin[64], KeyContent& keyCon);
int   CheckAndReadKey(KeyContent&  keyCon)
{
	int nret = 0;
	char szVendorID[16];
	char szVendorPin[64];

	strcpy(szVendorID,VENDORID);
	strcpy(szVendorPin,VENDORPIN);
	nret=ReadKey(szVendorID,szVendorPin,keyCon);
	return nret;
}

void check_key()
{
	KeyContent  keyCon;
	memset(&keyCon,0,sizeof(keyCon));
	int nret = 0;
	printf("[check_key] start....\n");
	while(1){
		if( (nret=CheckAndReadKey(keyCon))==0 )
		{
			g_net_max_session = keyCon.userNum > NET_MAX_SESSION ? NET_MAX_SESSION : keyCon.userNum;
		    printf("key nret = %d g_net_max_session=%d \n",nret,g_net_max_session);
			log_info("key nret = %d g_net_max_session=%d \n",nret,g_net_max_session);
			return ;
		}
		else
		{
			if(nret== -87){
				log_error("key expired  nret = %d\n",nret);
			}else{
			  log_error("key error nret = %d\n",nret);
			}
			printf("key error nret = %d\n",nret);
			MySleepMilliSecond(5000);
		}
	}
}
#endif







#ifndef CFG_MDL_UPGRADE_SVR
int _tmain(int argc, char* argv[])
{	

#if 0
	int AuthRspTest();
	AuthRspTest();
	return 0;
#endif
	read_config();
	initLog("");

#ifdef CHECK_KEY
	check_key();
#endif
	log_info("------------------------------------ start --------------------------------------- ");
	string curr_dir= current_dir();
	sprintf(NetSession::s_strZiChaDir, "%s%s/", curr_dir.c_str() ,g_T_ZICHA.c_str());
	sprintf(NetSession::s_strCheckDir, "%s%s/", curr_dir.c_str() ,g_T_CheckVice.c_str());
	sprintf(NetSession::s_strTaskDir, "%s%s/", curr_dir.c_str() ,g_T_Log.c_str());

	if(!g_mt.ConnectDB()){
		printf("数据库连接错误，请检查数据库配置是否正确。\n");
		return 0;
	}
	/* 初始化设备信息 */
	g_mt.InitDbDeviceInfo();
	/* 初始化用户信息 */
	if (g_mt.InitDbUserInfo() == FALSE)
	{
		log_error("g_tWorld InitDbUserInfo Error!");
	}
	//从数据库获取策略
	//关键词-文件后缀名-告警等级控制-告警模板-变化表版本号
	CBUpldateLocalPolicy();
	g_tWorld.SetFRMonGbl(&g_tDbRegstyMon, &g_tDbFilesMon);
	g_login.InitLogInfoProc(NULL);
	/*文件拷贝线程*/
	boost::thread td3(boost::bind(FileCopyProc));
	/* 登录线程 */
	boost::thread td(boost::bind(&LoginManage::DbLoginsProc, &g_login));

	/* 主任务线程，读数据库，更新内存，投递命令 */
	boost::thread td2(boost::bind(&MainTask::MainLoop,&g_mt));

	myNetServ m;
	g_m=&m;
	m.start();

	td.join();
	td2.join();
	return 0;
}


#else

void  AppUpdate_StartListen()
{
	read_config();
	initLog("");
	//myNetServ m;
	g_m=new myNetServ();
    g_m->start();
}

void AppUpdate_SetTransBuf(char *p, unsigned int  len)
{
	  printf("AppUpdate_SetTransBuf = %d\n",len);
	  NetWorld::SetTransBuf(p, len);
}

void  AppUpdate_SetVersion(unsigned short w)
{
      NetWorld::SetVersion(w);
	  printf("AppUpdate_SetVersion= %d\n",NetWorld::GetVersion());
}

void  AppUpdate_SetVfileN( unsigned short w)
{
     NetWorld::SetVfileN(w);
}

void  AppUpdate_SetAllowedN(int w)
{
	NetWorld::SetAllowedN(w);
}


#endif

	