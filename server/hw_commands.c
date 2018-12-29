#include "hw_commands.h"
#include "server.h"
#include <stdio.h>

BOOLEAN _CmdMsgNameIsValidChar(char Msg)
{
    return (Msg >= 'a' && Msg <= 'z')
        || (Msg >= 'A' && Msg <= 'Z')
        || (Msg >= '0' && Msg <= '9')
        || (Msg == ' ');
}

BOOLEAN _CmdMsgNameIsValid(char* Msg)
{
    int len = 0;

    if ((NULL == Msg) || ('\0' == *Msg))
    {
        return FALSE;
    }

    while (*Msg)//isn't null terminated char
    {
        len++;
        if (!_CmdMsgNameIsValidChar(*Msg))
        {
            return FALSE;
        }
        Msg++;
    }

    if (len > MSG_NAME_MAX_LENGTH)
    {
        return FALSE;
    }

    return TRUE;
}

HANDLE _OpenFile(
    CHAR*   Parameter, 
    BOOLEAN IsNewFile, 
    char *  Output,
    int *   OutLength, 
    int     UserId
)
{
    char filePath[PATH_MAX_LENGTH] = { 0 };
    int retCode = 0;
    HANDLE msgFileHandle = INVALID_HANDLE_VALUE;

    __try
    {
        if (!_CmdMsgNameIsValid(Parameter))
        {
            Log("Message %s is not valid", Parameter);
            sprintf_s(Output, DEFAULT_BUFLEN, "Invalid arguments");
            printf("Returning: %s\n", Output);
            *OutLength = strlen(Output);
            __leave;
        }

        retCode = sprintf_s(filePath, PATH_MAX_LENGTH, ".\\%d\\%s.txt", UserId, Parameter);
        if (!(retCode > 0 && retCode <= PATH_MAX_LENGTH))
        {
            Log("sprintf_s failed with: %d\n", GetLastError());
            sprintf_s(Output, DEFAULT_BUFLEN, "Internal error");
            printf("Returning: %s\n", Output);
            *OutLength = strlen(Output);
            __leave;
        }

        msgFileHandle = CreateFileA(
            filePath,
            GENERIC_WRITE,
            0,
            NULL,
            IsNewFile ? CREATE_NEW : OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (INVALID_HANDLE_VALUE == msgFileHandle)
        {
            Log("CreateFileA failed with: %d\n", GetLastError());
            sprintf_s(Output, DEFAULT_BUFLEN, "Message creation has been failed");
            printf("Returning: %s\n", Output);
            *OutLength = strlen(Output);
            __leave;
        }
    }
    __finally
    {

    }

    return msgFileHandle;
}

//
// Returns TRUE if all end with success or there was an error but we already complete given buffer, and FALSE if parameters are not valid
//
BOOLEAN CmdHandleCreateMsg(
    char *  Parameter, 
    char *  Output, 
    int *   OutLength, 
    int *   UserId
)
{
    HANDLE msgFileHandle = INVALID_HANDLE_VALUE;

    if ((NULL == Output) || (NULL == OutLength) || (NULL == UserId))
    {
        return FALSE;
    }

    msgFileHandle = _OpenFile(Parameter, TRUE, Output, OutLength, *UserId);
    if (INVALID_HANDLE_VALUE != msgFileHandle)
    {
        CloseHandle(msgFileHandle);
        sprintf_s(Output, DEFAULT_BUFLEN, "The message was created");
        printf("Returning: %s\n", Output);
        *OutLength = strlen(Output);
        msgFileHandle = INVALID_HANDLE_VALUE;
    }

    return TRUE;
}
