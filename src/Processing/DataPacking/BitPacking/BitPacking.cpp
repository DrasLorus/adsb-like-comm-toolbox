#include "BitPacking.hpp"

BitPacking::BitPacking() {
}

BitPacking::~BitPacking() {
}

void BitPacking::execute(const std::vector<uint8_t> & ibuffer, std::vector<uint8_t> & obuffer) {
    // Le buffer de sortie doit etre 2x plus grand...
    if ((ibuffer.size() / 8) != obuffer.size()) {
        printf("\x1B[33m(WW) vector size was updated in BitPacking(%s::%d)\x1B[0m\n", __FILE__, __LINE__);
        obuffer.resize(ibuffer.size() / 8);
    }

    const uint8_t * ptr     = ibuffer.data();
    const uint32_t  oLength = obuffer.size();
    for (uint32_t i = 0; i < oLength; i += 1) {
        uint8_t v = 0;
#if defined(__clang__)
#pragma clang loop unroll(full)
#endif
        for (uint32_t q = 0; q < 8; q += 1) {
            v = (v << 1) | (*ptr++);
        }
        obuffer[i] = v;
    }
}
