#ifndef _LIB_FUN_H_
#define _LIB_FUN_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <vector>

using namespace std;
string addr2string(unsigned int addr);
int ping(const char * remote_host, int timeout = 200);
void system_result(const char *command, string& s);


#endif
