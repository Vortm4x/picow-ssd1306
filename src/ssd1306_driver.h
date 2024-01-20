#ifndef SSD1306_DRIVER_H
#define SSD1306_DRIVER_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#define SSD1306_HEIGHT              _u(64)
#define SSD1306_WIDTH               _u(128)
#define SSD1306_PAGE_HEIGHT         _u(8)
#define SSD1306_PAGE_COUNT          (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_RAM_BUFF_SIZE       (SSD1306_PAGE_COUNT * SSD1306_WIDTH) 
#define SSD1306_I2C_CLK_FREQ_KHZ    _u(400)


#define SSD1306_COMMAND_HEADER              _u(0x80)
#define SSD1306_DATA_HEADER                 _u(0x40)

// FUNDAMENTAL COMMAND TABLE
#define SSD1306_CMD_SET_CONTRAST_CONTROL        _u(0x81)
#define SSD1306_CMD_ALL_PIXELS_ON               _u(0xA4)
    #define SSD1306_CMD_FLAG_IGNORE_DISPLAY_RAM     _u(0x01)
    #define SSD1306_CMD_FLAG_FOLLOW_DISPLAY_RAM     _u(0x00)
#define SSD1306_CMD_SET_INVERSION_MODE          _u(0xA6)
    #define SSD1306_CMD_FLAG_INVERSION_ON           _u(0x01)
    #define SSD1306_CMD_FLAG_INVERSION_OFF          _u(0x00)
#define SSD1306_CMD_TURN_DISPLAY_POWER          _u(0xAE)
    #define SSD1306_CMD_FLAG_POWER_ON               _u(0x01)
    #define SSD1306_CMD_FLAG_POWER_OFF              _u(0x00)


// SCROLLING COMMAND TABLE
#define SSD1306_CMD_SETUP_H_SCROLL              _u(0x26)
    #define SSD1306_CMD_FLAG_L_SCROLL               _u(0x01)
    #define SSD1306_CMD_FLAG_R_SCROLL               _u(0x00)             
#define SSD1306_CMD_SETUP_VH_SCROLL             _u(0x29)
    #define SSD1306_CMD_FLAG_VL_SCROLL              _u(0x01)
    #define SSD1306_CMD_FLAG_VR_SCROLL              _u(0x02)              
#define SSD1306_CMD_SET_SCROLL_STATUS           _u(0x2E)
    #define SSD1306_CMD_FLAG_START_SCROLL           _u(0x01)
    #define SSD1306_CMD_FLAG_STOP_SCROLL            _u(0x00)
#define SSD1306_CMD_SET_V_SCROLL_AREA           _u(0xA3)

#define SSD1306_SCROLL_FRAME_FREQ_2             _u(0b111)
#define SSD1306_SCROLL_FRAME_FREQ_3             _u(0b100)    
#define SSD1306_SCROLL_FRAME_FREQ_4             _u(0b101)
#define SSD1306_SCROLL_FRAME_FREQ_5             _u(0b000)
#define SSD1306_SCROLL_FRAME_FREQ_25            _u(0b110)
#define SSD1306_SCROLL_FRAME_FREQ_64            _u(0b001)
#define SSD1306_SCROLL_FRAME_FREQ_128           _u(0b010)
#define SSD1306_SCROLL_FRAME_FREQ_256           _u(0b011)


// ADDRESSING SETTING COMMAND TABLE
#define SSD1306_CMD_MASK_PAGE_MODE_START_COL_LOW    _u(0x0F)
#define SSD1306_CMD_MASK_PAGE_MODE_START_COL_HIGH   _u(0x1F)
#define SSD1306_CMD_SET_MEM_ADDR_MODE               _u(0x20)
    #define SSD1306_MEM_ADDR_MODE_HORIZONTAL            _u(0x00)
    #define SSD1306_MEM_ADDR_MODE_VERTICAL              _u(0x01)
    #define SSD1306_MEM_ADDR_MODE_PAGE                  _u(0x02)
    #define SSD1306_MEM_ADDR_MODE_INVALID               _u(0x03)
#define SSD1306_CMD_SET_COL_ADDR                    _u(0x21)
#define SSD1306_CMD_SET_PAGE_ADDR                   _u(0x22)
#define SSD1306_CMD_SET_PAGE_MODE_START_PAGE        _u(0xB0)


// HARDWARE CONFIGURATION COMMAND TABLE
#define SSD1306_CMD_SET_DISPLAY_START_LINE          _u(0x40)
#define SSD1306_CMD_SET_SEG_REMAP                   _u(0xA0)
    #define SSD1306_CMD_FLAG_SEG_REMAP_DISABLE          _u(0x00)
    #define SSD1306_CMD_FLAG_SEG_REMAP_ENABLE           _u(0x01)
