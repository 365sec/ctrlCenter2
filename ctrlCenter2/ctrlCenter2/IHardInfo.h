#ifndef I_HARD_INFO
#define I_HARD_INFO

#include "customtypedef.h"

#ifdef OS_LINUX

    #define HARDINFO_API
    #define CALL_METHOD
#else
    #ifdef HARDINFO_EXPORTS
    #define HARDINFO_API __declspec(dllexport)
    #else
    #define HARDINFO_API __declspec(dllimport)
    #endif

    #include <Windows.h>

    #define CALL_METHOD __stdcall
#endif

#define  ADAPTER_NAME_LENGTH    256
#define  ADAPTER_DESCRIPTION_LENGTH  128
#define  ADAPTER_ADDRESS_LENGTH      100
#define  IP_ADDRESS_LENGTH      5 * 5


#define  DRIVERBUFFER_LENGTH     512
#define  LENGTH                  1024

#define HARDDISK_LENGTH     1024

#define SEQUENCE_LENGTH   1024

#define		SystemTimeInformation      3

typedef struct _BIOSINFO{
    char name[LENGTH];//名称
    char parameterValue[LENGTH];
    char brand[LENGTH];//主板品牌
    char factoryNumber[LENGTH];//出厂编号
    char model[LENGTH];//型号
    char time[LENGTH];
    char softSetting[LENGTH];
}BiosInfo;

typedef struct  _ACCOUNTRIGHTS
{
    char strAccount[1024];
    int  arrRights[32];
    bool valid;
    _ACCOUNTRIGHTS *pNext;
    _ACCOUNTRIGHTS()
    {
        ZeroMemory(strAccount, sizeof(_ACCOUNTRIGHTS));
    }
}AccountRights, *PACCOUNTRIGHTS;

typedef struct _CPUINFO{
    char name[LENGTH];//名称
    char parameterValue[LENGTH];
    char brand[LENGTH];//主板品牌
    char factoryNumber[LENGTH];//出厂编号
    char model[LENGTH];//型号
    char time[LENGTH];
    char softSetting[LENGTH];
    DWORD   dwNumberOfProcessors;   // 核数
    DWORD   dwNumberOfCpu;          // CPU数
}CpuInfo, _PCPUINFO;

#define HARDDISK_LENGTH     1024
typedef struct _HARDDISKINFO{
    char name[LENGTH];//名称
    //char parameterValue[LENGTH];
    char brand[LENGTH];//主板品牌
    char factoryNumber[LENGTH];//出厂编号
    char model[LENGTH];//型号
    char time[LENGTH];
    char softSetting[LENGTH];

    char size[HARDDISK_LENGTH];
    UINT64 captacity;

}HardDiskInfo, _PHARDDISKINFO;

typedef struct _DISPLAYINFO{
    char name[LENGTH];//名称
    //char parameterValue[LENGTH];
    char brand[LENGTH];//主板品牌
    char factoryNumber[LENGTH];//出厂编号
    char model[LENGTH];//型号
    char time[LENGTH];
    char softSetting[LENGTH];
    UINT64 captacity;
}DisplayInfo, _PDISPLAYINFO;

typedef struct _MAINBOARDINFO{
    char name[LENGTH];//主板名称
    char brand[LENGTH];//主板品牌
    char factoryNumber[LENGTH];//出厂编号
    char model[LENGTH];//型号
    char parameterValue[LENGTH];
    char time[LENGTH];
    char softSetting[LENGTH];
}MainBoardInfo, _PMAINBOARDINFO;

