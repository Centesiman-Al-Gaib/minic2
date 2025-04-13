#include <Windows.h>
#include "Importer.h"
#include "../crypto/Crypto.h"

HANDLE getModuleHandleCen()
{
    return NULL;
};

FARPROC getProcAddrCen(LPCWSTR moduleName, DWORD hash)
{

    HMODULE hDll = GetModuleHandleW(moduleName);
    /* A PARTIR DE AQUÍ ES OBTENER LAS FUNCIONES IMPORTADAS POR EL MODULO */
    PBYTE pHDll = (PBYTE)hDll;
    if (hDll == INVALID_HANDLE_VALUE)
    {
        printf("[-] Module Not Found\n");
        exit(1);
    }
    printf("[+] Parsing headers...\n");

    PIMAGE_DOS_HEADER pImgDosHeader = (PIMAGE_DOS_HEADER)hDll;
    if (pImgDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        printf("[-] DOS Header is Invalid\n");
        exit(1);
    };

    PIMAGE_NT_HEADERS pImgNtHeader = (PIMAGE_NT_HEADERS)(pHDll + pImgDosHeader->e_lfanew);
    if (pImgNtHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        printf("[-] NT Header is Invalid\n");
        exit(1);
    };
    
    printf("[+] Getting the export table of the module...\n");
    PIMAGE_EXPORT_DIRECTORY pImgExportTable = (PIMAGE_EXPORT_DIRECTORY)(pHDll + pImgNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    PDWORD addressOfFunctions = (PDWORD)(pHDll + pImgExportTable->AddressOfFunctions);
    PDWORD addressOfNames = (PDWORD)(pHDll + pImgExportTable->AddressOfNames);
    PWORD  addressOfNameOrdinals = (PWORD)(pHDll + pImgExportTable->AddressOfNameOrdinals);
    PVOID pFunction;

    for (DWORD i = 0; i < pImgExportTable->NumberOfFunctions; i++)
    {
        // Getting the name of the function
        CHAR* pFunctionName = (CHAR*)(pHDll + addressOfNames[i]);

        // Getting the ordinal of the function
        WORD wFunctionOrdinal = addressOfNameOrdinals[i];
        
        if (hashAscii(pFunctionName) == hash) 
        {
            pFunction = (PVOID)(pHDll + addressOfFunctions[wFunctionOrdinal]);
            //printf("[+] Function Found at -> 0x%p with ordinal %i\n", pFunction,wFunctionOrdinal);
            //printf("[+] Original GetProcAddress : 0x%p \n", GetProcAddress(GetModuleHandleA("C:\\Windows\\System32\\ws2_32.dll"), "connect"));
            return (FARPROC)pFunction;
        }
    }

    return NULL;
};