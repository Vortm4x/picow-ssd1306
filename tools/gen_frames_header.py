import cv2
import common_util


def gen_header(header_name : str, frames : list[cv2.typing.MatLike], frame_rate : int) -> None:

    frame_count = len(frames)
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

uint8_t __in_flash() {header_name}_video_frames[VIDEO_FRAME_COUNT][SSD1306_RAM_BUFF_SIZE] = {r'{'}
"""
    
    header_end = f"""
{r'}'};
    
#endif //{guard_macro}    
"""

    header = open(file_name, "w")
    header.write(header_begin)

    for frame_no in range(0, frame_count):
        header.write("   {\n")
        
        for line_no in range(0, len(frames[frame_no]), 8):
            header.write('        ')

            for col_no in range(0, len(frames[frame_no][line_no])):
                byte = 0

                for page_line_no in range(0, 8):
                    byte |= common_util.pixel_to_byte(frames[frame_no][line_no + page_line_no][col_no]) << page_line_no

                header.write(f"0x{byte:02x}, ")

            header.write('\n')

        header.write("   },\n")

        print(f"extracting frame {frame_no + 1}/{len(frames)}")

    header.write(header_end)
    header.close()


def main(video_filename : str, header_name : str, frame_rate : int) -> None:
    
    print("extracting frames...")
    video_frames = common_util.extract_frames(video_filename)

    print("generating header...")
    gen_header(header_name, video_frames, frame_rate)

    print("DONE!")



if __name__ == '__main__':

    # feel free to edit
    main(
        video_filename='bad_apple.mp4', 
        header_name='bad_apple', 
        frame_rate=30, 
    )