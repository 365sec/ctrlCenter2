#ifndef  UTILS_H
#define UTILS_H
#include<string>
#define CS_NO	0	//���Դ�Сд
#define CS_YES	1	//���ִ�Сд

bool WildCharMatch(const char *src,const char* pattern,bool bCaseSensitive=CS_NO);
int copyFile(const char *sourceFileNameWithPath,   const char *targetFileNameWithPath) ;
std::string& replace_all_distinct(std::string& str,const std::string& old_value,const std::string& new_value) ;
#endif