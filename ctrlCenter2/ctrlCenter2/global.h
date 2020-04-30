#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#include <string>
#include "sync_map.hpp"
#include "typedef.h"
#include "shared_buffer.hpp"


using namespace std;

typedef unsigned char byte;
extern int g_listen_port;
extern int g_http_port;
extern string tcp_data_string;
extern string tcp_http_string;
extern string g_db_name;
extern string g_db_user;
extern string g_db_passwd;
extern int g_num_threads;
extern int g_log_level;

extern string g_T_TaskList ; 
extern string g_T_CheckVice;
extern string g_T_CheckVice_Backup;

extern string g_T_Log;
extern string g_T_ZICHA ;
extern string g_T_ZICHA_Backup;


extern axis::wait_list<axis::shared_buffer> g_db_writing_list;

const double g_version = 0.2;

extern int	g_sDbNetMsgPair[101];
#endif
