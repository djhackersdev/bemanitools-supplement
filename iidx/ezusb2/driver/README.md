# Drivers for Ezusb FX2, aka IO2
Currently, this package offers drivers for the following platforms:
* Windows XP: x86, x64
* Windows Vista: x86, x64 (which you will never need...)
* Windows 7: x86, x64
* Windows 8: x86, x64
* Windows 8.1: x86, x64
* Windows 10: x86, x64

## Installing on Windows XP
* Plug in the device and Windows should recognize it automatically
* Do not connect to Windows Update to search for software if prompted
* "Install from a list or specific location (Advanced)" -> Next
* "Don't search. I will choose the driver to install" -> Next
* "Have Disk..." -> "Browse..." -> Select the xp/x86 subfolder containing the 
*sys* and *inf* file
* Select the device "Cypress FX2LP No EEPROM Device" from the list -> 
Next
* That should be it

## Installing on Windows Vista, 7, 8, 8.1
* Plug in the device and Windows should recognize it automatically
* "Browse my computer for driver software" -> Next
* "Let me pick from a list of device drivers on my computer" -> Next
* "Have Disk..." -> "Browse..." -> Select your target platform, e.g. Win 7, and
architecture, e.g. x64
* Select the device "Cypress FX2LP No EEPROM Device" from the list -> 
Next
* That should be it

## Flashing of firmware
In order to use the hardware with the game, you have to flash the correct
firmware to the device. Follow the (firmware guide)[../firmware/README.md] for
this.

## Testing
Instead of using the games, use one of the tools included with BT5 to test and
debug issues as it will error on more fine granular level. Furthermore, you
can also check the source code for details and to track down any unknown issues.
