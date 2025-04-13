#include <Windows.h>
#include "Crypto.h"



DWORD hashAscii(PCSTR value)
{
    #ifdef HASH_MODE_JENKINS
    
    
        SIZE_T Index = 0;
        UINT32 Hash = 0;
        SIZE_T Length = lstrlenA(value);

        while (Index != Length)
        {
            Hash += value[Index++];
            Hash += Hash << INITIAL_SEED;
            Hash ^= Hash >> 6;
        }

        Hash += Hash << 3;
        Hash ^= Hash >> 11;
        Hash += Hash << 15;

        return Hash;
    #endif

    #ifdef HASH_MODE_DJB2
        ULONG Hash = INITIAL_HASH;
        INT c;

        while (c = *value++)
            Hash = ((Hash << INITIAL_SEED) + Hash) + c;

        return Hash;
    #endif
}


DWORD hashWide(LPCWSTR value)
{
    #ifdef HASH_MODE_JENKINS
        SIZE_T Index = 0;
        UINT32 Hash = 0;
        SIZE_T Length = lstrlenW(value);

        while (Index != Length)
        {
            Hash += value[Index++];
            Hash += Hash << INITIAL_SEED;
            Hash ^= Hash >> 6;
        }

        Hash += Hash << 3;
        Hash ^= Hash >> 11;
        Hash += Hash << 15;

        return Hash;
    #endif

    #ifdef HASH_MODE_DJB2
        ULONG Hash = INITIAL_HASH;
        INT c;

        while (c = *value++)
            Hash = ((Hash << INITIAL_SEED) + Hash) + c;

        return Hash;
    #endif
}