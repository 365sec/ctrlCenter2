#ifndef TEST_LOG_H
#define TEST_LOG_H

#include "customtypedef.h"

#ifdef OS_LINUX
    #define TraceLevelLog   printf
    #define OutputDebugString(a)
#else
    void    TraceLevelLog(const char *fmt, ...);
#endif

    void    printbuf(char buf[], int len);


#endif  //TEST_LOG_H
