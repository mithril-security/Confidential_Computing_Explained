#include <stdio.h>
#include <openenclave/host.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>

#include "kms_u.h"

using namespace std;



// Simulation mode 
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

// function for getting the tls certificate and pk to pass on to the server
std::string get_file(char* filename)
{
    std::fstream file;
    std::string file_data;
    file.open(filename);

    if ( file.is_open() ) {
        while (file)
        {
            file_data += file.get();
        }
    }
    return file_data;
}


int main(int argc, const char* argv[])
{
    oe_result_t result;
    int ret = 1;
    uint32_t flags = OE_ENCLAVE_FLAG_DEBUG;
    oe_enclave_t *enclave = NULL;
    const char* server_port_untrusted = "9001";
    bool keep_server_up = false; 
    
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

    printf("[Host]: Getting certificate and private key for the tls connection.");
    std::string private_key = get_file("../key.pem");
    std::string certificate = get_file("../certificate.pem");

    const unsigned char *c_private_key = (const unsigned char*)private_key.c_str();
    size_t len_private_key = (size_t)private_key.size();
    const unsigned char *c_certificate = (const unsigned char*)certificate.c_str();
    size_t len_certificate = (size_t)certificate.size(); 

    // Enclave creation 
    enclave = create_enclave(argv[1], flags);
    if (enclave == NULL)
    {
        goto exit;
    }

    printf("[Host]: Setting up the http server.\n");

    ret = set_up_server(enclave, &ret, server_port_untrusted, c_private_key, len_private_key, c_certificate, len_certificate,  keep_server_up);
    
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