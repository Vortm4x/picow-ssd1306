import cv2
import common_util

def gen_header(header_name : str, bin_file_name : str, frame_rate : int, frame_count : int):
    guard_macro = f"{header_name.upper()}_H"
    file_name = f"{header_name}.h"

    header_begin = f"""
#include <stdint.h>
#ifndef {guard_macro}
#define {guard_macro}

#include <stdint.h>
#include <pico/platform.h>
#include "ssd1306_driver.h"

#define VIDEO_FRAME_COUNT {frame_count}
#define VIDEO_FRAME_RATE {frame_rate}

uint8_t __in_flash() {header_name}_compressed_video[] = {r'{'}
"""
    
    header_end = f"""
{r'}'};
    
#endif //{guard_macro}    
"""
    
    header = open(file_name, "w")
    header.write(header_begin)

    bytes = 0
    with open(bin_file_name, "rb") as f:
        while (byte := f.read(1)):
            if bytes % 16 == 0:
                header.write('\n')
                header.write('    ')
            
            header.write(f"0x{int(byte[0]):02x}, ")
            bytes += 1

    header.write(header_end)
    header.close()


def main(header_name : str, bin_file_name : str, frame_rate : int, frame_count : int):
    gen_header(header_name, bin_file_name, frame_rate, frame_count)


if __name__ == '__main__':

    # feel free to edit
    main('bad_apple_hsd', 'bad_apple.bin', 30, 6572)