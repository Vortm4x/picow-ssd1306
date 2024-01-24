#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <pico/binary_info.h>
#include <pico/platform.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "ssd1306_driver.h"
#include "bad_apple.h"
#include "heatshrink_decoder.h"
#include "heatshrink_config.h"


#define SSD1306_I2C_INSTANCE        i2c0
#define SSD1306_I2C_SDA_PIN         16
#define SSD1306_I2C_SCL_PIN         17
#define SSD1306_I2C_ADDRESS         _u(0x3C)

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


void init_display();
bool init_all();
void render(uint8_t ram_buffer[]);
void init_decoder_context(decoder_context_t* decoder_context, size_t frame_buffer_size, size_t video_buffer_size);
void reset_decoder_context(decoder_context_t* decoder_context);
bool decode_next_buffer(decoder_context_t* decoder_context, uint8_t frame_buffer[], uint8_t video_buffer[]);


void init_display()
{
    // set display off
    ssd1306_power_off();

    // memory mapping
    ssd1306_set_horizontal_mem_mode();

    // resolution and layout
    ssd1306_set_display_start_line(0);
    ssd1306_seg_remap_enable();
    ssd1306_set_multiplex_ratio(SSD1306_HEIGHT - 1);
    ssd1306_set_com_out_scan_dir_remapped();
    ssd1306_set_display_offset(0);
    ssd1306_set_com_pin_config(SSD1306_CMD_FLAG_PIN_CONFIG_ALT, SSD1306_CMD_FLAG_COM_REMAP_OFF);

    // timing and driving scheme
    ssd1306_set_dclk_and_freq(SSD1306_DEFAULT_DCLK_RATIO, SSD1306_DEFAULT_OSC_FREQ);
    ssd1306_set_pre_charge_period(1, 15);
    ssd1306_set_vcomh_deselect_level(SSD1306_VCOMH_DESELECT_LVL_0_83);

    // display
    ssd1306_set_contrast_control(0xFF);
    ssd1306_follow_display_ram();
    ssd1306_display_inversion_off();
    ssd1306_charge_pump_enable();
    ssd1306_stop_scrolling();

    // set display on
    ssd1306_power_on();
}

bool init_all()
{
    if(!stdio_init_all())
    {
        printf("stdio init failed\n");
        return false;
    }
    
    if (cyw43_arch_init() != PICO_OK) 
    {
        printf("CYW43 arch init failed\n");
        return false;
    }

    bi_decl(bi_2pins_with_func(SSD1306_I2C_SDA_PIN, SSD1306_I2C_SCL_PIN, GPIO_FUNC_I2C));
    ssd1306_init_i2c(SSD1306_I2C_INSTANCE, SSD1306_I2C_ADDRESS, SSD1306_I2C_SDA_PIN, SSD1306_I2C_SCL_PIN);

    init_display();

    return true;
}

void render(uint8_t ram_buffer[]) 
{
    ssd1306_set_column_addr(0, SSD1306_WIDTH - 1);
    ssd1306_set_page_addr(0, SSD1306_PAGE_COUNT - 1);

    ssd1306_send_data(ram_buffer, SSD1306_RAM_BUFF_SIZE);
}

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

int main() 
{   
    if(!init_all())
    {
        return -1;
    }

    uint8_t ram_buffer[SSD1306_RAM_BUFF_SIZE];
    decoder_context_t decoder_context;

    memset(ram_buffer, 0, SSD1306_RAM_BUFF_SIZE);
    init_decoder_context(&decoder_context, SSD1306_RAM_BUFF_SIZE, sizeof(compressed_video_buffer));


    while (true) 
    {
        render(video_title);
        sleep_ms(3000);
        
        absolute_time_t abs_time = get_absolute_time();

        while(decode_next_buffer(&decoder_context, ram_buffer, compressed_video_buffer))
        {            
            render(ram_buffer);

            abs_time._private_us_since_boot += 1000000 / VIDEO_FRAME_RATE;
            sleep_until(abs_time);

            abs_time = get_absolute_time();
        }
                
        reset_decoder_context(&decoder_context);
    }

    return 0;
}