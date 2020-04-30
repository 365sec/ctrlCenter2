#include "lib_function.h"


#ifndef WIN32
#include <unistd.h>
inline void Sleep(int n){
	usleep(n * 1000);
}

#endif


using namespace  std;
typedef unsigned char byte;
string addr2string(unsigned int addr){
	byte * b = (byte *)&addr;
	char buffer[32];
	sprintf(buffer, "%d.%d.%d.%d", b[3], b[2], b[1], b[0]);
	return buffer;
}


void system_result(const char *command, string& s)
{
	FILE * read_fp;
#ifdef WIN32
	read_fp = _popen(command, "r");
#else
	read_fp = popen(command, "r");
#endif
	if(read_fp)
	{
		while(true){
			char buffer[8192] = {0};
			int chars_read = fread(buffer, 1, sizeof(buffer), read_fp);
			if(chars_read <= 0)break;
			s.append(buffer, chars_read);
		}
	}
#ifdef WIN32
	_pclose(read_fp);
#else
	pclose(read_fp);
#endif
}

