# Firmware binaries Ezusb (legacy): C02 and D01 IO
Firmware binary blobs. Requires appropriate tools from BT5 to load them to the
hardware. There are two revisions of firmware images, **v1** and **v2**. 

Firmware types:
* `ezusb_XX.bin`: Base firmware which always needs to be uploaded to the board first.
* `fpga_XX.bin`: FPGA firmware, needs to be uploaded after the base firmware.
* `sram.bin`: SRAM dump with contents that need to be uploaded as well, only
relevant when using **v2** files.

**v1** was used on IIDX 9 to 13 and **v2** on IIDX 14 to 24. It does not matter
which revision of the base firmware and FPGA image as long as you do not mix
different versions.

Still, we recommend using the **v2** binaries as they are, from a development
standpoint, the latest "update".

Exception for D01 boards: Use the `ezusb_v1.bin` and `fpga_v1.bin`, only.