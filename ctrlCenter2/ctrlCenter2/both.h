#pragma once
#pragma warning(disable:4200)


#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
	#include <process.h>
#else
	#include <errno.h>
	#include <string.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <sys/wait.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>

	#include <pthread.h>
	#include <unistd.h>
	#include<sys/stat.h>
	#include<fcntl.h>
#endif


//编译----本工程，一定要注意顺序。
//gcc -std=c++11 -lstdc++ -lpthread -I/usr/local/openssl/include CBCdes.cpp /usr/local/openssl/lib/libcrypto.a -ldl Collect.cpp /opt/zlib/lib/libz.a grwifi.cpp -o wifimgr
#ifndef uint_16
#define uint_16 unsigned short
#endif

#ifndef uint_32
#define uint_16 unsigned short
#endif

#define ADD(x,y)			(x+y)


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef Add2Ptr
#define Add2Ptr(P,I) ((void*)((unsigned char*)(P) + (I)))
#endif

	#define myread(x,y,z)	recv(x,y,z,0)

#ifdef _WIN32
	#define	MyInitCsMutex(x)	InitializeCriticalSection(&x)
	#define	MY_CS_MUTEX	CRITICAL_SECTION
	#define MY_LOCK		EnterCriticalSection	
	#define MY_UNLOCK	LeaveCriticalSection	
	#define MyDeleteMutex DeleteCriticalSection

	#define THREAD_RET	void
	#define	DO_RET
	#define	MY_BEGINTHREAD(x,y,z)	z=_beginthread((x),0,(y))
	#define MY_WAITFOR(x)			WaitForSingleObject((HANDLE)x,INFINITE)
	#define	MYTIMEOUT(x)			(x*1000)	//单位毫秒


	#define bzero(x,y)		memset(x,0,y)
	#define socklen_t		int
	#define	MyCloseSocket(x)	closesocket(x)
	#define	MyShutDown(x)		closesocket(x)
	#define MySleepMilliSecond(x)		Sleep(x)		//单位毫秒
	#define THREADID		unsigned long
	#define MySend(x,y,z)	send(x,y,z,0)
	#define Myperror(x)		printf(x" error=%d\n",GetLastError())
	#define	MyINT64			__int64
	#define	STRLL			"%I64d"

	#define MyGetTID()		GetCurrentThreadId()
	#define MyGetPID()		GetCurrentProcessId()
	#define my_stricmp		stricmp
#else
	#define	MyInitCsMutex(x)	(x=PTHREAD_MUTEX_INITIALIZER)
	#define	MY_CS_MUTEX	pthread_mutex_t
	#define MY_LOCK		pthread_mutex_lock	
	#define MY_UNLOCK	pthread_mutex_unlock	
	#define MyDeleteMutex pthread_mutex_destroy

	#define THREAD_RET	void*
	#define	DO_RET		return((THREAD_RET)0)
	#define	MY_BEGINTHREAD(x,y,z)	pthread_create(&z,NULL,(x),(y))	
	#define MY_WAITFOR(x)			pthread_join(x,NULL)
	#define	MYTIMEOUT(x)			(x)		//单位秒


	#define	MyCloseSocket(x)	close(x)
	#define	MyShutDown(x)		shutdown(x,2)	//how=0 终止读取操作。how=1 终止传送操作 how=2 终止读取及传送操作
	#define MySleepMilliSecond(x)		usleep(x*1000)		//单位微妙
	#define THREADID		pthread_t
	#define MySend(x,y,z)	write(x,y,z)
	#define Myperror(x)		perror(x)
	#define	MyINT64			long long
	#define	STRLL			"%lld"

	#define MyGetTID()		gettid()
	#define MyGetPID()		getpid()
	#define my_stricmp		strcasecmp
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE 1 
#endif

#ifndef FALSE
#define FALSE 0
#endif

//qizc
#ifndef UINT16
typedef unsigned short UINT16;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef ULONG32
typedef unsigned int ULONG32;
//#define ULONG32 unsigned long
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif

#ifndef INT32
typedef int INT32;
#endif

#ifndef UINT32
typedef unsigned int UINT32;
#endif

#ifndef MAX_PATH
#define MAX_PATH (260*2)
#endif

#ifndef INT
typedef int INT;
#endif

#ifndef VOID
typedef void VOID;
#endif

#ifndef LONG
typedef long LONG;
#endif


//
//gcc -lstdc++ -lpthread Collect.cpp grwifi.cpp -o wifimgr
//gcc++ -lpthread Collect.cpp grwifi.cpp -o wifimgr
//
//	上面两句命令可能有警告
//  extended initializer lists only available with -std=c++11 or -std=gnu++11 [默认启用]
//
//
//gcc -std=c++11 -lstdc++ -lpthread Collect.cpp grwifi.cpp -o wifimgr
//gcc -std=gnu++11 -lstdc++ -lpthread Collect.cpp grwifi.cpp -o wifimgr
//

//编译----例子----openssl
//gcc -I/usr/local/openssl/include destest.cpp /usr/local/openssl/lib/libcrypto.a -ldl -o destest

//编译----例子----zip
//gcc  1.cpp /opt/zlib/lib/libz.a -o 1

