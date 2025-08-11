#pragma once

#pragma pack(push, 1)
struct Payload {
    static constexpr uint8_t SOF1_VALUE = 0xFF;
    static constexpr uint8_t SOF2_VALUE = 0x55;

    uint8_t sof1 = SOF1_VALUE;  // start-of-frame byte 1
    uint8_t sof2 = SOF2_VALUE;  // start-of-frame byte 2

    int16_t x;                  // 0–1023 joystick X
    int16_t y;                  // 0–1023 joystick Y
    uint8_t sw;                  // 0=pressed, 1=released
    uint8_t crc;                 // checksum over x, y, sw (not SOF bytes, not crc)
};
#pragma pack(pop)

static inline uint8_t payload_checksum(const Payload& p) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&p.x); // skip sof1, sof2
    uint8_t s = 0;
    for (size_t i = 0; i < sizeof(Payload) - 3; ++i) s += b[i]; // x,y,sw only
    return s;
}
