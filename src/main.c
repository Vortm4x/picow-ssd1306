#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <pico/binary_info.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "ssd1306_driver.h"
#include "raspberry26x32.h"
#include "ssd1306_font.h"


#define SSD1306_I2C_INSTANCE        i2c0
#define SSD1306_I2C_SDA_PIN         16
#define SSD1306_I2C_SCL_PIN         17


typedef struct point_t {
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;
    size_t ram_buffer_len;
} 
point_t;

typedef struct render_area_t {
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;
    size_t ram_buffer_len;
} 
render_area_t;


void init_display();
bool init_all();
void render(uint8_t* ram_buffer, render_area_t* render_area);
void set_pixel(uint8_t* ram_buffer, size_t x, size_t y, bool is_on);
void draw_line(uint8_t* ram_buffer, int x0, int y0, int x1, int y1, bool is_on);
size_t get_font_idx(uint8_t character); 
void write_char(uint8_t* ram_buffer, size_t x, size_t y, uint8_t character);
void write_str(uint8_t* ram_buffer, size_t x, size_t y, char* str);


void init_display()
{
    // set display off
    ssd1306_power_off();

    // memory mapping
    ssd1306_set_mem_mode_horizontal();

    // resolution and layout
    ssd1306_set_display_start_line(0);
    ssd1306_seg_remap_on();
    ssd1306_set_mux_ratio(SSD1306_HEIGHT - 1);
    ssd1306_com_out_remap_on();
    ssd1306_set_display_offset(0);
    ssd1306_set_com_pin_config_alt_remap_off();

    // timing and driving scheme
    ssd1306_set_dclk_div_and_osc_freq(SSD1306_DEFAULT_DCLK_DIV_RATIO, SSD1306_DEFAULT_OSC_FREQ_LEVEL);
    ssd1306_set_precharge_period(1, 15);
    ssd1306_set_vcomh_deselect_level(SSD1306_VCOMH_DESELECT_LVL_0_83);

    // display
    ssd1306_set_contrast(0xFF);
    ssd1306_follow_ram();
    ssd1306_inversion_off();
    ssd1306_charge_pump_on();
    ssd1306_scroll_off();

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
    ssd1306_init_i2c(SSD1306_I2C_INSTANCE, SSD1306_I2C_SDA_PIN, SSD1306_I2C_SCL_PIN);

    init_display();

    return true;
}

void render(uint8_t* ram_buffer, render_area_t* render_area) 
{
    ssd1306_set_col_address(render_area->start_col, render_area->end_col);
    ssd1306_set_page_address(render_area->start_page, render_area->end_page);

    ssd1306_send_data(ram_buffer, render_area->ram_buffer_len);
}

void set_pixel(uint8_t* ram_buffer, size_t x, size_t y, bool is_on) 
{
    assert(x >= 0 && x < SSD1306_WIDTH && y >=0 && y < SSD1306_HEIGHT);

    size_t bytes_per_row = (SSD1306_WIDTH * SSD1306_PAGE_HEIGHT) / 8;
    size_t byte_idx = (y / 8) * bytes_per_row + x;
    size_t bit_no = y % 8;
    
    uint8_t byte = ram_buffer[byte_idx];

    if(is_on)
    {
        byte |=  1 << bit_no;
    }
    else
    {
        byte &= ~(1 << bit_no);
    }

    ram_buffer[byte_idx] = byte;
}

void draw_line(uint8_t* ram_buffer, int x0, int y0, int x1, int y1, bool is_on) 
{

    int dx =  abs(x1-x0);
    int sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0);
    int sy = y0<y1 ? 1 : -1;
    int err = dx+dy;
    int e2;

    while (true) 
    {
        set_pixel(ram_buffer, x0, y0, is_on);

        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        
        e2 = 2 * err;

        if (e2 >= dy) 
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) 
        {
            err += dx;
            y0 += sy;
        }
    }
}

