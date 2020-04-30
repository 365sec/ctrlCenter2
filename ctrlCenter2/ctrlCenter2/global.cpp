
//#include "global.h"
#include <string>
#include "typedef.h"
#include "sync_map.hpp"
#include "shared_buffer.hpp"
#include "NetUtlity.h"

using namespace std;


int g_listen_port = 10070;
int g_http_port = 10080;
int g_num_threads = 1;
int g_log_level = 0;//trace:0,debug:1,info:2,warning:3,error:4,fatal:5

string g_db_name = "";
string g_db_user = "";
string g_db_passwd = "";
string tcp_data_string = "local data tcp";
string tcp_http_string = "local http";

string g_T_TaskList = "";  //
string g_T_CheckVice = ""; //���
string g_T_CheckVice_Backup = ""; //��鱸��
string g_T_Log = ""; //��־
string g_T_ZICHA = ""; //�Բ�
string g_T_ZICHA_Backup="";//�Բ鱸��



axis::wait_list<axis::shared_buffer> g_db_writing_list;


int	g_sDbNetMsgPair[] =
{
    0                	, // 0 		
    NET_MSG_NET_SVRONLY                	, // 1 	DB_CMD_UN_NET	//������� 	
    NET_MSG_NET_ENABLE                	, // 2 	DB_CMD_EN_NET	//�ָ����� 	
    NET_MSG_USB_DISABLE                	, // 3 	DB_CMD_UN_USB	//USB���  	
    NET_MSG_USB_ENABLE                	, // 4 	DB_CMD_EN_USB	//�ָ�USB  	
    NET_MSG_BLOCK_ALL                	, // 5 	DB_CMD_UN_SCRN	//����     	
    NET_MSG_UNBLOCK_ALL                	, // 6 	DB_CMD_EN_SCRN	//������� 	
    NET_MSG_SGL_NOTICE                	, // 7 	DB_CMD_SGL_NOTICE	//�������Ϣ֪ͨ
    0                	, // 8 		
    0                	, // 9 		
    0                	, // 10		
    0                	, // 11		
    0                	, // 12		
    0                	, // 13		
    0                	, // 14		
    0                	, // 15		
    0                	, // 16		
    0                	, // 17		
    0                	, // 18		
    0                	, // 19		
    0                	, // 20		
    0                	, // 21		
    0                	, // 22		
    0                	, // 23		
    0                	, // 24		
    0                	, // 25		
    0                	, // 26		
    0                	, // 27		
    0                	, // 28		
    0                	, // 29		
    0                	, // 30		
    NET_MSG_CTRLSTATE_REQ                	, // 31	DB_CMD_GET_CTRL		//��ȡ����״̬     	
    0                	, // 32	DB_CMD_SET_CTRL		//���ÿ���״̬     	
    NET_MSG_KEY3_POLICY                	, // 33	DB_CMD_UPD_KEYPLY	//���¹ؼ��ʲ���   	
    NET_MSG_EXT_POLICY                	, // 34	DB_CMD_UPD_EXTPLY	//����������׺���� 	
    0                	, // 35	DB_CMD_SET_CTRL_IP	//���ÿ��Ʒ�����IP 	
    0                	, // 36	DB_CMD_SET_DAT_IP	//�������ݷ�����IP 	
    0                	, // 37		
    0                	, // 38		
    0                	, // 39		
    0                	, // 40		
    0                	, // 41		
    0                	, // 42		
    0                	, // 43		
    0                	, // 44		
    0                	, // 45		
    0                	, // 46		
    0                	, // 47		
    0                	, // 48		
    0                	, // 49		
    NET_MSG_DISK_CHECK	, // 50		
    0                	, // 51		
    0                	, // 52		
    0                	, // 53		
    0                	, // 54		
    NET_MSG_USR_GET_SYSINFO, // 55		
    NET_MSG_DISK_DIR_SCAN, // 56		
    0                	, // 57		
    0                	, // 58		
    0                	, // 59		
    0                	, // 60		
    NET_MSG_SNAP_SCRN                	, // 61	DB_CMD_UP_SCRN		//�ϴ�����
    NET_MSG_UP_FILE                 	, // 62	DB_CMD_UP_FILE		//�ϴ��ļ�
    NET_MSG_UP_LOGCUR                	, // 63	DB_CMD_UP_LOG		//�ϴ���־
    0                	, // 64		
    0                	, // 65		
    0                	, // 66		
    0                	, // 67		
    0                	, // 68		
    0                	, // 69		
    NET_MSG_UP_PROCESS                	, // 70	DB_CMD_UP_PROCLIST	//��ȡ�����б�
    NET_MSG_UP_SERVICES                	, // 71	DB_CMD_UP_SRVCLIST	//��ȡ�����б�
    NET_MSG_UP_SOFTWARE                	, // 72	DB_CMD_UP_SOFTLIST	//��ȡ����б�
    0                	, // 73		
    0                	, // 74		
    0                	, // 75		
    0                	, // 76		
    0                	, // 77		
    0                	, // 78		
    0                	, // 79		
    0                	, // 80		
    0                	, // 81		
    0                	, // 82		
    0                	, // 83		
    0                	, // 84		
    0                	, // 85		
    0                	, // 86		
    0                	, // 87		
    0                	, // 88		
    0                	, // 89		
    0                	, // 90		
    0                	, // 91		
    0                	, // 92		
    0                	, // 93		
    0                	, // 94		
    0                	, // 95		
    0                	, // 96		
    0                	, // 97		
    0                	, // 98		
    0                	, // 99		
    0                	, // 100
};



