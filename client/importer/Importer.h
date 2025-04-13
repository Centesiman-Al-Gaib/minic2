#ifndef _IMPORTER_
#define _IMPORTER_

/* Perform IAT ofuscation*/


#include <Windows.h>
#include <stdio.h>

HANDLE getModuleHandleCen();
FARPROC getProcAddrCen(LPCWSTR moduleName, DWORD hash);

#endif