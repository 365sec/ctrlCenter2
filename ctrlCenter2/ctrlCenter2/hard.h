#ifndef GRXA_HARD_H
#define GRXA_HARD_H

#include "IHardInfo.h"

// ¹ıÂËµôĞéÄâÍø¿¨
int FilterNetCards(NetworkCardInfo *pCards, int cardNum, NetworkCardInfo *pSortCards, int sortCardNum);
void buildPwd(char szPwd[128], int pwdLen);
unsigned int hashFun(char *str);

#endif  //GRXA_HARD_H
