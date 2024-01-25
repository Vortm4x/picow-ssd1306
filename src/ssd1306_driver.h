#ifndef SSD1306_DRIVER_H
#define SSD1306_DRIVER_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#define SSD1306_HEIGHT                          _u(64)
#define SSD1306_WIDTH                           _u(128)
#define SSD1306_PAGE_HEIGHT                     _u(8)
#define SSD1306_PAGE_COUNT                      (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_RAM_BUFF_SIZE                   (SSD1306_PAGE_COUNT * SSD1306_WIDTH) 
#define SSD1306_I2C_CLK_FREQ_KHZ                _u(400)
#define SSD1306_I2C_ADDRESS                     _u(0x3C)
#define SSD1306_MIN_MUX_RATIO                   _u(0x0F)
#define SSD1306_MAX_MUX_RATIO                   _u(0x3F)
#define SSD1306_MAX_DCLK_DIV_RATIO              _u(15)
#define SSD1306_MAX_OSC_FREQ_LEVEL              _u(15)
#define SSD1306_MAX_PRECHARGE_PHASE_PERIOD      _u(15)
#define SSD1306_DEFAULT_DCLK_DIV_RATIO          _u(0)
#define SSD1306_DEFAULT_OSC_FREQ_LEVEL          _u(8)
#define SSD1306_DEFAULT_PRECHARGE_PHASE_PERIOD  _u(2)


typedef enum ssd1306_err_t
{
    SSD1306_ERR_OK,
    SSD1306_ERR_INITIALIZED,
    SSD1306_ERR_DEINITIALIZED,
    SSD1306_ERR_I2C_INSTANCE_INVALID,
    SSD1306_ERR_SDA_PIN_INVALID,
    SSD1306_ERR_SCL_PIN_INVALID,
    SSD1306_ERR_PINS_DUPLICATED,
    SSD1306_ERR_PICO_I2C_TIMEOUT,
    SSD1306_ERR_PICO_ERROR_GENERIC,
    SSD1306_ERR_ZERO_LEN_DATA,
    SSD1306_ERR_NULL_DATA,
    SSD1306_ERR_INVALID_PAGE,
    SSD1306_ERR_INVALID_ROW,
    SSD1306_ERR_INVALID_SCROLL_FREQ,
    SSD1306_ERR_INVALID_PAGE_BOUNDS,
    SSD1306_ERR_INVALID_HSCROLL_OFFSET,
    SSD1306_ERR_INVALID_HSCROLL_POSITION,
    SSD1306_ERR_INVALID_HSCROLL_HEIGHT,
    SSD1306_ERR_INVALID_COLUMN,
    SSD1306_ERR_INVALID_COLUMN_BOUNDS,
    SSD1306_ERR_INVALID_MUX_RATIO,
    SSD1306_ERR_INVALID_DCLK_DIV_RATIO,
    SSD1306_ERR_INVALID_OSC_FREQ_LEVEL,
    SSD1306_ERR_INVALID_PRECHARGE_PHASE_PERIOD,
    SSD1306_ERR_INVALID_VCOMH_DESELECT_LEVEL,
    SSD1306_ERR_INVALID_FADEOUT_MODE,
    SSD1306_ERR_INVALID_FADEOUT_FREQ,
}
ssd1306_err_t;

typedef enum ssd1306_scroll_freq_t
{
    SSD1306_SCROLL_FREQ_2       = _u(0b111),
    SSD1306_SCROLL_FREQ_3       = _u(0b100),    
    SSD1306_SCROLL_FREQ_4       = _u(0b101),
    SSD1306_SCROLL_FREQ_5       = _u(0b000),
    SSD1306_SCROLL_FREQ_25      = _u(0b110),
    SSD1306_SCROLL_FREQ_64      = _u(0b001),
    SSD1306_SCROLL_FREQ_128     = _u(0b010),
    SSD1306_SCROLL_FREQ_256     = _u(0b011),
    SSD1306_SCROLL_FREQ_COUNT   = _u(8),
}
ssd1306_scroll_freq_t;

typedef enum ssd1306_vcomh_t
{
    SSD1306_VCOMH_DESELECT_LVL_0_65 = _u(0x00),
    SSD1306_VCOMH_DESELECT_LVL_0_77 = _u(0x20),
    SSD1306_VCOMH_DESELECT_LVL_0_83 = _u(0x30),
}
ssd1306_vcomh_t;

typedef enum ssd1306_fade_out_mode_t
{
    SSD1306_FADE_OUT_MODE_OFF   = _u(0x00),
    SSD1306_FADE_OUT_MODE_FADE  = _u(0x20),
    SSD1306_FADE_OUT_MODE_BLINK = _u(0x30),
}
ssd1306_fade_out_mode_t;

