#include "mbedtls/config.h"
#include "tls_server.h"

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

#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_CERTS_C) ||    \
    !defined(MBEDTLS_ENTROPY_C) || !defined(MBEDTLS_SSL_TLS_C) || \
    !defined(MBEDTLS_SSL_SRV_C) || !defined(MBEDTLS_NET_C) ||     \
    !defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_CTR_DRBG_C) ||    \
    !defined(MBEDTLS_X509_CRT_PARSE_C) || !defined(MBEDTLS_FS_IO) || \
    !defined(MBEDTLS_PEM_PARSE_C)
int main( void )
{
    mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_CERTS_C and/or MBEDTLS_ENTROPY_C "
           "and/or MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_SRV_C and/or "
           "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
           "MBEDTLS_CTR_DRBG_C and/or MBEDTLS_X509_CRT_PARSE_C "
           "and/or MBEDTLS_PEM_PARSE_C not defined.\n");
    mbedtls_exit( 0 );
}
#else

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
#endif
// SSL cache definition
#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif

// HTTP success response
#define HTTP_RESPONSE \
    "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" \
    "<h2>mbed TLS Test Server</h2>\r\n" \
    "<p>Successful connection using: %s</p>\r\n"



// class TlsConnection {
//     private: 
//         mbedtls_net_context listen_fd;
//         mbedtls_net_context client_fd;
//         mbedtls_entropy_context entropy;
//         mbedtls_ctr_drbg_context ctr_drbg;
//         mbedtls_ssl_context ssl;
//         mbedtls_ssl_config conf;
//         mbedtls_x509_crt srvcert;
//         mbedtls_pk_context pkey;
// #if defined(MBEDTLS_SSL_CACHE_C)
//         mbedtls_ssl_cache_context cache;
// #endif

//     public: 
//         TlsConnection(mbedtls_x509_crt srvcert, mbedtls_pk_context pkey);
//         ~TlsConnection();
// }

using namespace Https;

TlsConnection::TlsConnection()
{
    // 1. Initialization of the file descriptors 
    mbedtls_net_init(&listen_fd);
    mbedtls_net_init(&client_fd); 

    // 2. Initialization of entropy & drbg
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    // 3. Initialization of ssl & conf
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);


    // 4. Initialization of cert & key
    mbedtls_pk_init(&pkey);
    mbedtls_x509_crt_init(&srvcert);
}

int TlsConnection::establish_connection(int port)
{
    return 0;
}

int TlsConnection::load_key_cert(const unsigned char *pk, size_t len_pk, const unsigned char *cert, size_t len_cert)
{
    int result;
    mbedtls_pk_context **private_key; 
    mbedtls_x509_crt **certificate; 

    if(pk==NULL || cert==NULL){
        return -1;
    }

    // Private key loading
    if((*private_key = (mbedtls_pk_context*)malloc(sizeof(mbedtls_pk_context))) == NULL)
    {
        return -1;
    }

    mbedtls_pk_init(*private_key); 

    if ((result = mbedtls_pk_parse_key(*private_key, pk, len_pk, NULL, NULL)) != 0)
    {
        mbedtls_printf("Error loading the private key file: %d, file: %s", result, pk);
        return -1;
    }

    // certificate loading
    if ((*certificate = (mbedtls_x509_crt*)malloc(sizeof(mbedtls_x509_crt))) == NULL)
    {
        return -1; 
    }

    mbedtls_x509_crt_init(*certificate);

    if ((result = mbedtls_x509_crt_parse(*certificate, cert, len_cert)) != 0) {
        mbedtls_printf("Error loading the X.509 certificates from %s | %d", cert, result);
        return -1; 
    }
    return 0;
}   