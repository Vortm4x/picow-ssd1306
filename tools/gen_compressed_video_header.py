import cv2
import common_util
import heatshrink2


def gen_header(
    header_name : str, 
    compressed_bytes : bytearray, 
    frame_rate : int, 
    frame_count : int
) -> None:
  
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
    
    for byte in compressed_bytes:
        if bytes % 16 == 0:
            header.write('\n')
            header.write('    ')
        
        header.write(f"0x{int(byte):02x}, ")
        bytes += 1

    header.write(header_end)
    header.close()



def gen_binary(frames : list[cv2.typing.MatLike]) -> bytearray:
    frame_no = 0
    binary_uncompressed = bytearray()
    
    for frame_no in range(0, len(frames)):

        for line_no in range(0, len(frames[frame_no]), 8):

            for col_no in range(0, len(frames[frame_no][line_no])):
                byte = 0

                for page_line_no in range(0, 8):
                    byte |= common_util.pixel_to_byte(frames[frame_no][line_no + page_line_no][col_no]) << page_line_no

                binary_uncompressed.append(byte)

        print(f"extracting frame {frame_no + 1}/{len(frames)}")

    return binary_uncompressed


def main(
    video_filename : str, 
    header_name : str, 
    frame_rate : int, 
    window_sz2 : int, 
    lookahead_sz2 : int
) -> None:
    
    print("extracting frames...")
    video_frames = common_util.extract_frames(video_filename)

    print("collecting bytes...")
    uncompressed_bytes = gen_binary(video_frames)

    print("compresssing video...")
    compressed_bytes = heatshrink2.compress(
        uncompressed_bytes, 
        window_sz2=window_sz2, 
        lookahead_sz2=lookahead_sz2
    )

    print("generating header...")
    gen_header(
        header_name,
        compressed_bytes,
        frame_rate,
        len(video_frames)
    )

    print("DONE!")



if __name__ == '__main__':
    
    # feel free to edit
    main(
        video_filename='bad_apple.mp4', 
        header_name='bad_apple', 
        frame_rate=30, 
        window_sz2=8, 
        lookahead_sz2=4
    )


