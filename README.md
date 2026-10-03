# Smart Motor Health & Fault Detection System

This STM32 project monitors a simulated industrial motor using ADC inputs.

## What it measures

- Motor temperature
- Vibration level
- Motor current
- Motor speed in RPM

## What it does

The firmware converts the sensor signals into engineering values, calculates a motor-health score, and detects over-temperature, excess vibration, over-current, and low-speed/stall conditions. LEDs and a buzzer show the current status, while the Serial Monitor provides one-second telemetry records.

## Run in Wokwi

1. Create an STM32 Nucleo C031C6 project.
2. Upload `sketch.ino` and `diagram.json`.
3. Start the simulation.
4. Move the four potentiometers to simulate temperature, vibration, current, and RPM.
5. Open the Serial Monitor at 115200 baud.

This is a low-voltage simulator model. Real motor installations require isolated, calibrated sensors and proper electrical protection.
