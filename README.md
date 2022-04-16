# bootloader-update
Update Uzebox V.5.0.xx bootloaders without a hardware programmer  
External SPIRAM is required

**This will only work if your Uzebox is already running Jubatian's V.5.0.xx bootloader**  
**It cannot be used to update from the older 0.4.5 bootloader**

If needed, this can also be used to downgrade to lower V.5.0.xx versions.

A very basic form of error recovery is available, if verification fails then it will ask to retry.  
If this fails then your Uzebox will be left in an undefined state.  
This utility has been tested many times and has always worked for me, but in the event that something fails, a hardware flasher can always be used.

## Usage
bootloader-update flashes the first 3840 bytes of `blupdate.bin` from the SD card to the bootloader section in memory.  
`objcopy` can be used to convert Intel hex to a raw binary:  
`avr-objcopy -I ihex Bootloader5.hex -O binary blupdate.bin`

## Demo Video
[![Video demo](https://img.youtube.com/vi/LZz9b3QuU6I/0.jpg)](https://www.youtube.com/watch?v=LZz9b3QuU6I)