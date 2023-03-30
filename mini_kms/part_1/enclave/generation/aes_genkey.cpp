#include <stdio.h>
#include <string.h>
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "../trace.h"


void generate_aes_key()
{
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;

    char *personalized = "aes gen key miniKMS";
    int ret; 

    // variable containing the AES Key 
    unsigned char key[32];

    // Initialization 
    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );

    if ( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, 
    (unsigned char *)personalized, strlen(personalized) ) ) != 0 )
    {
        TRACE_ENCLAVE("Failed ! mbedtls_ctr_drbg_seed returned with -0x%04x", -ret);
    }

    if ( ( ret = mbedtls_ctr_drbg_random(&ctr_drbg, key, 32) ) != 0 )
    {
        TRACE_ENCLAVE("Failed ! mbedtls_ctr_drbg_random returned with -0x%04x", -ret);
    }
    
    TRACE_ENCLAVE("Key generated is  : %s", key);
}