#define SSD1306_CMD_SET_MUX_RATIO                   _u(0xA8)
#define SSD1306_CMD_SET_COM_REMAP                   _u(0xC0)
    #define SSD1306_CMD_FLAG_COM_REMAP_DISABLE         _u(0x00)
    #define SSD1306_CMD_FLAG_COM_REMAP_ENABLE          _u(0x08)
#define SSD1306_CMD_SET_COM_OUT_SCAN_DIR            _u(0xC0)
    #define SSD1306_CMD_FLAG_COM_OUT_NORMAL             _u(0x00)
    #define SSD1306_CMD_FLAG_COM_OUT_REMAPPED           _u(0x08)
#define SSD1306_CMD_SET_DISPLAY_OFFSET              _u(0xD3)
#define SSD1306_CMD_SET_COM_PIN_CONFIG              _u(0xDA)
    #define SSD1306_CMD_FLAG_PIN_CONFIG_SEQ             _u(0x00)
    #define SSD1306_CMD_FLAG_PIN_CONFIG_ALT             _u(0x10)
    #define SSD1306_CMD_FLAG_COM_REMAP_OFF              _u(0x00)
    #define SSD1306_CMD_FLAG_COM_REMAP_ON               _u(0x20)


// TIMING & DRIVING SCHEME SETTING COMMAND TABLE
#define SSD1306_CMD_SET_DCLK_AND_FREQ               _u(0xD5)
    #define SSD1306_DEFAULT_OSC_FREQ                    _u(0b1000)
    #define SSD1306_DEFAULT_DCLK_RATIO                  _u(0b0000)

#define SSD1306_CMD_SET_PRE_CHARGE_PERIOD           _u(0xD9)
#define SSD1306_CMD_SET_VCOMH_DESELECT_LVL          _u(0xDB)
    #define SSD1306_VCOMH_DESELECT_LVL_0_65             _u(0x00)
    #define SSD1306_VCOMH_DESELECT_LVL_0_77             _u(0x20)
    #define SSD1306_VCOMH_DESELECT_LVL_0_83             _u(0x30)
#define SSD1306_CMD_NO_OPERATION                    _u(0xE3)


// ADVANCE GRAPHIC COMMAND TABLE
#define SSD1306_CMD_SET_FADE_OUT                    _u(0xD5)
    #define SSD1306_CMD_FLAG_FADE_OUT_OFF               _u(0x00)
    #define SSD1306_CMD_FLAG_FADE_OUT_ON                _u(0x20)
    #define SSD1306_CMD_FLAG_FADE_OUT_BLINK             _u(0x30)
#define SSD1306_CMD_SET_ZOOM_IN                     _u(0xD6)
    #define SSD1306_ZOOM_IN_DISABLE                     _u(0x00)
    #define SSD1306_ZOOM_IN_ENABLE                      _u(0x01)

#define SSD1306_FADE_STEP_FRAMES_8                  _u(0b0000)
#define SSD1306_FADE_STEP_FRAMES_16                 _u(0b0001)
#define SSD1306_FADE_STEP_FRAMES_24                 _u(0b0010)
#define SSD1306_FADE_STEP_FRAMES_32                 _u(0b0011)
#define SSD1306_FADE_STEP_FRAMES_40                 _u(0b0100)
#define SSD1306_FADE_STEP_FRAMES_48                 _u(0b0101)
#define SSD1306_FADE_STEP_FRAMES_56                 _u(0b0110)
#define SSD1306_FADE_STEP_FRAMES_64                 _u(0b0111)
#define SSD1306_FADE_STEP_FRAMES_72                 _u(0b1000)
#define SSD1306_FADE_STEP_FRAMES_80                 _u(0b1001)
#define SSD1306_FADE_STEP_FRAMES_88                 _u(0b1010)
#define SSD1306_FADE_STEP_FRAMES_96                 _u(0b1011)
#define SSD1306_FADE_STEP_FRAMES_104                _u(0b1100)
#define SSD1306_FADE_STEP_FRAMES_112                _u(0b1101)
#define SSD1306_FADE_STEP_FRAMES_120                _u(0b1110)
#define SSD1306_FADE_STEP_FRAMES_128                _u(0b1111)


// CHARGE PUMP COMMAND TABLE
#define SSD1306_CMD_SET_CHARGE_PUMP                 _u(0x8D)
    #define SSD1306_CHARGE_PUMP_DISABLE                 _u(0x10)
    #define SSD1306_CHARGE_PUMP_ENABLE                  _u(0x14)


