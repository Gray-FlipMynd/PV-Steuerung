# PV-Steuerung
C++ Code for ESP32/Arduino based automation for Suntracking and motorcontrol

Currently in USE PV-Current tracking:

- Current tracking via ACS712 Sensor up to 30 amps.
- Reading/Writing currenttracking to SD Card via SC Card Reader Module using SPI Protocoll
- RTC for timecorrect readings

Currently in USE -Motor Control (V7):
- Using an IBT-2 HBridge Motorcontroller for high torque Motors up to 40A
- DC Motor with gear Set to control The Position of the PV - Panel.
- USS HC-SR04 for distance measuring and controlling the Positon of PV -Panel for optimal sunray positioning
- RTC - Time based control for additional sunray positioning
- OLED Display SSD1306 for displaying the measurements in RealTime
- added deepsleep functionality to safe resources over night
