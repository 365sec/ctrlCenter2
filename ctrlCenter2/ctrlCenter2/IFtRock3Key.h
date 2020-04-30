#ifndef I_FTROCK3_KEY
#define I_FTROCK3_KEY

#include "customtypedef.h"

#ifdef OS_LINUX
    #define FTROCK3KEY_API
    #define CALL_METHOD
#else
    #include <Windows.h>

    #ifdef FTROCK3KEY_EXPORTS
    #define FTROCK3KEY_API __declspec(dllexport)
    #else
    #define FTROCK3KEY_API __declspec(dllimport)
    #endif

    #define CALL_METHOD __stdcall
#endif // OS_LINUX


#define ROCK5_KEY_MEM_LEN       8192
#define ROCK5_KEY_MAX_BUF_LEN   (ROCK5_KEY_MEM_LEN+16)

#define VIRTUAL_METHOD   virtual
#define PURE_VIRTUAL    =0

class IFtRock3Key {

public:
    VIRTUAL_METHOD BOOL    CALL_METHOD Init(CHAR szVendorID[16], CHAR szVendorPin[64])  PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD UnInit()                                         PURE_VIRTUAL;
    VIRTUAL_METHOD VOID    CALL_METHOD SetVendorID(CHAR szVendorID[16])                 PURE_VIRTUAL;
    VIRTUAL_METHOD VOID    CALL_METHOD SetVendorPin(CHAR szVendorPin[36])               PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD VerifyDevPin()                                   PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD GetDevices(DWORD& dwDeviceNum)                   PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD GetCurDevice(DWORD& dwCurDevice)                 PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD SetDevice(DWORD dwDevice)                        PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD OpenDevice()                                     PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD CheckValid()                                     PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD CloseDevice()                                    PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD GetHardID(CHAR szHardID[128] = NULL)             PURE_VIRTUAL;

    // Encrypt,Decrypt
    VIRTUAL_METHOD BOOL     CALL_METHOD SetEncryptPwd(char *szEncryptPwd)               PURE_VIRTUAL;
// #define NO_ERRORS_DONE 0 //No errors.
// #define ERROR_SRC_FILE 1 //Can't open the source file.
// #define ERROR_DST_FILE 2 //Can't create the new file.
// #define ERROR_CANT_ENC 3 //Can't encrypt the file (must be normal file).
// #define ERROR_CANT_DEC 4 //Can't decrypt the file (must be encrypted file).
// #define ENCRYPTED_FILE 5 //The file is an encrypted file.
// #define NORMAL_FILE 6 //The file is a normal file.
// #define READ_WRITE_ERROR 7 // error in encryption/decryption process
// #define ALLOC_ERROR 8 //error allocating memory to buffers
// #define BAD_SIGNATURE 9 //user signature doesn't match
    VIRTUAL_METHOD BOOL      CALL_METHOD Encrypt(char *plaintext, int &len)                              PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL      CALL_METHOD Decrypt(char *ciphertext, int &len)                              PURE_VIRTUAL;

    VIRTUAL_METHOD VOID    CALL_METHOD SetReadLen(DWORD dwReadLen)                                                                      PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD ReadKeyBuf(CHAR* pReadBuf, DWORD dwBufLen, DWORD dwOffset=0)                                     PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD ReadFile(CHAR szFile[], CHAR szReadBuf[], DWORD dwReadLen, DWORD &dwRealReadLen, BOOL bEncrypt)  PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD WriteFile(CHAR szFileName[], CHAR szBuf[], DWORD dwBufLen, BOOL bEncrypt = FALSE)                PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL    CALL_METHOD WriteKeyBuf(VOID* pWriteBuf, DWORD BufLen, DWORD dwOffset=0)                                     PURE_VIRTUAL;

    VIRTUAL_METHOD DWORD   CALL_METHOD GetLastError()                                                                                   PURE_VIRTUAL;
};

#define ROCK3_EXPORT_IMPORT  FTROCK3KEY_API
#ifdef __cplusplus
extern "C" {
#endif

    ROCK3_EXPORT_IMPORT  IFtRock3Key*   CALL_METHOD GetRock3Key(CHAR szVendorID[16], CHAR szVendorPin[64]);
    ROCK3_EXPORT_IMPORT  VOID           CALL_METHOD ReleaseRock3Key(IFtRock3Key** ppRock3);

#ifdef __cplusplus
}
#endif


#endif
