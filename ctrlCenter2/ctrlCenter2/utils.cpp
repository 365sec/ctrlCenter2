#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"utils.h"
#include "log_utility.h"



#ifndef WIN32
#include <stdio.h>      /*fprintf(),stderr,BUFSIZ*/
#include <stdlib.h>     /**/
#include <string.h>     /*stderror()*/
#include <fcntl.h>      /*open(),flag*/
#include <errno.h>      /*errno*/
#include <unistd.h>     
#include <sys/stat.h>   
#endif


//匹配一个字符的宏
#define MATCH_CHAR(c1,c2,x)  ( (bool)( (c1==c2) || ((x==CS_NO)&&(tolower(c1)==tolower(c2))) ) )




int ReverseCountWild(const char* sz)
{
	int count=0;
	int size=strlen(sz);

	for(int i=size-1;size>=0;i--)
	{
		if(sz[i]=='*'||sz[i]=='?')
		{
			count++;
		}
		else
		{
			break;
		}
	}

	return count;
}

//
//  通配符匹配算法。没有完全验证，其中“?”完全没有验证。如“*.txt”有BUG。
//	src：字符串，pattern：含有通配符(*或?)的字符串，bCaseSensitive：是否区分大小写
//
bool WildCharMatch_Oneway(const char *src,const char* pattern,const char* patternOriginal,bool bCaseSensitive=CS_NO)
{
	bool result=false;

	while(*src)
	{
		if(*pattern == '*')
		{
			// 如果 pattern 的当前字符是 '*' 
			// 如果后续有多个 '*', 跳过
			while( (*pattern == '*') || (*pattern == '?') )
				pattern++;

			// 如果 '*" 后没有字符了，则正确匹配
			if (!*pattern)
			{
				return true;
			}

			// 在 src 中查找一个与 pattern中'*"后的一个字符相同的字符
			while(*src && (!MATCH_CHAR(*src,*pattern,bCaseSensitive)))
				src++;

			// 如果找不到，则匹配失败
			if (!*src)
			{
				return false;
			}

			// 如果找到了，匹配剩下的字符串
			result = WildCharMatch_Oneway(src,pattern,patternOriginal,bCaseSensitive);

			// 如果剩下的字符串匹配不上，但src后一个字符等于pattern中'*"后的一个字符
			// src前进一位，继续匹配
			while( (!result) && (*(src+1)) && MATCH_CHAR(*(src+1),*pattern,bCaseSensitive) )
				result = WildCharMatch_Oneway (++src, pattern,patternOriginal,bCaseSensitive);

			return result;
		}
		else
		{
			// 如果pattern中当前字符不是 '*' 
			// 匹配当前字符
			if( MATCH_CHAR(*src,*pattern,bCaseSensitive) || ('?'==*pattern) )
			{ 
				// src,pattern分别前进一位，继续匹配 
				return WildCharMatch_Oneway(++src,++pattern,patternOriginal,bCaseSensitive);
			}
			else
			{
				//qizc修正-------start

				//
				//	pattern还没有到尾
				//	被搜索长度大于pattern除去末尾通配符的长度
				//	pattern以*开头
				//
				if( *pattern && strlen(src)>strlen(pattern)-ReverseCountWild(pattern) && *patternOriginal=='*' )
				{
					return WildCharMatch_Oneway(++src,patternOriginal,patternOriginal,bCaseSensitive);
				}
				else

				//qizc修正---------end

				return false;
			}
		}
	}


	// 如果src结束了，看pattern有否结束       
	if(*pattern)  
	{ 
		// pattern没有结束          
		if ( (*pattern=='*') && (*(pattern+1)==0) ) // 如果pattern有最后一位字符且是'*' 
			return true;
		else
			return false;
	}
	else
		return true;


	return false;
}

bool WildCharMatch(const char *src,const char* pattern,bool bCaseSensitive)
{
	bool ret=false;
	ret=WildCharMatch_Oneway(src,pattern,pattern,bCaseSensitive);
	return ret;
}

