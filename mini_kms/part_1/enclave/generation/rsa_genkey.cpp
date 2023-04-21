
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

void generate_rsa_keypair(unsigned char** public_key, unsigned char** private_key)
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

    if ((ret = mbedtls_rsa_gen_key(mbedtls_pk_rsa(key), mbedtls_ctr_drbg_random, &ctr_drbg, 2048, 65537)) != 0) {
        TRACE_ENCLAVE("Failed to generate the RSA key pair: %d\n", ret);
    }
    TRACE_ENCLAVE( "Generated\n");

    // Print the public and private keys in PEM format
    unsigned char public_keybuf[4096];
    unsigned char private_keybuf[4096];
    mbedtls_pk_write_pubkey_pem(&key, public_keybuf, sizeof(public_keybuf));
    *public_key = public_keybuf; 
    TRACE_ENCLAVE("Public key:\n%s\n", public_keybuf);
    mbedtls_pk_write_key_pem(&key, private_keybuf, sizeof(private_keybuf));
    TRACE_ENCLAVE("Private key:\n%s\n", private_keybuf);
    // *private_key = private_keybuf;

    mbedtls_pk_free(&key);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

}


// void generate_rsa_key(unsigned char* public_key, unsigned char* private_key)
// {
//     int ret = 1;
//     size_t buflen = KEY_SIZE*8;

//     int exit_code = MBEDTLS_EXIT_FAILURE;
//     mbedtls_rsa_context rsa;
//     mbedtls_entropy_context entropy;
//     mbedtls_ctr_drbg_context ctr_drbg;
//     mbedtls_mpi N, P, Q, D, E, DP, DQ, QP;

//     const char *pers = "rsa_genkey";

//     mbedtls_ctr_drbg_init( &ctr_drbg );
//     mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
//     mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P ); mbedtls_mpi_init( &Q );
//     mbedtls_mpi_init( &D ); mbedtls_mpi_init( &E ); mbedtls_mpi_init( &DP );
//     mbedtls_mpi_init( &DQ ); mbedtls_mpi_init( &QP );

//     TRACE_ENCLAVE( "Seeding the random number generator...\n" );


//     mbedtls_entropy_init( &entropy );
//     if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
//                                (const unsigned char *) pers,
//                                strlen( pers ) ) ) != 0 )
//     {
//         TRACE_ENCLAVE( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
//         goto exit;
//     }

//     TRACE_ENCLAVE( "Generating the RSA key [ %d-bit ]...\n", KEY_SIZE );
 

//     if( ( ret = mbedtls_rsa_gen_key( &rsa, mbedtls_ctr_drbg_random, &ctr_drbg, KEY_SIZE,
//                                      EXPONENT ) ) != 0 )
//     {
//         TRACE_ENCLAVE( " failed\n  ! mbedtls_rsa_gen_key returned %d\n\n", ret );
//         goto exit;
//     }

//     TRACE_ENCLAVE( "Exporting the public  key....\n");
    

//     if( ( ret = mbedtls_rsa_export    ( &rsa, &N, &P, &Q, &D, &E ) ) != 0 ||
//         ( ret = mbedtls_rsa_export_crt( &rsa, &DP, &DQ, &QP ) )      != 0 )
//     {
//         TRACE_ENCLAVE( " failed\n  ! could not export RSA parameters\n\n" );
//         goto exit;
//     }

//     // if( ( fpub = fopen( "rsa_pub.txt", "wb+" ) ) == NULL )
//     // {
//     //     TRACE_ENCLAVE( " failed\n  ! could not open rsa_pub.txt for writing\n\n" );
//     //     goto exit;
//     // // }
//     // if( ( ret = mbedtls_mpi_write_binary(&N,public_key, buflen) ) != 0 ||
//     //     ( ret = mbedtls_mpi_write_binary( &E,public_key, buflen ) ) != 0 )
//     // {
//     //     TRACE_ENCLAVE( " failed\n  ! mbedtls_mpi_write_binary returned %d\n\n", ret );
//     //     goto exit;
//     // }

//     TRACE_ENCLAVE( "Exporting the private key in rsa_priv.txt...\n" );


//     // if( ( fpriv = fopen( "rsa_priv.txt", "wb+" ) ) == NULL )
//     // {
//     //     TRACE_ENCLAVE( " failed\n  ! could not open rsa_priv.txt for writing\n" );
//     //     goto exit;
//     // }

//     // if( ( ret = mbedtls_mpi_write_binary( "N = " , &N , 16, fpriv ) ) != 0 ||
//     //     ( ret = mbedtls_mpi_write_binary( "E = " , &E , 16, fpriv ) ) != 0 ||
//     //     ( ret = mbedtls_mpi_write_binary( "D = " , &D , 16, fpriv ) ) != 0 ||
//     //     ( ret = mbedtls_mpi_write_binary( "P = " , &P , 16, fpriv ) ) != 0 ||
//     //     ( ret = mbedtls_mpi_write_binary( "Q = " , &Q , 16, fpriv ) ) != 0 ||
//     //     ( ret = mbedtls_mpi_write_binary( "DP = ", &DP, 16, fpriv ) ) != 0 ||
//     //     ( ret = mbedtls_mpi_write_binary( "DQ = ", &DQ, 16, fpriv ) ) != 0 ||
//     //     ( ret = mbedtls_mpi_write_binary( "QP = ", &QP, 16, fpriv ) ) != 0 )
//     // {
//     //     TRACE_ENCLAVE( " failed\n  ! mbedtls_mpi_write_binary returned %d\n\n", ret );
//     //     goto exit;
//     // }
// /*
//     TRACE_ENCLAVE( " ok\n  . Generating the certificate..." );

//     x509write_init_raw( &cert );
//     x509write_add_pubkey( &cert, &rsa );
//     x509write_add_subject( &cert, "CN='localhost'" );
//     x509write_add_validity( &cert, "2007-09-06 17:00:32",
//                                    "2010-09-06 17:00:32" );
//     x509write_create_selfsign( &cert, &rsa );
//     x509write_crtfile( &cert, "cert.der", X509_OUTPUT_DER );
//     x509write_crtfile( &cert, "cert.pem", X509_OUTPUT_PEM );
//     x509write_free_raw( &cert );
// */
//     TRACE_ENCLAVE( "Ok\n" );

//     exit_code = MBEDTLS_EXIT_SUCCESS;

// exit:

//     // if( fpub  != NULL )
//     //     fclose( fpub );

//     // if( fpriv != NULL )
//     //     fclose( fpriv );

//     mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P ); mbedtls_mpi_free( &Q );
//     mbedtls_mpi_free( &D ); mbedtls_mpi_free( &E ); mbedtls_mpi_free( &DP );
//     mbedtls_mpi_free( &DQ ); mbedtls_mpi_free( &QP );
//     mbedtls_rsa_free( &rsa );
//     mbedtls_ctr_drbg_free( &ctr_drbg );
//     mbedtls_entropy_free( &entropy );

// }