# Firmware binaries Ezusb (legacy): C02 and D01 IO
Firmware binary blobs. Requires appropriate tools from BT5 to load them to the
hardware. Pick the binary blobs according to your hardware.

Important: Do not mix different binary blobs from different packages.

## C02-v1
Base firmware (`ezusb.bin`) and FPGA program (`fpga.bin`) which were used on IIDX 9 to 13 with the
C02 IO board.

## C02-v2
Base firmware (`ezusb.bin`), FPGA program (`fpga.bin`) and SRAM contents (`sram.bin`) that are
written to the C02 IO board on IIDX 14 to 24. Note that all three binary images need to be written
to the IO using appropriate tooling. 

There have been reports that not all C02 IO boards support the v2 binary data set (apparently, 
there are different revisions of the board). Therefore, if v2 doesn't work for you, fallback to the
v1 binary set.

## D01
Base firmware (`ezusb.bin`) and FPGA program (`fpga.bin`) which were used on IIDX 9 to 13 with the
D01 IO board.