#ifndef WIN32

#define BUFFER_SIZE 4096
 
int copyFile(const char *sourceFileNameWithPath,  
        const char *targetFileNameWithPath)  
{
   int from_fd,to_fd;
   int bytes_read,bytes_write;
   char buffer[BUFFER_SIZE];
   char *ptr;
 
 
   /* 打开源文件 */
   if((from_fd=open(sourceFileNameWithPath,O_RDONLY))==-1)   /*open file readonly,返回-1表示出错，否则返回文件描述符*/
   {
     log_error("Open %s Error:%s\n",sourceFileNameWithPath,strerror(errno));
     return 0;
   }
 
   /* 创建目的文件 */
   /* 使用了O_CREAT选项-创建文件,open()函数需要第3个参数,
      mode=S_IRUSR|S_IWUSR表示S_IRUSR 用户可以读 S_IWUSR 用户可以写*/
   if((to_fd=open(targetFileNameWithPath,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1) 
   {
      log_error("Open %s Error:%s\n",targetFileNameWithPath,strerror(errno));
     return 0;
   }
 
   /* 以下代码是一个经典的拷贝文件的代码 */
   while(bytes_read=read(from_fd,buffer,BUFFER_SIZE))
   {
     /* 一个致命的错误发生了 */
     if((bytes_read==-1)&&(errno!=EINTR)) 
         break;
     else if(bytes_read>0)
     {
       ptr=buffer;
       while(bytes_write=write(to_fd,ptr,bytes_read))
       {
         /* 一个致命错误发生了 */
         if((bytes_write==-1)&&(errno!=EINTR))
             break;
         /* 写完了所有读的字节 */
         else if(bytes_write==bytes_read) 
             break;
         /* 只写了一部分,继续写 */
         else if(bytes_write>0)
         {
           ptr+=bytes_write;
           bytes_read-=bytes_write;
         }
       }
       /* 写的时候发生的致命错误 */
       if(bytes_write==-1)
           break;
     }
   }
   close(from_fd);
   close(to_fd);
   return  1;
} 



#else
int copyFile(const char *sourceFileNameWithPath,  
        const char *targetFileNameWithPath)  
{  
	#define BUFFER_SIZE 4096

    FILE *fpR, *fpW;  
    char buffer[BUFFER_SIZE];  
    int bytes_read, bytes_write;  
	 char *ptr=buffer;
    if ((fpR = fopen(sourceFileNameWithPath, "rb")) == NULL)  
    {  
        log_error("qizc:The file '%s' can not be opened! \n", sourceFileNameWithPath);  
        return 0;  
    }  
    if ((fpW = fopen(targetFileNameWithPath, "wb")) == NULL)  
    {  
        log_error("qizc:The file '%s' can not be opened! \n", targetFileNameWithPath);  
        fclose(fpR);  
        return 0;  
    }  
    memset(buffer, 0, BUFFER_SIZE);  
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fpR)) > 0)  
    {  
       while((bytes_write =fwrite(ptr, 1, bytes_read, fpW))>0)
	   {
			 /* 写完了所有读的字节 */
			 if(bytes_write==bytes_read) 
				 break;
			 /* 只写了一部分,继续写 */
			 else if(bytes_write>0)
			 {
			   ptr+=bytes_write;
			   bytes_read-=bytes_write;
			 }
       } 
        memset(buffer, 0, BUFFER_SIZE);  
    }  

    fclose(fpR);  
    fclose(fpW);  
    return 1;  
}
#endif

std::string& replace_all_distinct(std::string& str,const std::string& old_value,const std::string& new_value) 
{ 
	for(std::string::size_type pos(0); pos!=std::string::npos; pos+=new_value.length()) 
	{ 
		if( (pos=str.find(old_value,pos))!=std::string::npos ) 
			str.replace(pos,old_value.length(),new_value); 
		else
			break; 
	} 

	return str; 
}