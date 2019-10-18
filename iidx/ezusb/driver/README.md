# Drivers for Ezusb (legacy), aka C02 IO
Currently, this package offers drivers for the following platforms:
* Windows XP: x86
* Windows 7: x86, x64
* Windows 10: x86, x64

Furthermore, the source code of the driver is included (see *src* folder). The
Windows XP driver is the original driver people have used in the past already.
All other platforms are compiled from source and were not supported initially.

## Installing on Windows XP
* Plug in the device and Windows should recognize it automatically
* Do not connect to Windows Update to search for software if prompted
* "Install from a list or specific location (Advanced)" -> Next
* "Don't search. I will choose the driver to install" -> Next
* "Have Disk..." -> "Browse..." -> Select the xp/x86 subfolder containing the 
*sys* and *inf* file
* Select the device "Cypress EZ-USB (2235) - EEPROM missing" from the list -> 
Next
* That should be it

## Installing on Windows 7, 10
### IMPORATNT: Unsigned drivers
The driver is not signed, so you have to allow loading of unsigned drivers. One 
method to do is this: 
run in cmd.exe as Administrator:
```
bcdedit -set loadoptions DISABLE_INTEGRITY_CHECKS
bcdedit -set TESTSIGNING ON
```
Reboot.

If you don't allow unsigned drivers to load, you can install the driver but
in the device manager, the device will always show up as "not working" and
cannot be used by the game or any of the ezusb tools of BT5.

### Install
* Plug in the device and Windows should recognize it automatically
* "Browse my computer for driver software" -> Next
* "Let me pick from a list of device drivers on my computer" -> Next
* "Have Disk..." -> "Browse..." -> Select your target platform, e.g. Win 7, and
architecture, e.g. x64
* Select the device "Cypress EZ-USB (2235) - EEPROM missing" from the list -> 
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

## Development and Compiling
You need a recent version of Visual Studio, e.g. 2019, to open the source located
in *src*.

To change the target Windows version, right-click on the project -> Properties ->
Driver Settings -> General -> Target OS Platform and change that to your target
platform you want to compile for, e.g. Windows 7. Make sure the project
platform, e.g. x86, x64, as well as the build type, e.g. debug, release, is set
correctly.

### Debugging
You can setup a remote kernel debugger but that's quite a pain in the ass and
might not even be necessary if you can work with printf, aka DbgPrint for
kernel modules. Compile the project in Debug mode and you enable a macro
that will insert the debug prints available in the module.

Use Microsoft's "DebugView" Tool, run it as administrator on the target machine
and enable kernel debugging. Once you plug-in the ezusb board, you should see
log messages of the ezusb.sys module.
