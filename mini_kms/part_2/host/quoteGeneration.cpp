#include <openenclave/host.h>
#include "quoteGeneration.h"

QuoteGeneration::QuoteGeneration(
    oe_report_t parsedReport, 
    uint8_t* ehd, 
    size_t ehdSize, 
    uint8_t* quote, 
    size_t quoteSize, 
)
{
    _parsedReport = parsedReport; 
    _enclaveHeldData = ehd; 
    _enclaveHeldDataSize = ehdSize; 
    _quote = quote; 
    _quoteSize = quoteSize;
}

QuoteGeneration::~QuoteGeneration()
{

}


QuoteGeneration::parseQuote(char* jsondata)
{

}

QuoteGeneration::FormatHexBuffer(char* buffer, uint maxSize, uint8_t *data, size_t size)
{

}