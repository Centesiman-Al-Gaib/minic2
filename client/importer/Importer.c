#include <Windows.h>
#include <winternl.h>
#include "Importer.h"
#include "../crypto/Crypto.h"

LPWSTR lowerWideString (LPCWSTR Str1) {

	int		len1	= lstrlenW(Str1);
    LPWSTR lStr1 = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (len1 + 1) * sizeof(WCHAR));
	int		i		= 0;

	// Checking length. We dont want to overflow the buffers
	if (len1 >= MAX_PATH)
		return FALSE;

    // Converting Str1 to lower case string (lStr1)
	for (i = 0; i < len1; i++){
		lStr1[i] = (WCHAR)tolower(Str1[i]);
	}
	lStr1[i++] = L'\0'; // null terminating

	return lStr1;
}

HMODULE getModuleHandleCen(LPCSTR moduleName)
{

    /* A PARTIR DE AQUÍ BUSCAMOS OBTENER UN HANDLE QUE APUNTE AL MODULO*/
    PPEB pPeb = (PPEB)(__readgsqword(0x60));

    PPEB_LDR_DATA loadedDll = pPeb->Ldr;
    PLIST_ENTRY headNode = (loadedDll->InMemoryOrderModuleList.Flink);
    PLDR_DATA_TABLE_ENTRY headModule = (PLDR_DATA_TABLE_ENTRY)((PBYTE)headNode - (sizeof(LIST_ENTRY)));

    PLIST_ENTRY nextNode = headNode->Flink;
    PLDR_DATA_TABLE_ENTRY nextModule = (PLDR_DATA_TABLE_ENTRY)((PBYTE)nextNode - (sizeof(LIST_ENTRY)));

    while (nextNode != headNode)
    {   
        LPWSTR moduleNameLower = lowerWideString(nextModule->FullDllName.Buffer);
        if (HASH_KERNERL32DLL_ROUTE == hashWide(moduleNameLower) && HeapFree(GetProcessHeap(), 0, (LPVOID)moduleNameLower))
        {
            break;
        }
        
        nextNode = nextNode->Flink;
        nextModule = (PLDR_DATA_TABLE_ENTRY)((PBYTE)nextNode - (sizeof(LIST_ENTRY)));
    }

    if (nextNode == headNode)
    {
        return NULL;
    }

    /* A PARTIR DE AQUÍ ES OBTENER LAS FUNCIONES IMPORTADAS POR EL MODULO */
    PBYTE pHDll = (PBYTE)nextModule->DllBase;

    PIMAGE_DOS_HEADER pImgDosHeader = (PIMAGE_DOS_HEADER)pHDll;
    if (pImgDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        exit(1);
    };

    PIMAGE_NT_HEADERS pImgNtHeader = (PIMAGE_NT_HEADERS)(pHDll + pImgDosHeader->e_lfanew);
    if (pImgNtHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        exit(1);
    };
    
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
        
        if (hashAscii(pFunctionName) == HASH_GETMODULEHANDLE_FUNCTION) 
        {
            pFunction = (PVOID)(pHDll + addressOfFunctions[wFunctionOrdinal]);
            typedef HMODULE(WINAPI* GetModuleCen)(
                LPCSTR moduleName
            );
            GetModuleCen getHandle = (GetModuleCen)pFunction;
            return getHandle(moduleName);
        }
    }

    return NULL;
};

FARPROC getProcAddrCen(LPCSTR moduleName, DWORD hash)
{
    /* DECRYPT MOUDLE NAME */
    DWORD length = frombase64GetLength(moduleName, strlen(moduleName));
    PBYTE blobData = fromBase64GetBlob(moduleName, strlen(moduleName), length);
    HMODULE hDll = getModuleHandleCen((LPCSTR)encrypt((unsigned char*)blobData, length));
    HeapFree(GetProcessHeap(), 0, blobData);
    /* A PARTIR DE AQUÍ ES OBTENER LAS FUNCIONES IMPORTADAS POR EL MODULO */
    PBYTE pHDll = (PBYTE)hDll;
    if (hDll == INVALID_HANDLE_VALUE)
    {
        exit(1);
    }

    PIMAGE_DOS_HEADER pImgDosHeader = (PIMAGE_DOS_HEADER)pHDll;
    if (pImgDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        exit(1);
    };

    PIMAGE_NT_HEADERS pImgNtHeader = (PIMAGE_NT_HEADERS)(pHDll + pImgDosHeader->e_lfanew);
    if (pImgNtHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        exit(1);
    };
    
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
            return (FARPROC)pFunction;
        }
    }

    return NULL;
};