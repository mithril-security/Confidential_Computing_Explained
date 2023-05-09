#ifndef OE_ATTESTATION_H
#define OE_ATTESTATION_H

#include <openenclave/enclave.h>
#include "crypto.h"

#define ENCLAVE_SECRET_DATA_SIZE 16
class Attestation
{
  private:
    Crypto* m_crypto;

  public:
    Attestation(Crypto* crypto);

    /**
     * Generate evidence for given data
    */
   bool generate_attestation_evidence(
      const oe_uuid_t* format_id, 
      uint8_t* format_settings, 
      size_t format_settings_size, 
      const uint8_t* data, 
      const size_t data_size, 
      uint8_t **evidence, 
      size_t* evidence_size
   ); 

    // Generate a remote report for the given data. The SHA256 digest of the
    // data is stored in the report_data field of the generated remote report.
    bool generate_report(
        const uint8_t* data,
        size_t data_size,
        uint8_t** remote_report_buf,
        size_t* remote_report_buf_size);
};

#endif 