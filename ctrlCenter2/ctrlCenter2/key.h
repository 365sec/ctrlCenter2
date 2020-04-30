//---------------------------------------------------------------------------

#ifndef keyH
#define keyH
//---------------------------------------------------------------------------

//wp增加key结构体
typedef struct _KEY_CONTENT
{
    char commandSql[100];
    char resultSql[100];
    char clientSql[100];
	char userSql[100];
	char key_version[100];
    int userNum;
	unsigned int lasttime;

}KeyContent;

/*select id, client_guid, command_type, file from t_command where state=1 and ID>|select id,client_guid,keywords,config from t_aqjc_result where state=1 and ID>|select id, gip, client_guid, hardware,name from t_client|select ID,userCode,userName,userPass,Status from T_User */

extern KeyContent g_kc;

#endif
