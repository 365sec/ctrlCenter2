#ifndef CIPHER_H_
#define CIPHER_H_

#include "customtypedef.h"

#ifdef OS_LINUX
#define CIPHER_API
#else
#ifdef CIPHER_EXPORTS
#define CIPHER_API __declspec(dllexport)
#else
#define CIPHER_API __declspec(dllimport)
#endif
#endif



#define NO_ERRORS_DONE 0 //No errors.
#define ERROR_SRC_FILE 1 //Can't open the source file.
#define ERROR_DST_FILE 2 //Can't create the new file.
#define ERROR_CANT_ENC 3 //Can't encrypt the file (must be normal file).
#define ERROR_CANT_DEC 4 //Can't decrypt the file (must be encrypted file).
#define ENCRYPTED_FILE 5 //The file is an encrypted file.
#define NORMAL_FILE 6 //The file is a normal file.
#define READ_WRITE_ERROR 7 // error in encryption/decryption process
#define ALLOC_ERROR 8 //error allocating memory to buffers
#define BAD_SIGNATURE 9 //user signature doesn't match
extern "C" {

    CIPHER_API  int  CALL_METHOD  DecryptBuf(char *szSrcBuf, int iSrcLen, char*szPwd, char *szDstBuf, int &iDstLen);
    CIPHER_API  int  CALL_METHOD  EncryptBuf(char *szSrcBuf, int iSrcLen, char*szPwd, char *szDstBuf, int &iDstLen);

}


#endif  // CIPHER_H_