size_t get_font_idx(uint8_t character) 
{
    if (isalpha(character)) 
    {
        return toupper(character) - 'A' + 1;
    }
    else 
    if (isdigit(character)) 
    { 
        return  character - '0' + 27;
    }
    else 
    {
        return  0;
    }
}

void write_char(uint8_t* ram_buffer, size_t x, size_t y, uint8_t character) 
{
    if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
    {
        return;
    }

    // For the moment, only write on Y row boundaries (every 8 vertical pixels)
    
    y /= 8;

    size_t font_idx = get_font_idx(character);
    size_t ram_buffer_idx = y * 128 + x;

    for (size_t i = 0; i < 8; ++i) 
    {
        ram_buffer[ram_buffer_idx++] = font[font_idx * 8 + i];
    }
}

void write_str(uint8_t* ram_buffer, size_t x, size_t y, char* str) 
{
    // Cull out any string off the screen
    if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
    {
        return;
    }

    for(size_t i = 0; str[i] != 0; ++i)
    {
        write_char(ram_buffer, x, y, str[i]);
        x += SSD1306_PAGE_HEIGHT;
    }
}


int main() 
{   
    if(!init_all())
    {
        return -1;
    }

    render_area_t frame_area;
    frame_area.start_col = 0;    
    frame_area.end_col = SSD1306_WIDTH - 1;
    frame_area.start_page = 0;    
    frame_area.end_page = SSD1306_PAGE_COUNT - 1;
    frame_area.ram_buffer_len = SSD1306_RAM_BUFF_SIZE;

    uint8_t ram_buffer[SSD1306_RAM_BUFF_SIZE];
    memset(ram_buffer, 0, SSD1306_RAM_BUFF_SIZE);
    render(ram_buffer, &frame_area);

    for (int i = 0; i < 3; ++i) 
    {
        ssd1306_ignore_ram();
        sleep_ms(500);
        
        ssd1306_follow_ram();
        sleep_ms(500);
    }

    render_area_t picure_area;
    picure_area.start_page = 0;
    picure_area.end_page = (IMG_HEIGHT / SSD1306_PAGE_HEIGHT) - 1;    


    while (true) 
    {
        picure_area.start_col = 0;
        picure_area.end_col = IMG_WIDTH - 1;
        picure_area.ram_buffer_len = IMG_WIDTH * (IMG_HEIGHT / SSD1306_PAGE_HEIGHT);

        uint8_t picture_offset = 5 + IMG_WIDTH;
        for (int i = 0; i < 3; ++i) 
        {
            render(raspberry26x32, &picure_area);
            picure_area.start_col += picture_offset;
            picure_area.end_col += picture_offset;
        }
        
        ssd1306_h_scroll_right_setup(0, 3, SSD1306_SCROLL_FREQ_5);
        ssd1306_scroll_on();
        sleep_ms(5000);
        ssd1306_scroll_off();

        char* text[] = {
            // "A long time ago",
            // "  on an OLED ",
            // "   display",
            // " far far away",
            // "Lived a small",
            // "red raspberry",
            // "by the name of",
            // "    PICO"
            "ABCDEF",
            "GHIJKL",
            "MNOPQR",
            "STUVWX",
            "YZ0123",
            "456789",
        };
        
        size_t y = 0;
        for (size_t i = 0; i < count_of(text); ++i) 
        {
            write_str(ram_buffer, 5, y, text[i]);
            y+=8;
        }
        render(ram_buffer, &frame_area);


        sleep_ms(3000);
        ssd1306_inversion_on();
        sleep_ms(3000);
        ssd1306_inversion_off();
        

        bool pixel_value = true;

        for (size_t i = 0; i < 2; ++i) 
        {
            for (size_t x = 0; x < SSD1306_WIDTH; ++x) 
            {
                draw_line(ram_buffer, x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, pixel_value);
                render(ram_buffer, &frame_area);
            }

            for (int y = SSD1306_HEIGHT - 1; y >= 0; --y) 
            {
                draw_line(ram_buffer, 0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, pixel_value);
                render(ram_buffer, &frame_area);
            }

            pixel_value = false;
        }
    }

    return 0;
}