# dependencies

```bash
pip install opencv-python heatshrink2
```

# uncompressed video header generation

```bash
cd tools
python3 gen_frames_header.py
```

Generates 2d byte array of frame buffers


# compressed video header generation

```bash
cd tools
python3 gen_compressed_video_header.py
```

Generates compressed byte array, that should be decompressed at runtime.

# requirements

Video should have the same resolution as display have (128x64)
Video data will be located in flash memory of the board (check __in_flash macro)