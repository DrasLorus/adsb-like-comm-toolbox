#include "BitUnpacking.hpp"


BitUnpacking::BitUnpacking()
{

}


BitUnpacking::~BitUnpacking()
{

}


void BitUnpacking::execute(const std::vector<uint8_t>& ibuffer, std::vector<uint8_t>& obuffer)
{
	// Le buffer de sortie doit etre 2x plus grand...
    if( obuffer.size() != (8 * ibuffer.size()) )
    {
        printf("\x1B[33m(WW) vector size was updated in BitUnpacking(%s::%d)\x1B[0m\n", __FILE__, __LINE__);
        obuffer.resize(8 *  ibuffer.size());
    }

    uint8_t* ptr = obuffer.data();
    const uint32_t iLength = ibuffer.size();
    for(uint32_t i = 0; i < iLength; i += 1)
    {
        const uint8_t v = ibuffer[i];
#if defined(__clang__)
#pragma clang loop unroll(full)
#endif
        for( uint32_t q = 0; q < 8 ; q += 1 )
        {
            (*ptr++) = (v >> (7-q)) & 0x01;
        }
    }
}
