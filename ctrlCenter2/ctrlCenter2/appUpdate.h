#ifndef   APPUPDATE_H
#define  APPUPDATE_H


#ifdef CFG_MDL_UPGRADE_SVR

void  AppUpdate_StartListen();
void  AppUpdate_SetTransBuf(char *p, unsigned int  len);
void  AppUpdate_SetVersion(unsigned short w);
void  AppUpdate_SetVfileN( unsigned short w);	
void  AppUpdate_SetAllowedN(int w);

#endif

#endif