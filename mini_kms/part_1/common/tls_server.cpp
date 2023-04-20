#include <openenclave/enclave.h>

#include <mbedtls/certs.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/pk.h>
#include <mbedtls/platform.h>
#include <mbedtls/rsa.h>
#include <mbedtls/ssl.h>
#include <mbedtls/ssl_cache.h>
#include <mbedtls/x509.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <openenclave/attestation/attester.h>
#include <openenclave/attestation/sgx/evidence.h>
#include <openenclave/attestation/sgx/report.h>
#include <stdio.h>


#include "mbedtls/config.h"
#include "tls_server.h"

// Platform definition
// #if defined(MBEDTLS_PLATFORM_C)
// #include "mbedtls/platform.h"
// #else
// #include <stdlib.h>
// #include <string.h>
#define mbedtls_time       time
#define mbedtls_time_t     time_t
#define mbedtls_fprintf    fprintf
#define mbedtls_printf     printf
#define mbedtls_exit            exit
#define MBEDTLS_EXIT_SUCCESS    EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE    EXIT_FAILURE
// #endif

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

// #include <stdlib.h>
// #include <string.h>


// #include "mbedtls/entropy.h"
// #include "mbedtls/ctr_drbg.h"
// #include "mbedtls/certs.h"
// #include "mbedtls/x509.h"
// #include "mbedtls/ssl.h"
// #include "mbedtls/net_sockets.h"
// #include "mbedtls/error.h"
// #include "mbedtls/debug.h"
// #endif
// // SSL cache definition
// #if defined(MBEDTLS_SSL_CACHE_C)
// #include "mbedtls/ssl_cache.h"
// #endif

// HTTP success response
#define HTTP_RESPONSE \
    "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" \
    "<h2>mbed TLS Test Server</h2>\r\n" \
    "<p>Successful connection using: %s</p>\r\n"

#define SERVER_IP "0.0.0.0"

oe_result_t load_oe_modules()
{
    oe_result_t result = OE_FAILURE;

    // Explicitly enabling features
    if ((result = oe_load_module_host_resolver()) != OE_OK)
    {
        printf(
            "oe_load_module_host_resolver failed with %s\n",
            oe_result_str(result));
        goto exit;
    }
    if ((result = oe_load_module_host_socket_interface()) != OE_OK)
    {
        printf(
            "oe_load_module_host_socket_interface failed with %s\n",
            oe_result_str(result));
        goto exit;
    }
exit:
    return result;
}

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



int TlsConnection::start_server(const char* port)
{
    int ret = 0;
    mbedtls_printf("Setting up the listening TCP Socket\n");
    // mbedtls_net_context listener = get_listen_fd();
    mbedtls_net_context listen_fd_1;
    
  /* Load host resolver and socket interface modules explicitly */
    if (load_oe_modules() != OE_OK)
    {
        printf("loading required Open Enclave modules failed\n");
        return -1;
    }
    mbedtls_printf("(listen_fd = %d)\n", listen_fd_1.fd);
    mbedtls_net_init(&listen_fd_1);
    mbedtls_printf("(listen_fd = %d)\n", listen_fd_1.fd);

    if (( ret = mbedtls_net_bind( &listen_fd_1, SERVER_IP, port, MBEDTLS_NET_PROTO_TCP ) ) != 0)
    {
        mbedtls_printf("Failed.\nmbedtls_net_bind returned %d \n\n", ret);
        return -1;
    }
    mbedtls_printf("mbedtls_net_bind returned successfully. (listen_fd = %d)", listen_fd.fd);

    return 0;
}

int TlsConnection::load_key_cert(const unsigned char *pk, size_t len_pk, const unsigned char *cert, size_t len_cert)
{
    int result;
    mbedtls_pk_context private_key = get_pk(); 
    mbedtls_x509_crt certificate = get_x509_cert();

    if(pk==NULL || cert==NULL){
        return -1;
    }

    // Private key loading
    // if((*private_key = (mbedtls_pk_context*)malloc(sizeof(mbedtls_pk_context))) == NULL)
    // {
    //     mbedtls_printf("malloc function %s", *private_key);

    //     return -1;
    // }

    // mbedtls_pk_init(&private_key); 

    if ((result = mbedtls_pk_parse_key(&private_key, pk, len_pk+1, NULL, 0)) != 0)
    {
        mbedtls_printf("Error loading the private key file: %s, file: %s\n", result, pk);
        return -1;
    }
    // certificate loading
    // if ((*certificate = (mbedtls_x509_crt*)malloc(sizeof(mbedtls_x509_crt))) == NULL)
    // {
    //     return -1; 
    // }
    // mbedtls_x509_crt_init(*certificate);

    if ((result = mbedtls_x509_crt_parse(&certificate, cert, len_cert+1)) != 0) {
        mbedtls_printf("Error loading the X.509 certificates from %s | %d\n", cert, result);
        return -1; 
    }

    // setting the new private key and certificate 
    set_private_key(private_key);
    set_x509_cert(certificate);
    return 0;
}   

// TlsConnection::~TlsConnection()
// {
//     // // 1. freeing of the file descriptors 
//     // mbedtls_net_free(&listen_fd);
//     // mbedtls_net_free(&client_fd); 

//     // // 2. freeing of entropy & drbg
//     // mbedtls_entropy_free(&entropy);
//     // mbedtls_ctr_drbg_free(&ctr_drbg);

//     // // 3. freeing of ssl & conf
//     // mbedtls_ssl_free(&ssl);
//     // mbedtls_ssl_config_free(&conf);


//     // // 4. freeing of cert & key
//     // mbedtls_pk_free(&pkey);
//     // mbedtls_x509_crt_free(&srvcert);
// }