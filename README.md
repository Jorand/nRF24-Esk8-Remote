# nRF24-Esk8-Remote

Control your electric skateboard an Arduino controlled remote. This repository contains the needed software for the remote and the receiver, however you will need to install a few Arduino Libraries in order to compile the Arduino sketches. The Arduino IDE comes with most of the needed libraries, but you will ned to manually install VescUartControl from RollingGecko: https://github.com/RollingGecko/VescUartControl.

You can find the 3D-models for the remote (STL files) on Thingiverse: https://www.thingiverse.com/thing:2454391 and read more about the project on: https://www.electric-skateboard.builders/t/simple-3d-printed-nrf-remote-arduino-controlled/28543

I have made a Wiki here on Github, with a few tips and guides on how to build the remote. The Wiki can be found here: https://github.com/SolidGeek/nRF24-Esk8-Remote/wiki

## What I added so far

**! Important :** this code is still developing, you can test it under your own responsibility. Stay safe !

- Add UART option (saved in EEPROM) in the receiver for sending throttle value and make this option editable from the settings of the remote
- Improve the management of the dead zone and add it in remote settings (very useful for joystick user)
- Make Trigger Mode setting "Killswitch" and "data toggle" working
- Fix battery voltage
- Add cruse control. Trigger Mode `1` (only working with UART yet)
