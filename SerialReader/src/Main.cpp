#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>

#include "Payload.h"
#include "SerialConnection.h"

const float moveMultiplier = 0.03f;
const int movingThreshold = 10;
const int maxJoystickValue = 1023;
const int halfJoystickValue = maxJoystickValue / 2;

int main() {
    const char* port = "\\\\.\\COM3";
    DWORD baudRate = CBR_57600;
    int ByteSize = 5;
    SerialConnection ArduinoConnection(port, baudRate, ByteSize);


    uint8_t last_sw = 1;
    float accX = 0, accY = 0;

    while (true) {
		Payload payload = { 0 };

        std::cerr << "Loop Working -" << std::endl;

        if (ArduinoConnection.readStruct<Payload>(payload)) {
            std::cout <<
            
                "x: "       << payload.x << ", " <<
                "y: "       << payload.y << ", " <<
                "Button: "  << (payload.sw == 0 ? "PRESSED" : "RELEASED") << "\n";
        } else {
            std::cerr << "Failed to read payload." << std::endl;
        }

        accX += (payload.x - halfJoystickValue) / 10 * moveMultiplier;
        accY += (payload.y - halfJoystickValue) / 10 * moveMultiplier;

        int moveX = static_cast<int>(accX);
        int moveY = static_cast<int>(accY);

        accX -= moveX;
        accY -= moveY;

        if (payload.x > movingThreshold || payload.y > movingThreshold) {
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dx = moveX;
            input.mi.dy = moveY;
            input.mi.mouseData = 0;
            input.mi.dwFlags = MOUSEEVENTF_MOVE;
            input.mi.time = 0;
            input.mi.dwExtraInfo = GetMessageExtraInfo();
            SendInput(1, &input, sizeof(INPUT));
        }

        if (payload.sw != last_sw) {
            INPUT click = { 0 };
            click.type = INPUT_MOUSE;
            click.mi.dwFlags = (payload.sw == 0) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
            click.mi.time = 0;
            click.mi.dwExtraInfo = GetMessageExtraInfo();
            SendInput(1, &click, sizeof(INPUT));
            last_sw = payload.sw;
        }
    }

    std::cout << "Press Enter to exit...\n";
    std::cin.get();

	return 0;
}