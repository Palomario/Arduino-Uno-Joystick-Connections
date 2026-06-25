#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>

#include "Payload.h"
#include "SerialConnection.h"

const float moveMultiplier = 0.3f;
const int movingThreshold = 30;
const int maxJoystickValue = 1023;
const int halfJoystickValue = maxJoystickValue / 2;

int main() {
    const char* port = "\\\\.\\COM3";
    DWORD baudRate = CBR_9600;
    int ByteSize = 8;
    SerialConnection ArduinoConnection(port, baudRate, ByteSize);

    uint8_t last_sw = 1;
    float accX = 0, accY = 0;

    int lastX = -1;
    int lastY = -1;
	int lastSW = -1;

    while (true) {
        Payload payload = {};

        if (!ArduinoConnection.readStruct<Payload>(payload)) {
            std::cerr << "Failed to read payload.\n";
            continue;
        }

           

        if (payload.x != lastX || payload.y != lastY || payload.sw != lastSW) { 
            

            //bool noMovement = abs(rawX - 512) < movingThreshold && abs(rawY - 512) < movingThreshold;

            std::string xString = std::to_string(payload.x - 512);
            std::string yString = std::to_string(payload.y - 512);
            std::string SWString = payload.sw == 0 ? "ON" : "OFF";

            

            if ((std::abs(payload.x - 512) > movingThreshold || std::abs(payload.y - 512) > movingThreshold) || payload.sw != lastSW) {
                system("cls");

                std::cout << "x:" << xString
                    << ", y:" << yString
                    << ", sw:" << SWString
                    << '\n';
            }

			lastSW = payload.sw;
            lastX = payload.x;
            lastY = payload.y;
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

    return 0;
}