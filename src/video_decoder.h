#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <ctype.h>
#include <string.h>
#include <heatshrink_decoder.h>
#include <heatshrink_config.h>
#include <pico/stdlib.h>


#define DECODER_WINDOW_SZ2          8
#define DECODER_LOOKAHEAD_SZ2       4


typedef struct decoder_context_t
{
    size_t frame_buffer_size;
    size_t video_buffer_size;
    size_t video_bytes_read;
    heatshrink_decoder hsd;
}
decoder_context_t;


void init_decoder_context(decoder_context_t* decoder_context, size_t frame_buffer_size, size_t video_buffer_size);
void reset_decoder_context(decoder_context_t* decoder_context);
bool decode_next_buffer(decoder_context_t* decoder_context, uint8_t frame_buffer[], uint8_t video_buffer[]);

#endif //VIDEO_DECODER_H