# Pins Tester

`Pins Tester` is an ESP32-based CLI (Command Line Interface) tool for configuring and resetting GPIO pins as buttons. It allows you to specify a pin and its active state, then interact with the button using a console-based command system.

## Features
- Configure a GPIO pin as a button with an active state (`0` or `1`).
- Reset the button to free the GPIO pin.
- Console-based interaction using the `esp-console` library.
- Input validation for pins and states.

## Prerequisites
- ESP-IDF environment set up on your system.
- Argtable3 library included in your ESP-IDF components.
- Any ESP32 based development board.

## Usage
### 1. Build and Flash
To build and flash the firmware, run the following commands in the ESP-IDF environment:
```bash
idf.py set-target esp32h2
idf.py build
idf.py flash
```


## CLI Usage
### 1. Build and Flash
To configure a pin and its corresponding state
```bash
esp> config --state 0 --pin 5
```
To reset a pin
```bash
esp> reset
```
To view the help message
```bash
esp> help
```
