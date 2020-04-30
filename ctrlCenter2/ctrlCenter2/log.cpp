#include "log.h"
#include <stdio.h>


#ifndef OS_LINUX

#include <Windows.h>

    void    TraceLevelLog(const char *fmt, ...)
    {
        va_list args;
        int n;
        char buf[4096] = "                                                                ";

        //int offset=strlen(buf);
        va_start(args, fmt);
        //n = vsprintf(buf+offset, fmt, args);
        n = vsprintf(buf, fmt, args);
        va_end(args);

        printf("%s\n", buf);

        OutputDebugString(buf);
        OutputDebugString("\n");
    }

#endif

    void printbuf(char buf[], int len)
    {
#ifdef DEBUG
        char tb[4096];
        printf("===============================\n");
        OutputDebugString("======================\n");
        for(int m=0; m<len; m++)
        {
            if (0 == (m&0xf))
            {
                sprintf(tb, "\n%08d", m/16);
                printf(tb);
                OutputDebugString(tb);
            }
            sprintf(tb, "%02x ", buf[m]&0xff);
            printf(tb);
            OutputDebugString(tb);
        }
        printf("\n");
        OutputDebugString("\n");
#endif
    }


