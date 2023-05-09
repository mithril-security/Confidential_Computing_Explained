#include <openenclave/host.h>

/**
 * QuoteGeneration takes the enclave's report and compute the 
 * quote, that is then parsed in json. 
 * The quote is sent to the relying party to verify all the information contained
 * in it. 
*/
class quoteGeneration {
    private: 
        oe_report_t _parsedReport;
        uint8_t* _enclaveHeldData; 
        size_t _enclaveHeldDataSize; 
        uint8_t* _quote;
        size_t _quoteSize; 
    
    public: 
        //Constructor 
        quoteGeneration(oe_report_t parsedReport, uint8_t* ehd, 
            size_t ehdSize, uint8_t* quote, size_t quoteSize    
        ); 
        //Destructor
        ~quoteGeneration();

        //Parser to JSon
        void parseQuote(char* jsondata);

    private: 
        const char *FormatHexBuffer(char* buffer, uint maxSize, uint8_t *data, size_t size); 
}