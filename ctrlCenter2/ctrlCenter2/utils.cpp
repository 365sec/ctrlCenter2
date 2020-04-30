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


//ƥ��һ���ַ��ĺ�
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
//  ͨ���ƥ���㷨��û����ȫ��֤�����С�?����ȫû����֤���硰*.txt����BUG��
//	src���ַ�����pattern������ͨ���(*��?)���ַ�����bCaseSensitive���Ƿ����ִ�Сд
//
bool WildCharMatch_Oneway(const char *src,const char* pattern,const char* patternOriginal,bool bCaseSensitive=CS_NO)
{
	bool result=false;

	while(*src)
	{
		if(*pattern == '*')
		{
			// ��� pattern �ĵ�ǰ�ַ��� '*' 
			// ��������ж�� '*', ����
			while( (*pattern == '*') || (*pattern == '?') )
				pattern++;

			// ��� '*" ��û���ַ��ˣ�����ȷƥ��
			if (!*pattern)
			{
				return true;
			}

			// �� src �в���һ���� pattern��'*"���һ���ַ���ͬ���ַ�
			while(*src && (!MATCH_CHAR(*src,*pattern,bCaseSensitive)))
				src++;

			// ����Ҳ�������ƥ��ʧ��
			if (!*src)
			{
				return false;
			}

			// ����ҵ��ˣ�ƥ��ʣ�µ��ַ���
			result = WildCharMatch_Oneway(src,pattern,patternOriginal,bCaseSensitive);

			// ���ʣ�µ��ַ���ƥ�䲻�ϣ���src��һ���ַ�����pattern��'*"���һ���ַ�
			// srcǰ��һλ������ƥ��
			while( (!result) && (*(src+1)) && MATCH_CHAR(*(src+1),*pattern,bCaseSensitive) )
				result = WildCharMatch_Oneway (++src, pattern,patternOriginal,bCaseSensitive);

			return result;
		}
		else
		{
			// ���pattern�е�ǰ�ַ����� '*' 
			// ƥ�䵱ǰ�ַ�
			if( MATCH_CHAR(*src,*pattern,bCaseSensitive) || ('?'==*pattern) )
			{ 
				// src,pattern�ֱ�ǰ��һλ������ƥ�� 
				return WildCharMatch_Oneway(++src,++pattern,patternOriginal,bCaseSensitive);
			}
			else
			{
				//qizc����-------start

				//
				//	pattern��û�е�β
				//	���������ȴ���pattern��ȥĩβͨ����ĳ���
				//	pattern��*��ͷ
				//
				if( *pattern && strlen(src)>strlen(pattern)-ReverseCountWild(pattern) && *patternOriginal=='*' )
				{
					return WildCharMatch_Oneway(++src,patternOriginal,patternOriginal,bCaseSensitive);
				}
				else

				//qizc����---------end

				return false;
			}
		}
	}


	// ���src�����ˣ���pattern�з����       
	if(*pattern)  
	{ 
		// patternû�н���          
		if ( (*pattern=='*') && (*(pattern+1)==0) ) // ���pattern�����һλ�ַ�����'*' 
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
 
 
   /* ��Դ�ļ� */
   if((from_fd=open(sourceFileNameWithPath,O_RDONLY))==-1)   /*open file readonly,����-1��ʾ�������򷵻��ļ�������*/
   {
     log_error("Open %s Error:%s\n",sourceFileNameWithPath,strerror(errno));
     return 0;
   }
 
   /* ����Ŀ���ļ� */
   /* ʹ����O_CREATѡ��-�����ļ�,open()������Ҫ��3������,
      mode=S_IRUSR|S_IWUSR��ʾS_IRUSR �û����Զ� S_IWUSR �û�����д*/
   if((to_fd=open(targetFileNameWithPath,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1) 
   {
      log_error("Open %s Error:%s\n",targetFileNameWithPath,strerror(errno));
     return 0;
   }
 
   /* ���´�����һ������Ŀ����ļ��Ĵ��� */
   while(bytes_read=read(from_fd,buffer,BUFFER_SIZE))
   {
     /* һ�������Ĵ������� */
     if((bytes_read==-1)&&(errno!=EINTR)) 
         break;
     else if(bytes_read>0)
     {
       ptr=buffer;
       while(bytes_write=write(to_fd,ptr,bytes_read))
       {
         /* һ�������������� */
         if((bytes_write==-1)&&(errno!=EINTR))
             break;
         /* д�������ж����ֽ� */
         else if(bytes_write==bytes_read) 
             break;
         /* ֻд��һ����,����д */
         else if(bytes_write>0)
         {
           ptr+=bytes_write;
           bytes_read-=bytes_write;
         }
       }
       /* д��ʱ�������������� */
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
			 /* д�������ж����ֽ� */
			 if(bytes_write==bytes_read) 
				 break;
			 /* ֻд��һ����,����д */
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