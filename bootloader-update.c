/*
 *  bootloader-update
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>
#include <spiram.h>
#include <bootlib.h>

#include "data/tileset.inc"
#include "data/font-8x8-full.inc"

// strings
static const char str_intro_1[]    PROGMEM = "      This utility will";
static const char str_intro_2[]    PROGMEM = "  flash V.5.0.xx bootloaders";
static const char str_intro_3[]    PROGMEM = "      from blupdate.bin";

static const char str_warning_1[]  PROGMEM = "           WARNING:";
static const char str_warning_2[]  PROGMEM = "    This has the potential";
static const char str_warning_3[]  PROGMEM = "  to soft-brick your Uzebox!";

static const char str_no_sd[]      PROGMEM = "Unable to initialize SD card!";
static const char str_no_spiram[]  PROGMEM = "Unable to initialize SPI RAM!";
static const char str_no_file[]    PROGMEM = "blupdate.bin not found!";

static const char str_reading[]    PROGMEM = "     Reading blupdate.bin";
static const char str_writing[]    PROGMEM = "     Writing page 0x";

static const char str_finished_1[] PROGMEM = "      Finished updating!";

static const char str_buttons_1[]  PROGMEM = "        Continue /   Exit";
static const char str_buttons_2[]  PROGMEM = "             Reset";

sdc_struct_t sd_struct;
u8 sd_buf[512];
u8 update_buf[256];

int btn = 0;

int main() {
    ClearVram();
    SetTileTable(tileset);
    SetFontTilesIndex(TILESET_SIZE);

    // initialize SD card
    sd_struct.bufp = &(sd_buf[0]);
    u8 sd_res = FS_Init(&sd_struct);
    if (sd_res != 0) {
        Print(1, 1, str_no_sd);
        while(1);
    }

    // find and open blupdate.bin
    u32 sd_cluster = FS_Find(&sd_struct,
        ((u16)('B') << 8) |
        ((u16)('L')     ),
        ((u16)('U') << 8) |
        ((u16)('P')     ),
        ((u16)('D') << 8) |
        ((u16)('A')     ),
        ((u16)('T') << 8) |
        ((u16)('E')     ),
        ((u16)('B') << 8) |
        ((u16)('I')     ),
        ((u16)('N') << 8) |
        ((u16)(0)       ));
    if (sd_cluster == 0) {
        Print(1, 1, str_no_file);
        while(1);
    }

    if (!SpiRamInit()) { // initialize SPIRAM
        Print(1, 1, str_no_spiram);
        while(1);
    }

    FS_Select_Cluster(&sd_struct, sd_cluster);
    FS_Reset_Sector(&sd_struct);
    FS_Read_Sector(&sd_struct);

    // setup screen layout
    Print(0, 8, str_intro_1);
    Print(0, 9, str_intro_2);
    Print(0, 10, str_intro_3);
    Print(0, 14, str_warning_1);
    Print(0, 15, str_warning_2);
    Print(0, 16, str_warning_3);
    Print(0, 25, str_buttons_1);
    SetTile(6, 25, 1);
    SetTile(19, 25, 2);

    // wait for user to make a choice
    while(1) {
        btn = ReadJoypad(0);
        if (btn & BTN_A)
            break;
        if (btn & BTN_B)
            SoftReset();
    }

    ClearVram();
    Print(0, 12, str_reading);

    // read blupdate.bin into spiram
    for (uint8_t sector = 0; sector < 8; sector++) {
        for (uint16_t byte = 0; byte < 512; byte++) {
            SpiRamWriteU8(0, byte+(sector*512), sd_buf[byte]);
        }
        FS_Next_Sector(&sd_struct);
        FS_Read_Sector(&sd_struct);
    }

    ClearVram();
    Print(0, 12, str_writing);

    // write new bootloader to flash
    for (uint16_t flash_page = 0xF0; flash_page < 0xFF; flash_page++) {
        // fill buffer with file contents for this page
        for (uint16_t byte = 0; byte < 256; byte++) {
            update_buf[byte] = SpiRamReadU8(0, byte+((flash_page-0xF0)*256));
        }
        PrintHexInt(20, 12, flash_page*256);
        WaitVsync(1);
        // write 256 byte flash pages
        asm volatile(
            "mov r24, %A0" "\n\t"
            "mov r25, %B0" "\n\t"
            "mov r22, %1"  "\n\t"
            "call 0xFFAA"  "\n" :: // call Prog_Page
            "e" (&update_buf),
            "r" (flash_page) :
            "r0", "r1", "r25", "r26", "r27", "r30", "r31"
        );
    }

    ClearVram();
    Print(0, 12, str_finished_1);
    Print(0, 25, str_buttons_2);
    SetTile(11, 25, 1);

    // wait for user to press A
    while(1) {
        btn = ReadJoypad(0);
        if (btn & BTN_A)
            SoftReset();
    }
}