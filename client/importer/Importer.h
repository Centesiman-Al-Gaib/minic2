#ifndef _IMPORTER_
#define _IMPORTER_

/* Perform IAT ofuscation*/


#include <Windows.h>
#include <stdio.h>

HMODULE getModuleHandleCen(LPCSTR moduleNam);
PVOID getProcAddrCen(LPCSTR moduleName, DWORD hash);

#endif