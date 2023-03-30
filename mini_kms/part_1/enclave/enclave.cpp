#include <stdio.h>
#include "trace.h"

#include "kms_t.h"

#include "../common/tls_server.h"
#include "../common/tls_server.cpp"

using namespace Https;

// class EnclaveUserCtxt
// {
// private:
//     int     user;
// public:
//     EnclaveUserCtxt(); // constructor 
//     ~EnclaveUserCtxt(); // desctructor
//     // void generate_aes_key();
//     // void ~generate_aes_key();
// };

// EnclaveUserCtxt::EnclaveUserCtxt()
// {

// }

// EnclaveUserCtxt::~EnclaveUserCtxt()
// {

// }


int set_up_server(const char* server_port_untrusted, const unsigned char* private_key, size_t len_private_key, const unsigned char* certificate, size_t len_certificate, bool keep_server_up)
{
    TRACE_ENCLAVE("Entering enclave.\n");
    TRACE_ENCLAVE("Running set_up_server https server.");
    Https::TlsConnection server;
    return 1; 
}

