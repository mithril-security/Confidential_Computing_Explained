#pragma once
#include <openenclave/attestation/attester.h>
#include <openenclave/attestation/sgx/report.h>
#include <openenclave/attestation/verifier.h>
#include <openenclave/enclave.h>
#include <string>
#include "attestation.h"
#include "crypto.h"
#include "kms_t.h"


using namespace std;

class dispatcher
{
  private:
    bool m_initialized;
    Crypto* m_crypto;
    Attestation* m_attestation;
    string m_name;

  public:
    dispatcher(const char* name);
    ~dispatcher();
    int get_remote_report_with_pubkey(
        uint8_t** pem_key,
        size_t* key_size,
        uint8_t** remote_report,
        size_t* remote_report_size);

    int get_evidence_with_pubkey(
      const oe_uuid_t* format_id, 
      format_settings_t* format_settings, 
      pem_key_t* pem_key, 
      evidence_t* evidence
    );

  private:
    bool initialize(const char* name);
};