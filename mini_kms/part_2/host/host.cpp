#include <stdio.h>
#include <openenclave/host.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <string>

#include "kms_u.h"

using namespace std;



// Simulation mode : Doesn't work with Remote Attestation, need of real hardward
bool check_simulate_opt(int* argc, const char* argv[])
{
    for (int i=0; i<*argc; i++)
    {
        if (strcmp(argv[i], "--simulation"))
        {
            cout << "Running on simulation mode" << endl;
            memmove(&argv[i], &argv[i+1], (*argc - i)* sizeof(char *));
            (*argc)--;
            return true; 
        }
    }
    return false;
}

// Enclave creation function
oe_enclave_t* create_enclave(const char* enclave_path, uint32_t flags)
{
    oe_enclave_t* enclave = NULL;

    printf("[Host]: Enclave path %s\n", enclave_path);
    oe_result_t result = oe_create_kms_enclave(
        enclave_path, 
        OE_ENCLAVE_TYPE_AUTO, 
        flags, 
        NULL, 
        0, 
        &enclave);

    if (result != OE_OK)
    {
        printf(
            "[Host]: Enclave creation failed at enclave init : %s\n", oe_result_str(result)
        ); 
    }
    else {
        printf(
            "[Host]: Enclave created Successfully.\n"
        );
    }
    return enclave;
}


int main(int argc, const char* argv[])
{
    oe_result_t result;
    int ret = 1;
    uint32_t flags = OE_ENCLAVE_FLAG_DEBUG;
    oe_enclave_t *enclave = NULL;
    char* server_port_untrusted = "8000";
    char* server_port_trusted = "8001";
    bool keep_server_up = false; 

    // Remote attestation variables
    uint8_t *pem_key = NULL;
    size_t key_size = NULL;
    uint8_t *report = NULL;
    size_t report_size = NULL; 

    
    cout << "[Host]: entering main" << endl;

    // if (check_simulate_opt(&argc, argv))
    // {
    //     flags |= OE_ENCLAVE_FLAG_SIMULATE;
    // }

    // if (argc != 2)
    // {
    //     cout << "Usage" << argv[0] << "enclave_image_path [ --simulation ]"  << endl; 
    //     goto exit;
    // }


    // Enclave creation 
    enclave = create_enclave(argv[1], flags);
    if (enclave == NULL)
    {
        goto exit;
    }
    // Getting the claims for the remote attestation from the enclave
    printf("[Host]: Requesting the report and all the proofs required to establish the Remote Attestation.\n");

    ret = get_report(enclave, &ret, &pem_key, &key_size, &report, &report_size);


    // Setting up the untrusted server
    printf("[Host]: Setting up the http server.\n");

    ret = set_up_server(enclave, &ret, server_port_untrusted, keep_server_up);
    if (ret!=0)
    {
        printf("[Host]: set_up_server failed.\n");
        goto exit;
    }

    // Setting up the trusted server that must be done after the remote attestation 
    // it will give access to the operations around our KMS 
    ret = set_up_trusted_server(enclave, &ret, server_port_trusted);
    if (ret!=0)
    {
        printf("[Host]: set_up_server failed.\n");
        goto exit;
    }

exit: 
    cout << "[Host]: terminate the enclave" << endl;
    cout << "[Host]: running with exit successfully." << endl;
    oe_terminate_enclave(enclave);
    return ret; 
}