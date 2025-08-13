#pragma once
#include <windows.h>
#include <iostream>
#include <cstdlib>

class SerialConnection {

public:
    SerialConnection(const char* port, DWORD baudRate, int ByteSize);

    ~SerialConnection();

    template <typename StructVariable>
    bool readStruct(StructVariable& result);

private:
    HANDLE hSerial;

    const char* port;
    int baudRate;
    int ByteSize;

    HANDLE createHSerial();

    void configureSerialPort();
};

template <typename StructVariable>
bool SerialConnection::readStruct(StructVariable& result) {
    BYTE byte;
    DWORD read;

    std::cerr << "Reading Structure" << std::endl;


    bool checkingSOF1 = true; 

    while (true) {
        std::cerr << "Got header: 0x" << std::hex << int(StructVariable::SOF1_VALUE)
          << " 0x" << int(StructVariable::SOF2_VALUE)
          << " | last bytes read: 0x" << int(byte)
          << std::dec << "\n";


        if (!ReadFile(hSerial, &byte, 1, &read, NULL)) {
            std::cerr << "ReadFile failed while hunting SOF: " << GetLastError() << "\n";
            return false;
        }

        if (read != 1) continue;

        if (checkingSOF1) {
            if (byte == StructVariable::SOF1_VALUE) {
                checkingSOF1 = false;
            }
        } else {
            if (byte == StructVariable::SOF2_VALUE) {
                break;
            } else {
                checkingSOF1 = true;
            }
        }
    }

    // Read the rest of the struct, skipping the two SOF bytes
    BOOL success = ReadFile(
        hSerial,
        reinterpret_cast<uint8_t*>(&result) + 2,
        static_cast<DWORD>(sizeof(StructVariable) - 2),
        &read,
        NULL
    );

    if (!success || read != static_cast<DWORD>(sizeof(StructVariable) - 2)) {
        std::cerr << "Failed to read struct body.\n";
        return false;
    }

    // -- Important for Re-Emiting the Byte-String -- //
    //result.sof1 = StructVariable::SOF1_VALUE;
    //result.sof2 = StructVariable::SOF2_VALUE;

    if (payload_checksum(reinterpret_cast<const Payload&>(result)) != result.crc) {
        std::cerr << "CRC mismatch — discarding frame.\n";
        return false; 
    }

    return true;
}


