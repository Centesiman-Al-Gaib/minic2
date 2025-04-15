#ifndef _IMPORTER_
#define _IMPORTER_

/* Perform IAT ofuscation*/


#include <Windows.h>
#include <stdio.h>

HMODULE getModuleHandleCen(LPCSTR moduleNam);
FARPROC getProcAddrCen(LPCSTR moduleName, DWORD hash);

#endif