//编译----本工程，一定要注意顺序。
//gcc -std=c++11 -lstdc++ -lpthread -I/usr/local/openssl/include CBCdes.cpp /usr/local/openssl/lib/libcrypto.a -ldl Collect.cpp /opt/zlib/lib/libz.a grwifi.cpp -o wifimgr


/*
【安装补丁】针对64位CentOS的 /lib64/libc.so.6: version `GLIBC_2.14' not found
	查看当前 glibc 版本：rpm -qa |grep glibc
	查看当前 glibc 版本：strings /lib64/libc.so.6 |grep GLIBC_
	为什么需要查看？如果已经安装，仅需要配置环境。


	[root@localhost ~]# tar xvf glibc-2.14.tar.gz
	[root@localhost ~]# cd glibc-2.14
	[root@localhost glibc-2.14]# mkdir build
	[root@localhost glibc-2.14]# cd ./build
	[root@localhost build]# ../configure --prefix=/lib64/glibc-2.14
	[root@localhost build]# make -j4
	[root@localhost build]# make install
	
【如果已经安装】
	//下面这一步可以不需要
	[root@localhost 你的路径]两个都试一下
	# export LD_LIBRARY_PATH=/lib64/glibc-2.14/lib:$LD_LIBRARY_PATH
	# export LD_LIBRARY_PATH=/opt/glibc-2.14/lib:$LD_LIBRARY_PATH
*/

/*

  //把下面两句加到 /etc/profile	用法：
  //vi profile---方向键，到最后一个字母----任意字符，切换为insert状态-----粘贴-----ESC----:----wq
  export LD_LIBRARY_PATH=/opt/glibc-2.14/lib:$LD_LIBRARY_PATH
  TZ='Asia/Shanghai'; export TZ
*/

/*
【没有权限执行】
	查看各文件权限：ll 
	修改权限：chmod ［mode］ 文件名
	举个例子：chmod a+x grwifi
		  ./grwifi
*/

/*
一：Linux平台

$wget http://www.zlib.net/zlib-1.2.3.tar.gz
$tar -xvzf zlib-1.2.3.tar.gz
$cd zlib-1.2.3.tar.gz
$./configure --prefix=/opt/zlib 或者就是 $./configure
$make
$sudo make install

　　首先看看自己的机器上是不是已经安装好zlib了：

　　whereis zlib

　　如果安装好了，会输出zlib的路径，这样直接跳过前2步。

　　1.　　下载zlib的最新版，我的是1.2.3
　　2.　　解压，编译：

  　　./configure --prefix=/opt/zlib
	　　make
	　　sudo make install

　　3.　　zlib安装好了，下面我们写一个程序测试一下：

  编译。注意 -lz 顺序在后

    gcc -Wall -o 1 1.cpp -lz
	gcc  1.cpp /opt/zlib/lib/libz.a -o 1

clean:   
    rm -rf *.o test  
　　注意到，我们用-lz加入了zlib库

　　test.c

二、Windows 平台。
	zlib-1.2.3\projects\visualc6\zlib.dsw 选择 Win32_LIB_Release 生成静态库 ，
		并添加 zlib.h/zconf.h

*/


//lib文件--相对路径，可能linux环境配置不好，会失败 
//gcc -I/usr/local/openssl/include destest.cpp -lcrypto -ldl -o destest

//lib文件--完整路径
//gcc -I/usr/local/openssl/include destest.cpp /usr/local/openssl/lib/libcrypto.a -ldl -o destest


/*
1、下载地址：http://www.openssl.org/source/ 下一个新版本的OpenSSL，我下的版本是：openssl-1.0.0e.tar.gz
2、在下载的GZ目录中，用命令执行：tar -xzf openssl-1.0.0e.tar.gz
3、进入解压的目录：openssl-1.0.0e  [.......]#cd openssl-1.0.0e
4、[.....openssl-1.0.0e]# ./config --prefix=/usr/local/openssl
5、[...../openssl-1.0.0e]# ./config -t
6、[...../openssl-1.0.0e]# make depend
7、[...../openssl-1.0.0e]# cd /usr/local
8、/usr/local]# ln -s openssl ssl
9、在/etc/ld.so.conf文件的最后面，添加如下内容：【题外话vi工具 缺省命令行模式，i进入修改模式--->esc--->wq】
	/usr/local/openssl/lib
10、...]# ldconfig
11、添加OPESSL的环境变量：
	在etc／的profile的最后一行，添加：
	export OPENSSL=/usr/local/openssl/bin
	export PATH=$OPENSSL:$PATH:$HOME/bin
12、退出命令界面，再从新登录。
13、以上OPENSSL就安装完毕，下面进行一些检查。
14、依次如下执行：
[root@localhost /]# cd /usr/local
[root@localhost local]# ldd /usr/local/openssl/bin/openssl
	会出现类似如下信息：
    	linux-vdso.so.1 =>  (0x00007fff3bc73000)
    	libdl.so.2 => /lib64/libdl.so.2 (0x00007fc5385d7000)
    	libc.so.6 => /lib64/libc.so.6 (0x00007fc538279000)
    	/lib64/ld-linux-x86-64.so.2 (0x00007fc5387db000)

15、查看路径
...]# which openssl
	/usr/local/openssl/bin/openssl

16、查看版本
...]# openssl version
	OpenSSL 1.0.0e 6 Sep 2011
*/