typedef struct tagWin32PhysicalMemory
{
    //Data type: string
    char Manufacturer[LENGTH];  // 制造商

    //Data type: string
    //Physically-labeled bank where the memory is located. This property is inherited from CIM_PhysicalMemory.
    //Examples: "Bank 0", "Bank A"
    char BankLabel[LENGTH];

    //Data type: uint64, string， 调试发现是string
    //Total capacity of the physical memory—in bytes. This property is inherited from CIM_PhysicalMemory.
    //For more information about using uint64 values in scripts, see Scripting in WMI.
    char Capacity[LENGTH];

    //Data type: string
    //Short description of the object—a one-line string. This property is inherited from CIM_ManagedSystemElement.
    char Caption[LENGTH];

    //Data type: string
    //Qualifiers: Key, MaxLen(256)
    //Name of the first concrete class that appears in the inheritance chain used in the creation of an instance. When used with the other key properties of the class, the property allows all instances of this class and its subclasses to be identified uniquely. This property is inherited from CIM_PhysicalElement.
    char CreationClassName[LENGTH];

    //Data type: uint16
    //Data width of the physical memory—in bits. A data width of 0 (zero) and a total width of 8 (eight) indicates that the memory is used solely to provide error correction bits. This property is inherited from CIM_PhysicalMemory.
    UINT16 DataWidth;

    //Data type: string
    //Description of an object. This property is inherited from CIM_ManagedSystemElement.
    char Description[LENGTH];

    //Data type: string
    //Label of the socket or circuit board that holds the memory.
    //Example: "SIMM 3"
    char DeviceLocator[LENGTH];

    //Data type: uint16
    //Implementation form factor for the chip. This property is inherited from CIM_Chip.
    UINT16 FormFactor;
}Win32PhysicalMemory;

typedef struct _MEMORYINFO{
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;

    Win32PhysicalMemory physicalMemory[10];
    DWORD   validMemoryNum;

}MemoryInfo, _PMEMORYINFO;

typedef struct _AUDIOINFO{
    char name[LENGTH];//名称
    char parameterValue[LENGTH];
    char brand[LENGTH];//主板品牌
    char factoryNumber[LENGTH];//出厂编号
    char model[LENGTH];//型号
    char time[LENGTH];
    char softSetting[LENGTH];
}AudioInfo, _PAUDIOINFO;

typedef struct _NETWORKCARDINFO{
    char name[LENGTH];
    char brand[LENGTH];//主板品牌
    char factoryNumber[LENGTH];//出厂编号
    char model[LENGTH];//型号
    char parameterValue[LENGTH];
    char time[LENGTH];
    char softSetting[LENGTH];
    char MACAddress[LENGTH];         //mac地址
    char IPAddress[LENGTH];               //IP地址
    char IPMask[LENGTH];                  //IP掩码
    char Gateway[LENGTH];                 //网关
}NetworkCardInfo, *PNETWORKCARDINFO;


#define VIRTUAL_METHOD   virtual
#define PURE_VIRTUAL    =0

class IHardware
{
public:
    VIRTUAL_METHOD BOOL CALL_METHOD Init()                                                      PURE_VIRTUAL;
    VIRTUAL_METHOD VOID CALL_METHOD UnInit()                                                    PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL CALL_METHOD GetWMIInfo()                                                PURE_VIRTUAL;

    VIRTUAL_METHOD BOOL CALL_METHOD GetBoardInfo(MainBoardInfo &pBoisInfo)                      PURE_VIRTUAL;
    VIRTUAL_METHOD int  CALL_METHOD GetNetworkCardInfo(NetworkCardInfo *pNetwork, int iSize)    PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL CALL_METHOD GetCPUInfo(CpuInfo &pCpuInfo)                               PURE_VIRTUAL;
    VIRTUAL_METHOD int  CALL_METHOD GetHardDiskInfo(HardDiskInfo *pHardDisk, int iSize)         PURE_VIRTUAL;

    VIRTUAL_METHOD BOOL CALL_METHOD GetDisplayInfo(DisplayInfo &pDisplayInfo)                   PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL CALL_METHOD GetMemoryInfo(MemoryInfo &memory)                           PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL CALL_METHOD GetAudioInfo(AudioInfo &pAudioInfo)                         PURE_VIRTUAL;
    VIRTUAL_METHOD BOOL CALL_METHOD GetBiosInfo(BiosInfo &pBiosInfo)                            PURE_VIRTUAL;

    VIRTUAL_METHOD int  CALL_METHOD LastErr()                                                   PURE_VIRTUAL;
};


#define HARD_EXPORT_IMPORT  HARDINFO_API

#ifdef __cplusplus
extern "C" {
#endif

    HARD_EXPORT_IMPORT  IHardware*  CALL_METHOD GetHardware();
    HARD_EXPORT_IMPORT  VOID        CALL_METHOD ReleaseHardware(IHardware **ppHardware);

#ifdef __cplusplus
}
#endif

#endif
