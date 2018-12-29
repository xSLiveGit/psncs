#ifndef _HW_COMMANDS_H_
#define _HW_COMMANDS_H_

#include <Windows.h>

#define MSG_NAME_MAX_LENGTH 20
#define INT_MAX_LENGTH 21 //20 + 1 for null terminated
#define PATH_MAX_LENGTH (MSG_NAME_MAX_LENGTH + INT_MAX_LENGTH + sizeof(".\\%d\\*.txt"))

BOOLEAN CmdHandleCreateMsg(
    _In_    char    *Parameter,
    _Out_   char    *Output,
    _Out_   int     *OutLength,
    _In_    int*    UserId
);

#endif // !_HW_COMMANDS_H_
