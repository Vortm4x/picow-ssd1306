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
#include "video_decoder.h"

#define SSD1306_I2C_INSTANCE        i2c0
#define SSD1306_I2C_SDA_PIN         16
#define SSD1306_I2C_SCL_PIN         17
#define SSD1306_I2C_ADDRESS         _u(0x3C)


void init_display();
bool init_all();
void render(uint8_t ram_buffer[]);


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