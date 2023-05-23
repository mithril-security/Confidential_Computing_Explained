
#include "mbedtls/config.h"


#include "mbedtls/platform.h"

#include <stdio.h>
#include <stdlib.h>
#define TRACE_ENCLAVE          TRACE_ENCLAVE
#define mbedtls_exit            exit
#define MBEDTLS_EXIT_SUCCESS    EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE    EXIT_FAILURE



#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/bignum.h"
#include "mbedtls/x509.h"
#include "mbedtls/rsa.h"
#include "../trace.h"
#include <stdio.h>
#include <string.h>


#define KEY_SIZE 2048
#define EXPONENT 65537

void generate_rsa_keypair(unsigned char* public_key, unsigned char* private_key)
{
    mbedtls_pk_context key;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "rsa_keygen";
    int ret;

    // Initialize contexts
    mbedtls_pk_init(&key);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    // Seed the random number generator
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char *) pers, strlen(pers))) != 0) {
        TRACE_ENCLAVE("Failed to seed the random number generator: %d\n", ret);
 
    }
    TRACE_ENCLAVE( "Setting up the context...\n" );


    // Generate the RSA key pair
    if ((ret = mbedtls_pk_setup(&key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA))) != 0) {
        TRACE_ENCLAVE("Failed to set up the PK context: %d\n", ret);

    }

    TRACE_ENCLAVE( "Generating the RSA key [ %d-bit ]...\n", KEY_SIZE );

    if ((ret = mbedtls_rsa_gen_key(mbedtls_pk_rsa(key), mbedtls_ctr_drbg_random, &ctr_drbg, KEY_SIZE, 65537)) != 0) {
        TRACE_ENCLAVE("Failed to generate the RSA key pair: %d\n", ret);
    }

    // Print the public and private keys in PEM format
    
    mbedtls_pk_write_pubkey_pem(&key, public_key, 2048 * sizeof(unsigned char));

    mbedtls_pk_write_key_pem(&key, private_key, 2048 * sizeof(unsigned char));


    mbedtls_pk_free(&key);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

}

