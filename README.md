# Proj2 MicroC2 Sess3 - LED Matrix Target Game

This repository contains my second school project for **243-33A-MO: Microcontroleur 2**. It is a small arcade-style target game built with PlatformIO and Arduino for an Arduino Mega 2560 using a 64x32 RGB LED matrix.

The game uses the matrix as the screen, the push buttons as controls, the rotary encoder to choose the starting lives, the potentiometer for a cheat option, and EEPROM to save the high score.

## Gameplay

- The title screen shows the selected lives and whether God Mode is enabled.
- The player moves a crosshair with the directional buttons.
- Button `B` shoots targets when the crosshair is over them.
- Targets grow over time and eventually cost a life if they are not hit.
- Button `A` ends the game immediately.
- Button `C` starts a new game or returns to the title screen after game over.
- The game saves the best score in EEPROM.

## Features

- Title, gameplay, and game over states.
- Moving crosshair controlled by push buttons.
- Expanding target objects implemented with a `Ball` class.
- Timer 3 compare interrupt setup.
- Interrupt-based rotary encoder handling.
- Potentiometer-controlled God Mode.
- EEPROM high-score storage.
- Local Game Design Document included as `Game_Design_Document.docx`.

## Hardware And Tools

- Arduino Mega 2560
- MOMO RGB Matrix / 64x32 RGB LED matrix
- Push buttons, rotary encoder, potentiometer, and EEPROM
- PlatformIO
- Arduino framework
- C++

## Project Structure

```text
.
|-- Game_Design_Document.docx
|-- platformio.ini          # PlatformIO board and framework configuration
|-- src/main.cpp            # Main game logic
|-- include/bits_manip.h    # Bit manipulation helper declarations
|-- lib/MOMO_RGB_Matrix/    # Local RGB matrix library
|-- lib/bits_manip/         # Bit manipulation helper library
`-- README.md
```

## Build And Upload

Install PlatformIO, connect the Arduino Mega 2560, then run:

```bash
pio run
pio run -t upload
```

## Notes

This was created as a school project, so the code is focused on demonstrating the required microcontroller concepts: matrix drawing, button controls, interrupts, timer usage, classes, analog input, and EEPROM storage.

## License

Released under the Unlicense. See [LICENSE.txt](LICENSE.txt) for details.
