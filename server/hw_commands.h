#ifndef _HW_COMMANDS_H_
#define _HW_COMMANDS_H_

#include <Windows.h>

#define MSG_NAME_MAX_LENGTH 20
#define INT_MAX_LENGTH 21 //20 + 1 for null terminated
#define PATH_MAX_LENGTH (MSG_NAME_MAX_LENGTH + INT_MAX_LENGTH + sizeof(".\\%d\\*.txt"))
#define CONTROL_FILE_SUFIX "_control\\control.txt"
#define CONTROL_DIRECTORY_NAME "_control"
#define CONTROL_DIRECTORY_NAME_SIZE sizeof("_control")
#define CONTROL_FILE_SUFIX_SIZE sizeof(CONTROL_FILE_SUFIX)

typedef struct _ENCRYPTION_STRUCT
{
    DWORD               ThreadId;
    HANDLE              FileHandle;
    DWORD               BytesToEncrypt;
    LONGLONG volatile * SharedCounter;
    volatile BOOLEAN*   ShouldNotExit;
    CHAR                EncryptionByte;
} ENCRYPTION_STRUCT, *PENCRYPTION_STRUCT;

BOOLEAN 
CmdHandleCreateMsg(
    _In_    char    *Parameter,
    _Out_   char    *Output,
    _Out_   int     *OutLength,
    _In_    int*    UserId
);

BOOLEAN 
CmdHandleWriteMessage(
    _In_    char    *Parameter,
    _In_    DWORD   ParameterSize,
    _Out_   char    *Output,
    _Out_   int     *OutLength,
    _In_    int*    UserId
);

BOOLEAN
CmdHandleEncryptMessage(
    _In_    char    *Parameter,
    _Out_   char    *Output,
    _Out_   int     *OutLength,
    _In_    int*    UserId
);

#endif // !_HW_COMMANDS_H_
