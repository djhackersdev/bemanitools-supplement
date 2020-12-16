# Driver for IIDX BIO2
Currently, this package offers drivers for the following platforms:
* Windows 7: x64 (confirmed to work on Windows 10)

### Install
* Plug in the device and Windows should recognize it automatically
* Windows will install a default driver that works fine when using
BT5's bio2 driver, e.g. `iidxio-bio2.dll`. However, if you want
to use the BIO2 with any version starting 25 without BT5 or its
emulation layer, the board cannot be detected resulting in an IO
error (due to a mismatch in the drivers name).
* Go to the device manager, select the COM port the BIO2 is enumerated
as -> Right click -> "Update driver"
* "Browse my computer for driver software" -> Next
* "Let me pick from a list of device drivers on my computer" -> Next
* "Have Disk..." -> "Browse..." -> Select the driver inf file
* Select the device "KAM BIO2(VIDEO) CDC DRIVER" from the list -> 
Next
* That should be it

## Testing
Instead of using the games, use one of the tools included with BT5 to test and
debug issues as it will error on more fine granular level. Furthermore, you
can also check the source code for details and to track down any unknown issues.