import cv2 

def pixel_to_byte(pixel):
    grey = (int(pixel[0]) + int(pixel[1]) + int(pixel[2])) // 3
    if grey > 0xF:
        return 0x1
    else:
        return 0x0


def extract_frames(video_path : str) -> list[cv2.typing.MatLike]: 

    video = cv2.VideoCapture(video_path)
 
    frames = []
    success, frame = video.read()
     
    while success:
        frames.append(frame)
        success, frame = video.read()
 
    video.release()

    return frames
