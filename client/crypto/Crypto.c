#include <Windows.h>
#include <wincrypt.h>
#include "Crypto.h"
#include "../memoryCollector/MemoryCollector.h"


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

typedef struct
{
	unsigned int i;
	unsigned int j;
	unsigned char s[256];

} Rc4Context;

void initChiper(Rc4Context* context, const unsigned char* key, size_t length)
{
	unsigned int i;
	unsigned int j;
	unsigned char temp;

	// Check parameters
	if (context == NULL || key == NULL)
		return;

	// Clear context
	context->i = 0;
	context->j = 0;

	// Initialize the S array with identity permutation
	for (i = 0; i < 256; i++)
	{
		context->s[i] = i;
	}

	// S is then processed for 256 iterations
	for (i = 0, j = 0; i < 256; i++)
	{
		//Randomize the permutations using the supplied key
		j = (j + context->s[i] + key[i % length]) % 256;

		//Swap the values of S[i] and S[j]
		temp = context->s[i];
		context->s[i] = context->s[j];
		context->s[j] = temp;
	}

}

PCSTR decrypt(unsigned char* base64Blob, DWORD base64DBlobLength)
{
	DWORD blobStrLength = frombase64GetLength(base64Blob, base64DBlobLength);
    PBYTE blob = fromBase64GetBlob(base64Blob, base64DBlobLength, blobStrLength);
    return encrypt(blob, blobStrLength);
};

DWORD frombase64GetLength(unsigned char* base64string,  DWORD lengthbase64string)
{
	DWORD sizeToAlloc = 0;
	CryptStringToBinaryA(base64string, 0, CRYPT_STRING_BASE64, NULL, &sizeToAlloc, NULL, NULL);
	return sizeToAlloc;
};

PBYTE fromBase64GetBlob(unsigned char* base64string,  DWORD lengthbase64string, DWORD blobDataLength)
{
	PBYTE chiperText = (PBYTE)allocMemory(blobDataLength);
	RtlSecureZeroMemory(chiperText, blobDataLength);
	CryptStringToBinaryA(base64string, 0, CRYPT_STRING_BASE64, chiperText, &blobDataLength, NULL, NULL);
	return chiperText;
};


PBYTE encrypt(unsigned char* input, DWORD length)
{
    Rc4Context ctx = { 0 };
    unsigned char key[] = "FNDSJFBSDFIDSFANLDSANIUADCBJISDCDFA";
    DWORD sizeofkey = sizeof(key) * sizeof(unsigned char);
    initChiper(&ctx, key, sizeof(key) * sizeof(unsigned char));
	unsigned char temp;
    Rc4Context* context = &ctx;
    
	// Restore context
	unsigned int i = context->i;
	unsigned int j = context->j;
	unsigned char* s = context->s;

    PBYTE outputChiper = (PBYTE)allocMemory(length + 1);
	PBYTE output = outputChiper;
	PBYTE auxInput = input;
	DWORD index = length;
    // Encryption loop
	while (index > 0)
	{
		// Adjust indices
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;

		// Swap the values of S[i] and S[j]
		temp = s[i];
		s[i] = s[j];
		s[j] = temp;

		// Valid input and output?
		if (input != NULL && output != NULL)
		{
			//XOR the input data with the RC4 stream
			*outputChiper = *input ^ s[(s[i] + s[j]) % 256];

			//Increment data pointers
			input++;
			outputChiper++;
		}

		// Remaining bytes to process
		index--;
	}

	// Save context
	context->i = i;
	context->j = j;
	output[length] = '\0';
	freeMemory(auxInput);
    return output;
}

