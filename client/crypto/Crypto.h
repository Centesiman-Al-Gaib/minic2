#ifndef _CRYPTO_
#define _CRYPTO_

#define INITIAL_SEED    7	
#define INITIAL_HASH	3731  // added to randomize the hash

#include <Windows.h>
#include "../Definitions.h"

DWORD hashAscii(PCSTR value);
DWORD hashWide(LPCWSTR value);

#endif


