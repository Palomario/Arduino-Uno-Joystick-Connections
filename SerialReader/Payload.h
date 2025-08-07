#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct Payload {
    int16_t x;
    int16_t y;
    uint8_t sw;
};
#pragma pack(pop)
