# Firmware binaries Ezusb (legacy), aka C02 IO
Firmware binary blobs. Requires appropriate tools from BT5 to load them to the
hardware. There are two revisions of firmware images, **v1** and **v2**. 

v1 was used on IIDX 9 to 13 and v2 on IIDX 14 to 24. It does not matter
which revision of the base firmware and FPGA image (always in combination!) are
used when running the real hardware with BT5. Still, we recommend using the v2
binaries as they are, from a development standpoint, the latest "update". Do
*NOT* mix v1 and v2 binaries.