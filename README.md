### `shared/`

Shared C++ header used by both sides. Keep serial packet changes here first so the Arduino firmware and PC reader stay compatible.

## Required tools

### Arduino firmware

- Visual Studio Code
- PlatformIO extension or PlatformIO Core
- Arduino Uno connected by USB

### Windows serial reader

- Windows
- Visual Studio 2022 Build Tools or equivalent MSVC install
- A terminal where `cl.exe` can run, or the included VS Code task that calls `vcvars64.bat`

## Build and upload

### 1. Open the workspace

Open this file in VS Code:

```text
Arduino Computer Connection VS Workspace.code-workspace
```

The workspace contains three folders: `ArduinoUno`, `SerialReader`, and `shared`.

### 2. Build the Arduino firmware

From VS Code, run the task:

```text
Arduino: Build (Platformio.IO)
```

Or from a terminal:

```bash
cd ArduinoUno
platformio run
```

### 3. Upload the Arduino firmware

From VS Code, run the task:

```text
Arduino: Upload (Platformio.IO)
```

Or from a terminal:

```bash
cd ArduinoUno
platformio run -t upload
```

### 4. Confirm the COM port

On Windows, check Device Manager or PlatformIO output to find the Arduino port. If it is not `COM3`, edit this line in `SerialReader/src/Main.cpp`:

```cpp
const char* port = "\\\\.\\COM3";
```

For example, `COM5` should be written as:

```cpp
const char* port = "\\\\.\\COM5";
```

### 5. Build the Windows reader

From VS Code, run the task:

```text
C++: Build (VS Toolkit)
```

The task compiles:

- `SerialReader/src/Main.cpp`
- `SerialReader/src/SerialConnection.cpp`

and includes:

- `SerialReader/include`
- `shared/include`

### 6. Run the Windows reader

From VS Code, run the task:

```text
C++: Run (VS Toolkit)
```

The reader should print joystick values and move/click the mouse according to joystick input.

## Runtime behavior

- Moving the joystick changes the Windows mouse position.
- Pressing the joystick button sends a left mouse button down event.
- Releasing the joystick button sends a left mouse button up event.
- The PC program prints each valid payload to the console.
- Invalid or incomplete frames are rejected.

## Tuning points

Common values to adjust while testing:

| File | Value | Effect |
| --- | --- | --- |
| `ArduinoUno/src/ArduinoUno.cpp` | `movingThreshold` | Defines how close to center counts as no movement for the Arduino LED indicator |
| `SerialReader/src/Main.cpp` | `moveMultiplier` | Controls mouse movement speed |
| `SerialReader/src/Main.cpp` | `movingThreshold` | Controls whether mouse movement is sent |
| `SerialReader/src/Main.cpp` | `port` | Selects the Windows COM port |

## Notes and limitations

- The PC reader is Windows-only because it uses Win32 serial and mouse APIs.
- The project currently sends raw joystick values. If your joystick center is not close to `512`, calibrate the calculations in `SerialReader/src/Main.cpp`.
- Keep the baud rate and payload structure synchronized on both sides.
- `SerialReader/build/` is generated build output and should not be committed.
