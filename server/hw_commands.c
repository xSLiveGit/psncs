#include "hw_commands.h"
#include "server.h"
#include <stdio.h>

static void SetReply(
    char *Output,
    int *OutLength,
    char *Message
)
{
    sprintf_s(Output, DEFAULT_BUFLEN, Message);
    printf("Returning: %s\n", Output);
    *OutLength = strlen(Output);
}

static BOOLEAN _CmdMsgNameIsValidChar(char Msg)
{
    return (Msg >= 'a' && Msg <= 'z')
        || (Msg >= 'A' && Msg <= 'Z')
        || (Msg >= '0' && Msg <= '9')
        || (Msg == ' ');
}

static BOOLEAN _CmdMsgNameIsValid(char* Msg)
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

static HANDLE _OpenFile(
    CHAR*   Parameter, 
    BOOLEAN IsNewFile,
    BOOLEAN RequestWriteRights,
    BOOLEAN IsEnryptMode,
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
            RequestWriteRights ? GENERIC_WRITE : GENERIC_READ,
            IsEnryptMode ? FILE_SHARE_WRITE | FILE_SHARE_READ : 0,
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

static BOOLEAN _GetLastMessageName(
    int     UserId,
    char *  Output,
    int *   OutLength,
    char*   LastFile,
    DWORD*    LastFileSize
)
{
    int retCode = 0;
    char filePath[PATH_MAX_LENGTH + CONTROL_FILE_SUFIX_SIZE] = { 0 };
    HANDLE controlFileHandle = INVALID_HANDLE_VALUE;
    BOOLEAN success = FALSE;

    __try
    {
        //
        // Format control path
        //
        retCode = sprintf_s(filePath, PATH_MAX_LENGTH, ".\\%d\\%s", UserId, CONTROL_FILE_SUFIX);
        if (retCode == -1)
        {
            Log("Internal error");
            SetReply(Output, OutLength, "[ERROR] The message wasn't created");
            __leave;
        }

        //
        // Open file
        //
        controlFileHandle = CreateFileA(
            filePath,
            GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (INVALID_HANDLE_VALUE == controlFileHandle)
        {
            Log("CreateFileA failed with gle: 0x%08x", GetLastError());
            SetReply(Output, OutLength, "[ERROR] The message wasn't created");
            __leave;
        }

        //
        // Read last file message name
        //
        if (!ReadFile(
            controlFileHandle,
            LastFile,
            PATH_MAX_LENGTH,
            LastFileSize,
            NULL
        ))
        {
            Log("ReadFile failed with: 0x%08x", GetLastError());
            SetReply(Output, OutLength, "[ERROR] Internal error");
            __leave;
        }

        success = TRUE;
    }
    __finally
    {
        if (controlFileHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(controlFileHandle);
            controlFileHandle = INVALID_HANDLE_VALUE;
        }
    }
    
    return success;
}

//
// Returns TRUE if all end with success or if there was an error but we already complete given buffer, and FALSE if parameters are not valid
//
BOOLEAN CmdHandleCreateMsg(
    char *  Parameter, 
    char *  Output, 
    int *   OutLength, 
    int *   UserId
)
{
    HANDLE msgFileHandle = INVALID_HANDLE_VALUE;
    HANDLE fileControlHandle = INVALID_HANDLE_VALUE;
    char controlDirectoryPath[PATH_MAX_LENGTH + CONTROL_DIRECTORY_NAME_SIZE] = { 0 };
    char controlFilePath[PATH_MAX_LENGTH + CONTROL_FILE_SUFIX_SIZE] = { 0 };
    int retCode = 0;
    DWORD bytesWritten = 0;

    if ((NULL == Output) || (NULL == OutLength) || (NULL == UserId))
    {
        return FALSE;
    }

    __try
    {
        //
        // Format control file path
        //
        retCode = sprintf_s(
            controlFilePath,
            PATH_MAX_LENGTH + CONTROL_FILE_SUFIX_SIZE, 
            ".\\%d\\%s", 
            *UserId, 
            CONTROL_FILE_SUFIX
        );
        if (retCode == -1)
        {
            Log("Internal error for sprintf_s");
            SetReply(Output, OutLength, "[ERROR] Internal error");
            __leave;
        }


        //
        // Open control path
        //
        fileControlHandle = CreateFileA(
            controlFilePath,
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (INVALID_HANDLE_VALUE == fileControlHandle)
        {
            //
            // Format control directory path
            //
            retCode = sprintf_s(
                controlDirectoryPath, 
                PATH_MAX_LENGTH + CONTROL_DIRECTORY_NAME_SIZE, 
                ".\\%d\\%s", 
                *UserId, 
                CONTROL_DIRECTORY_NAME
            );
            if (retCode == -1)
            {
                Log("Internal error");
                SetReply(Output, OutLength, "[ERROR] Internal error");
                __leave;
            }


            //
            //Create control directory if it doesn't exists
            //
            CreateDirectoryA(controlDirectoryPath, NULL);

            fileControlHandle = CreateFileA(
                controlFilePath,
                GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            if (INVALID_HANDLE_VALUE == fileControlHandle)
            {
                Log("CreateFileA failed with error: 0x%08x", GetLastError());
                SetReply(Output, OutLength, "[ERROR] Internal error");
                __leave;
            }
        }


        //
        // Create message file
        //
        msgFileHandle = _OpenFile(Parameter, TRUE, TRUE, FALSE, Output, OutLength, *UserId);
        if (INVALID_HANDLE_VALUE == msgFileHandle)
        {
            __leave;
        }//error msg was already completed in _OpenFile


        //
        // Write message name in control file
        // 
        if (!WriteFile(
            fileControlHandle, 
            Parameter, 
            strlen(Parameter), 
            &bytesWritten, 
            NULL
        ))
        {
            char filePath[PATH_MAX_LENGTH] = { 0 };
            sprintf_s(filePath, PATH_MAX_LENGTH, ".\\%d\\%s.txt", *UserId, Parameter);
            if (!DeleteFileA(filePath))
            {
                Log("[ERROR] DeleteFileA failed with gle: 0x%08X", GetLastError());
            }
            SetReply(Output, OutLength, "[ERRO] The message was created");
            __leave;
        }


        //
        // Success
        //
        SetReply(Output, OutLength, "[OK] The message was created");
    }
    __finally
    {
        if (INVALID_HANDLE_VALUE != fileControlHandle)
        {
            CloseHandle(fileControlHandle);
            fileControlHandle = INVALID_HANDLE_VALUE;
        }

        if (INVALID_HANDLE_VALUE != msgFileHandle)
        {
            CloseHandle(msgFileHandle);
            msgFileHandle = INVALID_HANDLE_VALUE;
        }
    }
 
    return TRUE;
}


//
// Returns TRUE if all end with success or if there was an error but we already complete given buffer, and FALSE if parameters are not valid
//
BOOLEAN CmdHandleWriteMessage(
    char *  Parameter,
    DWORD   ParameterSize,
    char *  Output,
    int *   OutLength, 
    int *   UserId
)
{
    HANDLE msgFileHandle = INVALID_HANDLE_VALUE;
    CHAR lastFileName[PATH_MAX_LENGTH] = { 0 };
    DWORD lastFileNameSize = 0;
    DWORD numberOfBytesWritten = 0;
    CHAR* message = NULL;

    if ((NULL == Output) || (NULL == OutLength) || (NULL == UserId))
    {
        return FALSE;
    }

    __try
    {
        if (NULL == Parameter || *Parameter == 0)
        {
            SetReply(Output, OutLength, "[ERROR] Invalid message content");
            __leave;
        }

        if (!_GetLastMessageName(
            *UserId,
            Output,
            OutLength,
            lastFileName,
            &lastFileNameSize
        ))
        {
            __leave;
        }

        msgFileHandle = _OpenFile(lastFileName, FALSE, TRUE, FALSE, Output, OutLength, *UserId);
        if (INVALID_HANDLE_VALUE == msgFileHandle)
        {
            __leave;
        }
        SetFilePointer(msgFileHandle, 0, NULL, FILE_END);

        message = (CHAR*)malloc(ParameterSize + 2);//!1 for new line and 1 for \0
        if (NULL == message)
        {
            Log("malloc failed");
            SetReply(Output, OutLength, "[ERROR] Internal error during message writing");
            __leave;
        }

        memcpy(message, Parameter, ParameterSize);
        message[ParameterSize] = '\n';
        message[ParameterSize + 1] = '\0';

        if (!WriteFile(
            msgFileHandle,
            message,
            ParameterSize + 1,
            &numberOfBytesWritten,
            NULL
        )) 
        {
            Log("WriteFile failed with: 0x%08X", GetLastError());
            SetReply(Output, OutLength, "[ERROR] Internal error during message writing");
            __leave;
        }

        SetReply(Output, OutLength, "[OK] Message was written successfully");
    }
    __finally
    {
        if (INVALID_HANDLE_VALUE != msgFileHandle)
        {
            CloseHandle(msgFileHandle);
            msgFileHandle = INVALID_HANDLE_VALUE;
        }

        if (NULL != message)
        {
            free(message);
            message = NULL;
        }
    }
    
    return TRUE;
}

static void _InitEncryptionStruct(PENCRYPTION_STRUCT EncryptionStruct)
{
    EncryptionStruct->BytesToEncrypt = 0;
    EncryptionStruct->FileHandle = INVALID_HANDLE_VALUE;
    EncryptionStruct->SharedCounter = NULL;
    EncryptionStruct->ThreadId = (DWORD)-1;
    EncryptionStruct->ShouldNotExit = NULL;
    EncryptionStruct->EncryptionByte = 0;
}

static BOOLEAN _FillEncryptionStruct(
    PENCRYPTION_STRUCT EncryptionStruct, 
    char * Parameter, 
    char * Output, 
    int * OutLength,
    volatile LONGLONG  * Counter,
    DWORD BytesToEncrypt,
    DWORD ThreadId,
    int UserId,
    volatile BOOLEAN* ShouldNotExit
    )
{
    EncryptionStruct->FileHandle = _OpenFile(Parameter, FALSE, TRUE, TRUE, Output, OutLength, UserId);
    if (INVALID_HANDLE_VALUE == EncryptionStruct->FileHandle)
    {
        return FALSE;
    }

    EncryptionStruct->BytesToEncrypt = BytesToEncrypt;
    EncryptionStruct->SharedCounter = Counter;
    EncryptionStruct->ThreadId = ThreadId;
    EncryptionStruct->ShouldNotExit = ShouldNotExit;
    EncryptionStruct->EncryptionByte = (BYTE)UserId+1;

    return TRUE;
}

static void _CleanUpEncryptionStruct(PENCRYPTION_STRUCT EncryptionStruct)
{
    CloseHandle(EncryptionStruct->FileHandle);
    _InitEncryptionStruct(EncryptionStruct);
}

static void _FreeEncryptionStruct(PENCRYPTION_STRUCT* EncryptionStruct)
{
    _CleanUpEncryptionStruct(*EncryptionStruct);
    free(*EncryptionStruct);
    *EncryptionStruct = NULL;
}

void _EncryptXor(
    CHAR* Buffer,
    DWORD BufferSize,
    CHAR  MagicByte
    )
{
    for (DWORD i = 0; i < BufferSize; i++)
    {
        Buffer[i] = Buffer[i] ^ MagicByte;
    }
}

static DWORD
WINAPI EncryptionThread(
    LPVOID lpThreadParameter
)
{
    PENCRYPTION_STRUCT encryptionStruct = (PENCRYPTION_STRUCT)lpThreadParameter;
    DWORD bytesToEncrypt = encryptionStruct->BytesToEncrypt;
    DWORD maxBytesToEncrypt = 64;
    CHAR encryptionBuffer[64] = { 0 };
    DWORD readBytes = 0;
    DWORD currentEncryptionBytes = 0;

    __try
    {
        if (! encryptionStruct->ShouldNotExit)
        {
            __leave;
        }

        while (bytesToEncrypt != 0)
        {
            currentEncryptionBytes = min(bytesToEncrypt, maxBytesToEncrypt);

            //
            // Read max 64 bytes
            //
            if (!ReadFile(encryptionStruct->FileHandle, encryptionBuffer, currentEncryptionBytes, &readBytes, NULL))
            {
                Log("Read file failed during encryption with status: 0x%08X", GetLastError());
                __leave;
            }

            //
            // Move cursor back
            //
            if (INVALID_SET_FILE_POINTER == SetFilePointer(encryptionStruct->FileHandle, currentEncryptionBytes, NULL, FILE_CURRENT))
            {
                Log("SetFilePointer returned INVALID_SET_FILE_POINTER");
                __leave;
            }

            //
            //Encrypt message
            //
            _EncryptXor(encryptionBuffer, readBytes, encryptionStruct->EncryptionByte);
            
            //
            // Write message => it will move file cursor
            //
            if (!WriteFile(encryptionStruct->FileHandle, encryptionBuffer, readBytes, NULL, NULL))
            {
                Log("WriteFile failed during ecnryption gle: 0x%08X", GetLastError());
                __leave;
            }

            bytesToEncrypt -= readBytes;
        }
    }
    __finally
    {
        _FreeEncryptionStruct(&encryptionStruct);
    }

    return 0;
}

BOOLEAN CmdHandleEncryptMessage(
    char * Parameter, 
    char * Output, 
    int * OutLength, 
    int * UserId
)
{
    HANDLE msgFileHandle = INVALID_HANDLE_VALUE;
    HANDLE hThreads[2] = { NULL };
    volatile BOOLEAN allGood = FALSE;
    PENCRYPTION_STRUCT workerParam = NULL;
    volatile LONGLONG counter = 0;
    int noThreads = 2;
    int i = 0;
    DWORD fileSize = 0;
    DWORD bytesToEncryptForCurrentWorker = 0;
    DWORD fileOffset = 0;
    SECURITY_ATTRIBUTES securityAttributes = { 0 };
    securityAttributes.bInheritHandle = TRUE;
    securityAttributes.nLength = sizeof(securityAttributes);

    if ((NULL == Output) || (NULL == OutLength) || (NULL == UserId))
    {
        return FALSE;
    }
    *OutLength = 0;

    __try
    {
        msgFileHandle = _OpenFile(Parameter, FALSE, TRUE, TRUE, Output, OutLength, *UserId);
        if (INVALID_HANDLE_VALUE == msgFileHandle)
        {
            __leave;
        }

        fileSize = GetFileSize(msgFileHandle, NULL);
        if (INVALID_FILE_SIZE == fileSize)
        {
            Log("GetFileSize failed with gle: 0x%08X", GetLastError());
            __leave;
        }

        CloseHandle(msgFileHandle);
        msgFileHandle = INVALID_HANDLE_VALUE;

        for (i = 0; i < noThreads; i++)
        {
            //
            // Create and init thread param
            //
            workerParam = (PENCRYPTION_STRUCT)malloc(sizeof(ENCRYPTION_STRUCT));
            if (NULL == workerParam)
            {
                Log("malloc failed for ENCRYPTION_STRUCT");
                __leave;
            }

            bytesToEncryptForCurrentWorker = fileSize / (noThreads - i);

            _InitEncryptionStruct(workerParam);
            if (!_FillEncryptionStruct(
                workerParam, 
                Parameter, 
                Output, 
                OutLength, 
                &counter, 
                bytesToEncryptForCurrentWorker, 
                i, 
                *UserId, 
                &allGood
            ))
            {
                __leave;
            }

            //
            //Set right handle offset
            //
            SetFilePointer(workerParam->FileHandle, fileOffset, NULL, FILE_BEGIN);
            fileOffset += bytesToEncryptForCurrentWorker;
            
            //
            // Create thread
            // 
            hThreads[i] = CreateThread(
                &securityAttributes,
                0,
                &EncryptionThread,
                workerParam,
                CREATE_SUSPENDED,
                NULL
            );
            if (hThreads[i] == NULL)
            {
                Log("CreateThread failed with 0x%08X", GetLastError());
                __leave;
            }

            workerParam = NULL;//change ownership
        }

        allGood = TRUE;
    }
    __finally
    {
        if (NULL != workerParam)
        {
            _FreeEncryptionStruct(&workerParam);
        }

        //
        // Resume all threads. They was created suspended
        // 
        for (i = 0; i < noThreads; i++)
        {
            if (NULL != hThreads[i])
            {
                if (-1 == ResumeThread(hThreads[i]))
                {
                    Log("ResumeThread failed with: 0x%08X", GetLastError());
                }
            }
        }

        //
        // Wait for all threads
        //
        for (i = 0; i < noThreads; i++)
        {
            if (WAIT_FAILED == WaitForSingleObject(hThreads[i], INFINITE))//ignore other statuses..
            {
                Log("WaitForSingleObject failed with 0x%08X", GetLastError());
            }
        }
    }

    if (allGood)
    {
        SetReply(Output, OutLength, "[OK] Encryption end successfully");
    }
    else if(OutLength != 0)
    {
        SetReply(Output, OutLength, "[ERROR] Internal error");
    }

    return TRUE;
}
