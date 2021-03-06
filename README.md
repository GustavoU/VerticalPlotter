# VerticalPlotter

This is the firmware repo for a custom open source vertical V plotter controlled by a physical joystick.
It's made with an ESP32 (NodeMCU-32s) and uses the Arduino framework.
Later commits will include (hopefully) an HTML Joystick based on this one: https://github.com/bobboteck/JoyStick.
You will be able to find a PlatformIO project as well as an ArduinoIDE one.


Materials:
  - 1x ESP32
  - 2x Stepper mottor
  - 1x Analog Joystick (X/Y)
  - 2x Pololu DRV8825 Stepper driver
  - 1x microSD breakout board (SPI)
  
![wiring_diagram](https://github.com/GustavoU/VerticalPlotter/blob/master/ESP32%20VPLOTTER%20conexiones.jpg)
![main_screen](https://github.com/GustavoU/VerticalPlotter/blob/master/main_screen.PNG)
![homing_screen](https://github.com/GustavoU/VerticalPlotter/blob/master/homing_screen.PNG)
![params_screen](https://github.com/GustavoU/VerticalPlotter/blob/master/params_screen.PNG)
![canvas_screen](https://github.com/GustavoU/VerticalPlotter/blob/master/canvas_screen.PNG)
