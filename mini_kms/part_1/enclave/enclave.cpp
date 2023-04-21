

#include <openenclave/enclave.h>
#include <openenclave/attestation/attester.h>
#include <openenclave/attestation/sgx/evidence.h>
#include <openenclave/attestation/sgx/report.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>


#include <sys/epoll.h>
#include<fcntl.h>

#include "../mongoose/mongoose.h"



#include "kms_t.h"
#include "trace.h"

#include "generation/aes_genkey.cpp"
#include "generation/rsa_genkey.cpp"


const char* certificate = "-----BEGIN CERTIFICATE-----\n" \
"MIIDazCCAlOgAwIBAgIUSncGJpHel3efzqcCSgKGmIKxKYMwDQYJKoZIhvcNAQEL\n" \
"BQAwRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM\n" \
"GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yMzA0MTIwNzIyMzVaFw0yNDA0\n" \
"MTEwNzIyMzVaMEUxCzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEw\n" \
"HwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwggEiMA0GCSqGSIb3DQEB\n" \
"AQUAA4IBDwAwggEKAoIBAQCXoWsB9MFYdL61FqH3RYaZhtFMhU/Aog0nf6bTPZyo\n" \
"qUBD8fqU8Bu1VBEJv8Eyyr3wrug7gguUxzzlqnBYIkw3iPLsE4BPkawb5jhMojrC\n" \
"CVyg3JAQiknztjFHhUI2IwGy8kQLB0kWhSajSlFXA2weXqW+jVFOPjcB6WQaQD8H\n" \
"+EwlyyBhEX6n7p4LEpfOkpgx4cwyaj/to+mIzJEc3rjye5clSN9PgjIceHIygaaV\n" \
"SQFFmEmlkGaA97XSHGQimjZVFEVmJosil8sLJ7CvRqvif1L7RQF5rQ4+8TT88Agd\n" \
"1Y/5SQBIqBir5ybhx2tKaK5mzijCuRXq9V0JRVomnMJhAgMBAAGjUzBRMB0GA1Ud\n" \
"DgQWBBS2YHJdg6qW5Jmw/F3bItYW/oRh8TAfBgNVHSMEGDAWgBS2YHJdg6qW5Jmw\n" \
"/F3bItYW/oRh8TAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQAI\n" \
"Pe0Ez3TR+5x7OrOTPOdWlo5nb1nXKpObyccJ6a1PrdepL+gr1awBJ6JmpXeBQ0eW\n" \
"ZdC9zwdyx2uL/ssyXzt3jp4GX7ICkgR/rpMkRgbzJUfvs9i4doFg8shf8A3KJGbV\n" \
"iS8sP54UKLYcA9+JfC5Ryiief+3LBpANJIVPDSTS664bRSG295ksE09Wnqryxlzm\n" \
"Up9hx10Z+pFxz0yNcoaFxiMv/H9wb2hlGJvg+tAgfsDF4E5qtE4T6pf+v1HL1R3O\n" \
"dtJbPalbEucdUZLeBycW8OHSjc0/Z8f8cELzSk8jXLlsOh1q6T8FSGxVbBINaG9p\n" \
"Jpe/k9J2P3ofjGrLtmzb\n" \
"-----END CERTIFICATE-----";