typedef enum ssd1306_fade_out_freq_t
{
    SSD1306_FADE_OUT_FREQ_8     = _u(0b0000),
    SSD1306_FADE_OUT_FREQ_16    = _u(0b0001),
    SSD1306_FADE_OUT_FREQ_24    = _u(0b0010),
    SSD1306_FADE_OUT_FREQ_32    = _u(0b0011),
    SSD1306_FADE_OUT_FREQ_40    = _u(0b0100),
    SSD1306_FADE_OUT_FREQ_48    = _u(0b0101),
    SSD1306_FADE_OUT_FREQ_56    = _u(0b0110),
    SSD1306_FADE_OUT_FREQ_64    = _u(0b0111),
    SSD1306_FADE_OUT_FREQ_72    = _u(0b1000),
    SSD1306_FADE_OUT_FREQ_80    = _u(0b1001),
    SSD1306_FADE_OUT_FREQ_88    = _u(0b1010),
    SSD1306_FADE_OUT_FREQ_96    = _u(0b1011),
    SSD1306_FADE_OUT_FREQ_104   = _u(0b1100),
    SSD1306_FADE_OUT_FREQ_112   = _u(0b1101),
    SSD1306_FADE_OUT_FREQ_120   = _u(0b1110),
    SSD1306_FADE_OUT_FREQ_128   = _u(0b1111),
    SSD1306_FADE_OUT_FREQ_COUNT = _u(16),
}
ssd1306_fade_out_freq_t;

ssd1306_err_t ssd1306_init_i2c(
    i2c_inst_t* i2c_instance,  
    uint sda_pin, 
    uint scl_pin
);
ssd1306_err_t ssd1306_deinit_i2c();

ssd1306_err_t ssd1306_send_data(
    uint8_t data[], 
    size_t data_len
);


ssd1306_err_t ssd1306_set_contrast(
    uint8_t contrast
);
ssd1306_err_t ssd1306_follow_ram();
ssd1306_err_t ssd1306_ignore_ram();
ssd1306_err_t ssd1306_inversion_off();
ssd1306_err_t ssd1306_inversion_on();
ssd1306_err_t ssd1306_power_off();
ssd1306_err_t ssd1306_power_on();


ssd1306_err_t ssd1306_h_scroll_right_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq
);
ssd1306_err_t ssd1306_h_scroll_left_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq
);
ssd1306_err_t ssd1306_vh_scroll_right_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq,
    uint8_t row_offset
);
ssd1306_err_t ssd1306_vh_scroll_left_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq,
    uint8_t row_offset
);
ssd1306_err_t ssd1306_scroll_off();
ssd1306_err_t ssd1306_scroll_on();


ssd1306_err_t ssd1306_set_page_mode_start_col(
    uint8_t col
);
ssd1306_err_t ssd1306_set_mem_mode_horizontal();
ssd1306_err_t ssd1306_set_mem_mode_vertical();
ssd1306_err_t ssd1306_set_mem_mode_page();
ssd1306_err_t ssd1306_set_col_address(
    uint8_t col_start, 
    uint8_t col_end
);
ssd1306_err_t ssd1306_set_page_address(
    uint8_t page_start, 
    uint8_t page_end
);
ssd1306_err_t ssd1306_set_page_mode_start_page(
    uint8_t page
);


ssd1306_err_t ssd1306_set_display_start_line(
    uint8_t row
);
ssd1306_err_t ssd1306_seg_remap_off();
ssd1306_err_t ssd1306_seg_remap_on();
ssd1306_err_t ssd1306_set_mux_ratio(
    uint8_t mux_ratio
);
ssd1306_err_t ssd1306_com_out_remap_off();
ssd1306_err_t ssd1306_com_out_remap_on();
ssd1306_err_t ssd1306_set_display_offset(
    uint8_t row
);
ssd1306_err_t ssd1306_set_com_pin_config_seq_remap_off();
ssd1306_err_t ssd1306_set_com_pin_config_alt_remap_off();
ssd1306_err_t ssd1306_set_com_pin_config_seq_remap_on();
ssd1306_err_t ssd1306_set_com_pin_config_alt_remap_on();


ssd1306_err_t ssd1306_set_dclk_div_and_osc_freq(
    uint8_t dclk_div_ratio,
    uint8_t osc_freq_level
);
ssd1306_err_t ssd1306_set_precharge_period(
    uint8_t phase1_peroid,
    uint8_t phase2_peroid
);
ssd1306_err_t ssd1306_set_vcomh_deselect_level(
    ssd1306_vcomh_t deselect_level 
);
ssd1306_err_t ssd1306_no_operation();


ssd1306_err_t ssd1306_set_fade_out_mode(
    ssd1306_fade_out_mode_t mode, 
    ssd1306_fade_out_freq_t freq
);
ssd1306_err_t ssd1306_zoom_in_off();
ssd1306_err_t ssd1306_zoom_in_on();

ssd1306_err_t ssd1306_charge_pump_off();
ssd1306_err_t ssd1306_charge_pump_on();

#endif //SSD1306_DRIVER_H