#ifndef TLS_SERVER_H
#define TLS_SERVER_H

#include "mbedtls/config.h"
// Platform definition
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_time       time
#define mbedtls_time_t     time_t
#define mbedtls_fprintf    fprintf
#define mbedtls_printf     printf
#define mbedtls_exit            exit
#define MBEDTLS_EXIT_SUCCESS    EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE    EXIT_FAILURE
#endif

// #if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_CERTS_C) ||    \
//     !defined(MBEDTLS_ENTROPY_C) || !defined(MBEDTLS_SSL_TLS_C) || \
//     !defined(MBEDTLS_SSL_SRV_C) || !defined(MBEDTLS_NET_C) ||     \
//     !defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_CTR_DRBG_C) ||    \
//     !defined(MBEDTLS_X509_CRT_PARSE_C) || !defined(MBEDTLS_FS_IO) || \
//     !defined(MBEDTLS_PEM_PARSE_C)
// int main( void )
// {
//     mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_CERTS_C and/or MBEDTLS_ENTROPY_C "
//            "and/or MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_SRV_C and/or "
//            "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
//            "MBEDTLS_CTR_DRBG_C and/or MBEDTLS_X509_CRT_PARSE_C "
//            "and/or MBEDTLS_PEM_PARSE_C not defined.\n");
//     mbedtls_exit( 0 );
// }
// #else
#include <stdlib.h>
#include <string.h>

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
// #endif
// SSL cache definition
#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif



namespace Https {
    class TlsConnection {
        private: 
            mbedtls_net_context listen_fd;
            mbedtls_net_context client_fd;
            mbedtls_entropy_context entropy;
            mbedtls_ctr_drbg_context ctr_drbg;
            mbedtls_ssl_context ssl;
            mbedtls_ssl_config conf;
            mbedtls_x509_crt srvcert;
            mbedtls_pk_context pkey;
    #if defined(MBEDTLS_SSL_CACHE_C)
            mbedtls_ssl_cache_context cache;
    #endif
        public: 
            TlsConnection();
            // ~TlsConnection();

            //getters
            mbedtls_x509_crt get_x509_cert(){ return srvcert; }
            
            // setters
            void set_x509_cert(mbedtls_x509_crt cert) { srvcert = cert; }
            void set_private_key(mbedtls_pk_context pk) { pkey = pk; }
            //method for establishing connection
            int establish_connection(int port); 
            int load_key_cert(const unsigned char *pk, size_t len_pk, const unsigned char* cert, size_t len_cert);
    };
}

#endif