const char*  private_key = "-----BEGIN PRIVATE KEY-----\n" \
"MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQCXoWsB9MFYdL61\n" \
"FqH3RYaZhtFMhU/Aog0nf6bTPZyoqUBD8fqU8Bu1VBEJv8Eyyr3wrug7gguUxzzl\n" \
"qnBYIkw3iPLsE4BPkawb5jhMojrCCVyg3JAQiknztjFHhUI2IwGy8kQLB0kWhSaj\n" \
"SlFXA2weXqW+jVFOPjcB6WQaQD8H+EwlyyBhEX6n7p4LEpfOkpgx4cwyaj/to+mI\n" \
"zJEc3rjye5clSN9PgjIceHIygaaVSQFFmEmlkGaA97XSHGQimjZVFEVmJosil8sL\n" \
"J7CvRqvif1L7RQF5rQ4+8TT88Agd1Y/5SQBIqBir5ybhx2tKaK5mzijCuRXq9V0J\n" \
"RVomnMJhAgMBAAECggEBAI0orajVhUVm8b8+AaxkIkk81A4rsTJA6oQR92mCWxw8\n" \
"7GIUnFYZLYJib7YmuKIjUi4FdpcnmsZ3sL6SLCIwU8oDiTqzcOkiURpr+Y+GG7NV\n" \
"6dCrZMaxJuVmgHv3l8s0TbmchjHKJ4V6kZzEa/LYccfYDu7VOpLfom/6KcGye7tb\n" \
"Qg2onMI7MWxpEom4jFW62/x50oDqGr3snbsAD4dbVxifQYtFkdxyW3oWSqU/ptm0\n" \
"XZ7wTXFMqSgwu3x6R3l9bY6GqFylLAd5xnR/NuuPEAFk8+yZA4tuKAMARFKZwvsr\n" \
"i8p+sI+CkAin2qygHWKeLJ8jB3lBXEWj0U1x1zr9XkECgYEAyFMkM2O6pCNv1RKi\n" \
"rif4fClpIh0YH2xSysHz0PP5tqwRSXCVbJv5t+pp9VYPW6dSTJN2j46oXAXErgyc\n" \
"EwHER1VN5LrycESaVS7X6DjO/t8wvktYoYEhNf75k3/TGjq/VXwcmuCs3ukHSCL6\n" \
"09Z3gP/uBiKrt+ELn/4ZMVGJOVkCgYEAwcW+Erhqo5XtaoMDQgmhUK7CGz/i5chR\n" \
"yfCtWGiYFAL1ngTDgWU/goOm/KwQ1BK6UxV1fUZbrxs5un1EyOYa7GbqPsSu8odT\n" \
"04Ns/Dd8cs8L4FAd/MSdM5qPq5SgeGVpSapkLVJziuoaYJMoaFzagn6lqobwqAWf\n" \
"1/OGACuBqEkCgYEAvJP32iChzG5Hkm9OkV+zPT7GZ4S39qzEa3aVjTh+9IWaaGYB\n" \
"ZBKYy5dyU6/GGWOzx75xt8Z1mtsbQTVBn2rkWLpm71FFdIZAOrw0J7mCss9sVVcc\n" \
"gjzLBMeYLlAAbAqtn02MqAek9NqtLi0vrkeQi/FhIol02U6S4PQUxyaYZVECgYEA\n" \
"pMa01VKBTE6IL9TtBlEOHoAdi/KNzfzEoEbMAawf9pR1rUQ7xGMG2MN/Uso3HUA3\n" \
"tcM/v6H/AS7TrmQsA/K43mK9u1ahtpehjvHp0aiVaUPrw3odpf7D90ft+GhQ9VC3\n" \
"ce81dNzyaCSe/ZTl/alIH0P3OMW7DAaMYKUzRB27ChECgYBiEOUV2soBRBRbMz1/\n" \
"SOmREqYz154TTLW9XyXmcP4ZMS4OcjowX8s1tYFq2jorCZ+xeKHDOUOWHgU/18EP\n" \
"goUDEg/BxfwEqsg7AOEMSTRxaOUXCSdOimumP0dD38C4zvO+imF9BzzGmMpstBst\n" \
"5XSIkvovaR8nRBYBV/aF/kMyPQ==\n" \
"-----END PRIVATE KEY-----";

// Explicitly enabling features 
// This is essential to use the different usual 
// syscall or sockets and file descriptors
oe_result_t load_oe_modules()
{
    oe_result_t result = OE_FAILURE;

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

    if ((result = oe_load_module_host_epoll())!=OE_OK) {
                printf(
            "oe_load_module_host_epoll failed with %s\n",
            oe_result_str(result));
        goto exit;
    }
exit:
    return result;
}

static void api(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_ACCEPT && fn_data != NULL)
    {
        struct mg_tls_opts opts = {
            .cert = certificate, 
            .certkey = private_key,
        };

        mg_tls_init(c, &opts);

    } else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        if (mg_http_match_uri(hm, "/generate-aes-key")) {
            unsigned char key;
            generate_aes_key(&key);
            TRACE_ENCLAVE("key is equal to : {%s}", &key);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%m: \"%s\", %m: \"%s\"}\r\n", mg_print_esc, 0, "aes_key",
                        &key, mg_print_esc, 0, "encoding", "base64");
        } else if (mg_http_match_uri(hm, "/generate-rsa-key-pair")) {
            unsigned char** rsa_public_key;
            unsigned char** rsa_private_key;
            generate_rsa_keypair(rsa_public_key, rsa_private_key);
            TRACE_ENCLAVE("key is equal to : {%x}", &rsa_public_key);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%m: \"%s\", %m: \"%s\"}\r\n", mg_print_esc, 0, "public_key",
                        &rsa_public_key, mg_print_esc, 0, "encoding", "base64");
        }
        else {
            mg_http_reply(c, 200, "", "{\"result\": \"%.*s\"}\n", (int) hm->uri.len,
                        hm->uri.ptr);
        }
    }
    (void) fn_data;
}

int set_up_server(const char* server_port_untrusted, bool keep_server_up )
{
    TRACE_ENCLAVE("Entering enclave.\n");
    TRACE_ENCLAVE("Modules loading...\n");
    if (load_oe_modules() != OE_OK)
    {
        printf("loading required Open Enclave modules failed\n");
        return -1;
    }
    TRACE_ENCLAVE("Modules loaded successfully.\n");

    // ------------------------------------------------------------------------

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);                                        // Init manager
    mg_http_listen(&mgr, "https://0.0.0.0:9000", api, &mgr);  
    // Setup listener
    TRACE_ENCLAVE("Listening at https://0.0.0.0:9000.\n");
    for (;;) mg_mgr_poll(&mgr, 1000);                         // Event loop

    mg_mgr_free(&mgr);  

    return 1;
}