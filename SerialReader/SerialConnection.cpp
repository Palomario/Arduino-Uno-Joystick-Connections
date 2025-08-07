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
    const char* port = "\\\\.\\COM3";
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

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;          // ms between characters
    timeouts.ReadTotalTimeoutConstant = 50;     // total read timeout
    timeouts.ReadTotalTimeoutMultiplier = 10;   // per-byte timeout

    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Error setting timeouts: " << GetLastError() << std::endl;
        CloseHandle(hSerial);
        exit(EXIT_FAILURE);
    }
}