typedef struct ssd1306_ctx_t
{
    i2c_inst_t* i2c_instance;
    uint8_t i2c_address;
    uint sda_pin;
    uint scl_pin;
    bool is_init;
} 
ssd1306_ctx_t;

typedef struct ssd1306_render_area_t {
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;
    int buflen;
} 
ssd1306_render_area_t;

extern ssd1306_ctx_t ssd1306_ctx;


void ssd1306_init_i2c(
    i2c_inst_t* i2c_instance,  
    uint8_t i2c_address, 
    uint sda_pin, 
    uint scl_pin
);
void ssd1306_deinit_i2c();
void ssd1306_send_cmd(uint8_t cmd);
void ssd1306_send_data(uint8_t data[], size_t data_len);


void ssd1306_set_contrast_control(uint8_t contrast);
void ssd1306_all_pixels_on(bool ignore_ram);
void ssd1306_follow_display_ram();
void ssd1306_ignore_display_ram();
void ssd1306_set_display_inversion(bool display_inverted);
void ssd1306_display_inversion_on();
void ssd1306_display_inversion_off();
void ssd1306_turn_display_power(bool is_on);
void ssd1306_power_on();
void ssd1306_power_off();

void ssd1306_setup_horizontal_scroll_common(
    uint8_t start_page,
    uint8_t end_page,
    uint8_t frame_frequency
);
void ssd1306_setup_horizontal_scroll(
    uint8_t direction_flag,
    uint8_t start_page,
    uint8_t end_page,
    uint8_t frame_frequency
);
void ssd1306_setup_vertical_and_horizontal_scroll(
    uint8_t direction_flag,
    uint8_t start_page,
    uint8_t end_page,
    uint8_t frame_frequency,
    uint8_t row_offset
);
void ssd1306_set_scroll_status(bool enabled);
void ssd1306_start_scrolling();
void ssd1306_stop_scrolling();
void ssd1306_set_vertical_scroll_area(uint8_t start_row, uint8_t row_offset);

void ssd1306_set_page_mode_start_col_high(uint8_t start_col_high);
void ssd1306_set_page_mode_start_col_low(uint8_t start_col_low);
void ssd1306_set_page_mode_start_col(uint8_t start_col);
void ssd1306_set_mem_addr_mode(uint8_t mode_flag);
void ssd1306_set_horizontal_mem_mode();
void ssd1306_set_vertical_mem_mode();
void ssd1306_set_page_mem_mode();
void ssd1306_set_column_addr(uint8_t start_col, uint8_t end_col);
void ssd1306_set_page_addr(uint8_t start_page, uint8_t end_page);
void ssd1306_set_page_mode_start_page(uint8_t start_page);

void ssd1306_set_display_start_line(uint8_t start_line);
void ssd1306_set_seg_remap(bool is_enabled);
void ssd1306_seg_remap_enable();
void ssd1306_seg_remap_disable();
void ssd1306_set_multiplex_ratio(uint8_t mux_ratio);
void ssd1306_set_com_remap(bool is_enabled);
void ssd1306_com_remap_enable();
void ssd1306_com_remap_disable();
void ssd1306_set_com_out_scan_dir(bool is_remapped);
void ssd1306_set_com_out_scan_dir_normal();
void ssd1306_set_com_out_scan_dir_remapped();
void ssd1306_set_display_offset(uint8_t com_offset);
void ssd1306_set_com_pin_config(uint8_t pin_config_flag, uint8_t com_remap_flag);

void ssd1306_set_dclk_and_freq(uint8_t display_clock_div, uint8_t oscillator_freq);
void ssd1306_set_pre_charge_period(uint8_t phase_1, uint8_t phase_2);
void ssd1306_set_pre_charge_period(uint8_t phase_1, uint8_t phase_2);
void ssd1306_set_vcomh_deselect_level(uint8_t deselect_level_flag);
void ssd1306_no_operation();

void ssd1306_set_fade_out_mode(uint8_t fade_out_mode_flag, uint8_t fade_steps);
void ssd1306_set_fade_out_off(uint8_t fade_steps);
void ssd1306_set_fade_out_on(uint8_t fade_steps);
void ssd1306_set_fade_out_blink(uint8_t fade_steps);
void ssd1306_set_zoom_in(bool is_enabled);
void ssd1306_zoom_in_enable();
void ssd1306_zoom_in_disable();

void ssd1306_set_charge_pump(bool is_enabled);
void ssd1306_charge_pump_enable();
void ssd1306_charge_pump_disable();

#endif //SSD1306_DRIVER_H