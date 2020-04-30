#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <stdint.h>

typedef unsigned char byte;


#ifdef WIN32
#define __attribute(x) 
#endif

#pragma pack(1)

namespace msg_type{
	struct head {
		unsigned short type;
		unsigned short len;
	}__attribute((packed)) ;


	struct db_pkg {
		struct head;
		char * payload;
	}__attribute((packed)) ;


}


#pragma pack()

#endif
