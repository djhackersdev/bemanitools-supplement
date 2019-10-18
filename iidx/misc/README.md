# Misc stuff

* <u>CLVSD.ax</u>: Video codec required to play background videos from IIDX 12
onwards.
Install: Start -> Run -> regsvr32 clvsd.ax
* <u>libacio_20_slot.dll</u>: libacio implementation from IIDX 20 which supports
slotted readers. Thank you Konami for leaving this on the drive.
* <u>RtEffect_stub.dll</u>: Required for IIDX 10 to 13 when running the games on 
hardware that does not have an analog version of a Realtek integrated sound
chip. Replace the existing RtEffect.dll in the game folder with this one.
Otherwise, the game won't boot (instantaneous crash).
