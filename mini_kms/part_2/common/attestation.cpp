#include "attestation.h"
#include <openenclave/attestation/attester.h>
#include <openenclave/attestation/custom_claims.h>
#include <openenclave/attestation/verifier.h>
#include <openenclave/bits/report.h>
#include <string.h>
#include "../enclave/trace.h"

Attestation::Attestation(Crypto* crypto)
{
    m_crypto = crypto;
}


bool Attestation::generate_attestation_evidence(
    const oe_uuid_t* format_id, 
    uint8_t* format_settings, 
    size_t format_settings_size, 
    const uint8_t* data, 
    const size_t data_size, 
    uint8_t **evidence, 
    size_t *evidence_size)
{
    bool ret = false; 
    uint8_t hash[32];
    oe_result_t result = OE_OK; 
    // custom claims 
    uint8_t* custom_claims_buffer = nullptr; 
    size_t custom_claims_buffer_size = 0; 
    char custom_claim1_name[] = "Event";
    char custom_claim1_value[] = "Attestation KMS example";
    char custom_claim2_name[] = "Public key hash";

    oe_claim_t custom_claims[2] = {
        {
            .name = custom_claim1_name,
            .value = (uint8_t*)custom_claim1_value,
            .value_size = sizeof(custom_claim1_value)
        },
        {
            .name = custom_claim2_name,
            .value = nullptr,
            .value_size = 0
        }
    };

    if (m_crypto->Sha256(data, data_size, hash) != 0)
    {
        TRACE_ENCLAVE("data hashing failed !\n");
        goto exit; 
    }

    // Initialization of the attestater and plugin 
    result = oe_attester_initialize();
    if (result != OE_OK)
    {
        TRACE_ENCLAVE("oe_attester_initialize failed !\n");
        goto exit; 
    }

    // Adding the Public key's Hash computed 
    custom_claims[1].value = hash;
    custom_claims[1].value_size = sizeof(hash);

    TRACE_ENCLAVE("Serializing the custom claims.\n");
    if (oe_serialize_custom_claims(
        custom_claims, 
        2, 
        &custom_claims_buffer, 
        &custom_claims_buffer_size
    ) != OE_OK)
    {
        TRACE_ENCLAVE("oe_serialize_custom_claims failed !\n");
        goto exit;
    }

    TRACE_ENCLAVE(
    "serialized custom claims buffer size: %lu", custom_claims_buffer_size);

        // Generate evidence based on the format selected by the attester.
    result = oe_get_evidence(
        format_id,
        0,
        custom_claims_buffer,
        custom_claims_buffer_size,
        format_settings,
        format_settings_size,
        evidence,
        evidence_size,
        nullptr,
        0);
    if (result != OE_OK)
    {
        TRACE_ENCLAVE("oe_get_evidence failed.(%s)", oe_result_str(result));
        goto exit;
    }

    ret = true;
    TRACE_ENCLAVE("generate_attestation_evidence succeeded.");
exit:
    oe_attester_shutdown();
    return ret;
}  


/**
 * Generate a remote report for the given data. The SHA256 digest of the data is
 * stored in the report_data field of the generated remote report.
 */
bool Attestation::generate_report(
    const uint8_t* data,
    const size_t data_size,
    uint8_t** remote_report_buf,
    size_t* remote_report_buf_size)
{
    bool ret = false;
    uint8_t sha256[32];
    oe_result_t result = OE_OK;
    uint8_t* temp_buf = NULL;

    if (m_crypto->Sha256(data, data_size, sha256) != 0)
    {
        goto exit;
    }

    // To generate a remote report that can be attested remotely by an enclave
    // running  on a different platform, pass the
    // OE_REPORT_FLAGS_REMOTE_ATTESTATION option. This uses the trusted
    // quoting enclave to generate the report based on this enclave's local
    // report.
    // To generate a remote report that just needs to be attested by another
    // enclave running on the same platform, pass 0 instead. This uses the
    // EREPORT instruction to generate this enclave's local report.
    // Both kinds of reports can be verified using the oe_verify_report
    // function.
    result = oe_get_report(
        OE_REPORT_FLAGS_REMOTE_ATTESTATION,
        sha256, // Store sha256 in report_data field
        sizeof(sha256),
        NULL, // opt_params must be null
        0,
        &temp_buf,
        remote_report_buf_size);
    if (result != OE_OK)
    {
        TRACE_ENCLAVE("oe_get_report failed.");
        goto exit;
    }
    *remote_report_buf = temp_buf;
    ret = true;
    TRACE_ENCLAVE("generate_remote_report succeeded.");
exit:
    return ret;
}