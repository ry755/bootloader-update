# bootloader-update
Update Uzebox V.5.0.xx bootloaders without a hardware programmer  
External SPIRAM is required

**This will only work if your Uzebox is already running Jubatian's V.5.0.xx bootloader**  
**It cannot be used to update from the older 0.4.5 bootloader**

If needed, this can also be used to downgrade to lower V.5.0.xx versions

A very basic form of error recovery is available, if verification fails then it will ask to retry  
If this fails then your Uzebox will be left in an undefined state  
It is *not* possible for a Uzebox to be permanently bricked as a result of using this utility, a hardware flasher can always be used

## Usage
bootloader-update flashes the first 3840 bytes of `blupdate.bin` to 0xF000 - 0xFF00  
`objcopy` can be used to convert Intel hex to a raw binary:  
`avr-objcopy -I ihex Bootloader5.hex -O binary blupdate.bin`

## Demo Video
[![Video demo](https://img.youtube.com/vi/LZz9b3QuU6I/0.jpg)](https://www.youtube.com/watch?v=LZz9b3QuU6I)