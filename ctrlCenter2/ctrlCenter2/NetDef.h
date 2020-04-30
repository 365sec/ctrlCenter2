
#ifndef _GRXA_GUARD_NET_DEF_H__
#define _GRXA_GUARD_NET_DEF_H__

//���ļ�����NETģ�鹫�õ����ݽṹ�ͺ�
//#ifdef CBUILDER_VER_6
//#define GR_NET_API
//#else
#ifdef GA_STATIC_RELEASE
#define GR_NET_API 
#else
#ifdef GRNET_EXPORTS
#define GR_NET_API __declspec(dllexport)
#else
//#define GR_NET_API __declspec(dllimport)
#define GR_NET_API
#endif
#endif
//#endif

extern  int g_net_max_session; 	//�����֧�ֵ����Ự��Ŀ
#define NET_MAX_SESSION			1000	//�����֧�ֵ����Ự��Ŀ
#define NET_MAX_DEACT_TIME		480		//max deactive time �Ự�������ʱ��6����û�м�������Ϊ��������Ҫ�ر�
#define NET_MAX_ACPTNODATA_TIME	120		//ACCEPTEX ���������������ݴ���ʱ��
#define NET_MAX_USER_PID		100		//�ն������ʶ����ַ�������
#define NET_MAX_USER_NAME		16		//user name����ַ�������
#define	NET_MAX_SSN_USERS		1000		//max client users of the system
#define	NET_MAX_SVR_DEVICE		1500		//max computer number configured by the server 
#define	NET_MAX_CNTXT_N			3600	//max context used for iocp post (exclude accept context)
#define	NET_MAX_ACPT_CNTXT_N	200		//max context used for iocp acceptex post
#define	NET_MAX_NETDATA_N		48000	//max data used for buffer (send and recv)

#define	NET_MAX_CLICNTXT_N		500		//max context used for iocp post (exclude accept context)
#define	NET_MAX_CLIACPTCN_N		0		//max context used for iocp acceptex post
#define	NET_MAX_CLINETDATA_N	1000	//max data used for buffer (send and recv)

#define	NET_MAX_BUF_LEN			1500	//max data buffer
#define NET_MAX_PERDATA_N		1400	//TCP

#define NET_CRTSECT_SHIFT		5
#define NET_MAX_CRTSECT_SSN		(1<<NET_CRTSECT_SHIFT)	//��Ӧ�ڸ����Ự����
#define NET_SVR_LOCK_MASK		0x1f					//5bits ->NET_CRTSECT_SHIFT

//SOCKET״̬����쳣����Ҫ��¼�쳣���ͺ�ԭ��
//�ļ��������ר�ŵ�״ֵ̬���������ִ���״̬��ҵ�����
enum NET_SOCK_CLISTATE_E
{
	E_NET_SOCK_UNKNOWN 	= 0,
	E_NET_SOCK_INIT		= 1,
	E_NET_SOCK_CONNECT	= 2,
	E_NET_SOCK_ESTAB	= 4,
	E_NET_SOCK_CLOSED	= 8,
	E_NET_SOCK_ERR		=0x10,
	E_NET_SOCK_INITERR	=0x20,
	E_NET_SOCK_CONNERR	=0x40,
	E_NET_SOCK_TRANERR	=0x80,
//	E_NET_SOCK_TRANS	=0x100,
};
#define E_NET_SOCK_ALLERR	0x0f0

#include "GuardDef.h"
#include"both.h"
#endif

