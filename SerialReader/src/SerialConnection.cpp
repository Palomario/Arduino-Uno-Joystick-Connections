#include "SerialConnection.h"

SerialConnection::SerialConnection(const char* port, DWORD baudRate, int ByteSize)
    : port(port), baudRate(baudRate), ByteSize(ByteSize)
{
    this->hSerial = createHSerial();
    configureSerialPort();
}

SerialConnection::~SerialConnection() {
    CloseHandle(hSerial);
}

HANDLE SerialConnection::createHSerial() {
    HANDLE hSerial = CreateFileA(
        port,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening serial port: " << GetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }

    return hSerial;
}

void SerialConnection::configureSerialPort() {

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error getting serial port state: " << GetLastError() << std::endl;
        CloseHandle(hSerial);
        exit(EXIT_FAILURE);
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = ByteSize;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error setting serial port state: " << GetLastError() << std::endl;
        CloseHandle(hSerial);
        exit(EXIT_FAILURE);
    }

    COMMTIMEOUTS to = { 0 };
    to.ReadIntervalTimeout = 50;  // ms between bytes
    to.ReadTotalTimeoutConstant = 100; // overall read cap
    to.ReadTotalTimeoutMultiplier = 10;  // per-byte add
    to.WriteTotalTimeoutConstant = 100;
    to.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &to)) {
        std::cerr << "SetCommTimeouts failed: " << GetLastError() << "\n";
        exit(EXIT_FAILURE);
    }

    // Optional but helpful
    SetupComm(hSerial, 1 << 12, 1 << 12);
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
}
