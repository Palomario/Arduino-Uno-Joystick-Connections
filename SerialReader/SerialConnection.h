#pragma once
#include <windows.h>
#include <iostream>

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
    BYTE startByte;
    DWORD read;

    // Wait for start byte (0xFF)
    while (true) {
        BOOL success = ReadFile(hSerial, &startByte, 1, &read, NULL);

        if (!success) {
            std::cerr << "ReadFile failed when waiting for start byte: " << GetLastError() << "\n";
            return false;
        }

        if (read == 1 && startByte == 0xFF)
            break; // We got the start byte!
    }

    // Read the actual struct
    BOOL success = ReadFile(hSerial, &result, sizeof(StructVariable), &read, NULL);

    if (!success || read != sizeof(StructVariable)) {
        std::cerr << "Failed to read struct.\n";
        return false;
    }

    return true;
}

