#ifndef _CRYPTO_
#define _CRYPTO_

#define INITIAL_SEED    7	
#define INITIAL_HASH	3731  // added to randomize the hash

#include <Windows.h>
#include "../Definitions.h"

DWORD hashAscii(PCSTR value);
DWORD hashWide(LPCWSTR value);

PBYTE encrypt(unsigned char* input, DWORD length);
PCSTR decrypt(unsigned char* base64Blob, DWORD base64DBlobLength);
DWORD frombase64GetLength(unsigned char* base64string,  DWORD lengthbase64string);
PBYTE fromBase64GetBlob(unsigned char* base64string,  DWORD lengthbase64string, DWORD blobDataLength);

#endif