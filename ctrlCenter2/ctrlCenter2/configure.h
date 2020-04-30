#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include <iostream>
#include <sstream>

#include "ini.h"
#include "typedef.h"
#include "global.h"

using namespace std;

void read_config(){
	

	read_ini ri("config.ini");

#ifndef CFG_MDL_UPGRADE_SVR
	ri.find_value("listen_port:", g_listen_port);//监听端口
	ri.find_value("http_port:",g_http_port);//调试页面端口
#else
	ri.find_value("update_port:", g_listen_port);//监听端口
	ri.find_value("update_http_port:",g_http_port);//调试页面端口
#endif
	
	ri.find_value("db_name:", g_db_name); //"127.0.0.1@CAT";
	ri.find_value("db_user:",g_db_user);
	ri.find_value("db_passwd:",g_db_passwd);
	ri.find_value("num_threads:",g_num_threads);//异步IO的多线程数量
	ri.find_value("log_level:",g_log_level);
    
	ri.find_value("log_level:",g_log_level);

	ri.find_value("T_CheckVice:",g_T_CheckVice);
	ri.find_value("T_CheckVice_Backup:",g_T_CheckVice_Backup);

	ri.find_value("T_Log:",g_T_Log);
	ri.find_value("T_ZICHA:",g_T_ZICHA);
	ri.find_value("T_ZICHA_Backup:",g_T_ZICHA_Backup);

	cout<<"log_level:"<<g_log_level<<endl;
	cout<<"g_listen_port:"<<g_listen_port<<endl;
	cout<<"g_db_name:"<<g_db_name<<endl;
	cout<<"g_num_threads:"<<g_num_threads<<endl;

	cout<<"T_Log:"<<g_T_Log<<endl;
	cout<<"T_ZICHA:" << g_T_ZICHA<<endl;
	cout<<"T_ZICHA_Backup:" << g_T_ZICHA_Backup<<endl;
	cout <<"T_CheckVice:" << g_T_CheckVice<<endl;
	cout <<"T_CheckVice_Backup:" << g_T_CheckVice_Backup<<endl;

}

void save_config(){
	stringstream ss;
	ss  << "listen_port:" << g_listen_port << "\r\n"
		<< "local_mac:" << "test_mac" << "\r\n";

	FILE * f = fopen("config.ini", "wb");
	if(f){
		fwrite(ss.str().c_str(), 1, ss.str().size(), f);
		fclose(f);
	}else{
		printf("save config failed\n");
	}
}

#endif