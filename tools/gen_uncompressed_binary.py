import cv2
import common_util


def gen_binary(binary_file_name : str, frames : list[cv2.typing.MatLike]):
    frame_no = 0
    binary_file = open(binary_file_name, "wb")

    for frame_no in range(0, len(frames)):

        for line_no in range(0, len(frames[frame_no]), 8):

            for col_no in range(0, len(frames[frame_no][line_no])):
                byte = 0

                for page_line_no in range(0, 8):
                    byte |= common_util.pixel_to_byte(frames[frame_no][line_no + page_line_no][col_no]) << page_line_no

                binary_file.write(bytes(bytearray([byte])))

        print(f"frame {frame_no + 1}/{len(frames)}")

    binary_file.close()


def main(video_file_name : str, binary_file_name : str):
    video_frames = common_util.extract_frames(video_file_name)
    gen_binary(binary_file_name, video_frames)


if __name__ == '__main__':

    # feel free to edit
    main('bad_apple.mp4', 'bad_apple.bin')