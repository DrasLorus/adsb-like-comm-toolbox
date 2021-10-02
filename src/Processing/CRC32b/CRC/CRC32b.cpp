#include "CRC32b.hpp"

CRC32b::CRC32b() // payload size
{
}

CRC32b::~CRC32b() {
}

uint32_t CRC32b::execute(const std::vector<uint8_t> & v) {
    return execute(v, 0);
}

uint32_t CRC32b::execute(const std::vector<uint8_t> & v, uint32_t R = 0) {
    return execute(v.data(), v.size(), R);
}

uint32_t CRC32b::execute(const uint8_t * M8, const uint32_t bytes) {
    return execute(M8, bytes, 0);
}

uint32_t CRC32b::execute(const uint8_t * M8, const uint32_t bytes, uint32_t R) {
    constexpr uint32_t P = 0x82f63b78;
    for (uint32_t i = 0; i < bytes; ++i) {
        R ^= M8[i];
        for (uint32_t j = 0; j < 8; ++j) {
            R = R & 1 ? (R >> 1) ^ P : R >> 1;
        }
    }
    return R;
}