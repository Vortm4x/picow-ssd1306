#include "video_decoder.h"


void init_decoder_context(decoder_context_t* decoder_context, size_t frame_buffer_size, size_t video_buffer_size)
{
    memset(decoder_context, 0, sizeof(decoder_context_t));

    decoder_context->frame_buffer_size = frame_buffer_size;
    decoder_context->video_buffer_size = video_buffer_size;
    reset_decoder_context(decoder_context);
}

void reset_decoder_context(decoder_context_t* decoder_context)
{
    decoder_context->video_bytes_read = 0;
    heatshrink_decoder_reset(&decoder_context->hsd);
}

bool decode_next_buffer(decoder_context_t* decoder_context, uint8_t frame_buffer[], uint8_t video_buffer[])
{
    if(decoder_context->video_buffer_size == decoder_context->video_bytes_read)
    {
        if(heatshrink_decoder_finish(&decoder_context->hsd) != HSDR_FINISH_MORE)
        {
            return false;
        }
    }

    size_t frame_bytes_read = 0;

    while(decoder_context->frame_buffer_size != frame_bytes_read)
    {
        size_t bytes_to_sink = MIN(
            decoder_context->video_buffer_size - decoder_context->video_bytes_read, 
            HEATSHRINK_STATIC_INPUT_BUFFER_SIZE
        );
        size_t bytes_to_poll = decoder_context->frame_buffer_size - frame_bytes_read;
        size_t bytes_sunk = 0;
        size_t bytes_polled = 0;

        if(bytes_to_sink > 0)
        {
            heatshrink_decoder_sink(
                &decoder_context->hsd,
                &video_buffer[decoder_context->video_bytes_read], 
                bytes_to_sink,
                &bytes_sunk
            );
            decoder_context->video_bytes_read += bytes_sunk;
        }

        heatshrink_decoder_poll(
            &decoder_context->hsd, 
            &frame_buffer[frame_bytes_read], 
            bytes_to_poll, 
            &bytes_polled
        );
        frame_bytes_read += bytes_polled;
    }

    return true;
}