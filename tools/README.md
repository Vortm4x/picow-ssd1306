# deps
```bash
pip install opencv-python
```

# uncompressed video header generation

```bash
cd tools
python3 gen_frames_header.py
```

# compressed video header generation

```bash
cd tools
python3 gen_uncompressed_binary.py
./heatshrink -e -v -w 8 -l 4 bad_apple.bin bad_apple.heatshrink
python3 gen_compressed_frames_header.py
```

Video should have the same resolution as display have (128x64)
Video data will be located in flash memory of the board (check __in_flash macro)