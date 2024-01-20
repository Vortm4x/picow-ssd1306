#include "ssd1306_driver.h"

ssd1306_ctx_t ssd1306_ctx = {};


void ssd1306_init_i2c(
    i2c_inst_t* i2c_instance,  
    uint8_t i2c_address, 
    uint sda_pin, 
    uint scl_pin
)
{
    if(ssd1306_ctx.is_init) return;

    memset(&ssd1306_ctx, 0, sizeof(ssd1306_ctx_t));
    ssd1306_ctx.i2c_instance = i2c_instance;
    ssd1306_ctx.i2c_address = i2c_address;
    ssd1306_ctx.sda_pin = sda_pin;
    ssd1306_ctx.sda_pin = scl_pin;
    ssd1306_ctx.is_init = true;

    i2c_init(i2c_instance, SSD1306_I2C_CLK_FREQ_KHZ * 1000);

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
}

void ssd1306_deinit_i2c()
{
    i2c_deinit(ssd1306_ctx.i2c_instance);
    gpio_deinit(ssd1306_ctx.scl_pin);
    gpio_deinit(ssd1306_ctx.sda_pin);

    memset(&ssd1306_ctx, 0, sizeof(ssd1306_ctx_t));
}

void ssd1306_send_cmd(uint8_t command)
{
    if(!ssd1306_ctx.is_init) return;

    uint8_t command_buffer[2] = { SSD1306_COMMAND_HEADER, command };

    i2c_write_blocking(ssd1306_ctx.i2c_instance, ssd1306_ctx.i2c_address, command_buffer, 2, false);
}

void ssd1306_send_data(uint8_t data[], size_t data_len)
{
    uint8_t *temp_buf = malloc(data_len + 1);

    temp_buf[0] = SSD1306_DATA_HEADER;
    memcpy(temp_buf + 1, data, data_len);

    i2c_write_blocking(i2c_default, ssd1306_ctx.i2c_address, temp_buf, data_len + 1, false);

    free(temp_buf);
}


void ssd1306_set_contrast_control(uint8_t contrast)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_CONTRAST_CONTROL);
    ssd1306_send_cmd(contrast);
}

void ssd1306_all_pixels_on(bool ignore_ram)
{
    if(ignore_ram)
    {
        ssd1306_send_cmd(SSD1306_CMD_ALL_PIXELS_ON | SSD1306_CMD_FLAG_IGNORE_DISPLAY_RAM);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_CMD_ALL_PIXELS_ON | SSD1306_CMD_FLAG_FOLLOW_DISPLAY_RAM);
    }
}

void ssd1306_ignore_display_ram()
{
    ssd1306_all_pixels_on(true);
}

void ssd1306_follow_display_ram()
{
    ssd1306_all_pixels_on(false);
}

void ssd1306_set_display_inversion(bool display_inverted)
{
    if(display_inverted)
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_INVERSION_MODE | SSD1306_CMD_FLAG_INVERSION_ON);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_INVERSION_MODE | SSD1306_CMD_FLAG_INVERSION_OFF);
    }
}

void ssd1306_display_inversion_on()
{
    ssd1306_set_display_inversion(true);
}

void ssd1306_display_inversion_off()
{
    ssd1306_set_display_inversion(false);
}

void ssd1306_turn_display_power(bool is_on)
{
    if(is_on)
    {
        ssd1306_send_cmd(SSD1306_CMD_TURN_DISPLAY_POWER | SSD1306_CMD_FLAG_POWER_ON);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_CMD_TURN_DISPLAY_POWER | SSD1306_CMD_FLAG_POWER_OFF);
    }
}

void ssd1306_power_on()
{
    ssd1306_turn_display_power(true);
}

void ssd1306_power_off()
{
    ssd1306_turn_display_power(false);
}


void ssd1306_setup_horizontal_scroll_common(
    uint8_t start_page,
    uint8_t end_page,
    uint8_t frame_frequency
)
{
    ssd1306_send_cmd(0x00);
    ssd1306_send_cmd(start_page);
    ssd1306_send_cmd(frame_frequency);
    ssd1306_send_cmd(end_page);
}

void ssd1306_setup_horizontal_scroll(
    uint8_t direction_flag,
    uint8_t start_page,
    uint8_t end_page,
    uint8_t frame_frequency
)
{
    ssd1306_send_cmd(SSD1306_CMD_SETUP_H_SCROLL | direction_flag);
    ssd1306_setup_horizontal_scroll_common(start_page, end_page, frame_frequency);
    ssd1306_send_cmd(0x00);
    ssd1306_send_cmd(0xFF);
}

void ssd1306_setup_vertical_and_horizontal_scroll(
    uint8_t direction_flag,
    uint8_t start_page,
    uint8_t end_page,
    uint8_t frame_frequency,
    uint8_t row_offset
)
{
    ssd1306_send_cmd(SSD1306_CMD_SETUP_VH_SCROLL | direction_flag);
    ssd1306_setup_horizontal_scroll_common(start_page, end_page, frame_frequency);
    ssd1306_send_cmd(row_offset);
}
void ssd1306_set_scroll_status(bool enabled)
{
    if(enabled)
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_SCROLL_STATUS | SSD1306_CMD_FLAG_START_SCROLL);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_SCROLL_STATUS | SSD1306_CMD_FLAG_STOP_SCROLL);
    }    
}
void ssd1306_start_scrolling()
{
    ssd1306_set_scroll_status(true);
}
void ssd1306_stop_scrolling()
{
    ssd1306_set_scroll_status(false);
}
void ssd1306_set_vertical_scroll_area(
    uint8_t start_row,
    uint8_t row_offset
)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_V_SCROLL_AREA);
    ssd1306_send_cmd(start_row);
    ssd1306_send_cmd(row_offset);
}

void ssd1306_set_page_mode_start_col_low(uint8_t start_col_low)
{
    ssd1306_send_cmd(SSD1306_CMD_MASK_PAGE_MODE_START_COL_LOW & start_col_low);
}

void ssd1306_set_page_mode_start_col_high(uint8_t start_col_high)
{
    ssd1306_send_cmd(SSD1306_CMD_MASK_PAGE_MODE_START_COL_HIGH & start_col_high);
}

void ssd1306_set_page_mode_start_col(uint8_t start_col)
{
    ssd1306_set_page_mode_start_col_low(start_col >> 0);
    ssd1306_set_page_mode_start_col_high(start_col >> 4);
}

void ssd1306_set_mem_addr_mode(uint8_t mode_flag)
{
    switch (mode_flag)
    {
        case SSD1306_MEM_ADDR_MODE_HORIZONTAL:
        case SSD1306_MEM_ADDR_MODE_VERTICAL:
        case SSD1306_MEM_ADDR_MODE_PAGE:
            ssd1306_send_cmd(SSD1306_CMD_SET_MEM_ADDR_MODE);
            ssd1306_send_cmd(mode_flag);
            break;
        default:
            break;
    }
}

void ssd1306_set_horizontal_mem_mode()
{
    ssd1306_set_mem_addr_mode(SSD1306_MEM_ADDR_MODE_HORIZONTAL);
}

void ssd1306_set_vertical_mem_mode()
{
    ssd1306_set_mem_addr_mode(SSD1306_MEM_ADDR_MODE_VERTICAL);
}

void ssd1306_set_page_mem_mode()
{
    ssd1306_set_mem_addr_mode(SSD1306_MEM_ADDR_MODE_PAGE);
}

void ssd1306_set_column_addr(uint8_t start_col, uint8_t end_col)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_COL_ADDR);
    ssd1306_send_cmd(start_col);
    ssd1306_send_cmd(end_col);
}

void ssd1306_set_page_addr(uint8_t start_page, uint8_t end_page)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_PAGE_ADDR);
    ssd1306_send_cmd(start_page);
    ssd1306_send_cmd(end_page);
}

void ssd1306_set_page_mode_start_page(uint8_t start_page)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_PAGE_MODE_START_PAGE | start_page);
}


void ssd1306_set_display_start_line(uint8_t start_line)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_DISPLAY_START_LINE | start_line);
}

void ssd1306_set_seg_remap(bool is_enabled)
{
    if(is_enabled)
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_SEG_REMAP | SSD1306_CMD_FLAG_SEG_REMAP_ENABLE);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_SEG_REMAP | SSD1306_CMD_FLAG_SEG_REMAP_DISABLE);
    }
}

void ssd1306_seg_remap_enable()
{
    ssd1306_set_seg_remap(true);
}

void ssd1306_seg_remap_disable()
{
    ssd1306_set_seg_remap(false);
}

void ssd1306_set_multiplex_ratio(uint8_t mux_ratio)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_MUX_RATIO);
    ssd1306_send_cmd(mux_ratio);
}

void ssd1306_set_com_remap(bool is_enabled)
{
    if(is_enabled)
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_COM_REMAP | SSD1306_CMD_FLAG_COM_REMAP_ENABLE);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_COM_REMAP | SSD1306_CMD_FLAG_COM_REMAP_DISABLE);
    }
}

void ssd1306_com_remap_enable()
{
    ssd1306_set_com_remap(true);
}

void ssd1306_com_remap_disable()
{
    ssd1306_set_com_remap(false);
}

void ssd1306_set_com_out_scan_dir(bool is_remapped)
{
    if(is_remapped)
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_COM_OUT_SCAN_DIR | SSD1306_CMD_FLAG_COM_OUT_REMAPPED);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_CMD_SET_COM_OUT_SCAN_DIR | SSD1306_CMD_FLAG_COM_OUT_NORMAL);
    }
}

void ssd1306_set_com_out_scan_dir_normal()
{
    ssd1306_set_com_out_scan_dir(false);
}

void ssd1306_set_com_out_scan_dir_remapped()
{
    ssd1306_set_com_out_scan_dir(true);
}

void ssd1306_set_display_offset(uint8_t com_offset)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_DISPLAY_OFFSET);
    ssd1306_send_cmd(com_offset);
}

void ssd1306_set_com_pin_config(uint8_t pin_config_flag, uint8_t com_remap_flag)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_COM_PIN_CONFIG | pin_config_flag | com_remap_flag);
}

void ssd1306_set_dclk_and_freq(uint8_t display_clock_div, uint8_t oscillator_freq)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_DCLK_AND_FREQ);
    ssd1306_send_cmd((oscillator_freq << 4) | (display_clock_div << 0));
}

void ssd1306_set_pre_charge_period(uint8_t phase_1, uint8_t phase_2)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_PRE_CHARGE_PERIOD);
    ssd1306_send_cmd((phase_2 << 4) | (phase_1 << 0));
}

void ssd1306_set_vcomh_deselect_level(uint8_t deselect_level)
{
    switch (deselect_level)
    {
        case SSD1306_VCOMH_DESELECT_LVL_0_65:
        case SSD1306_VCOMH_DESELECT_LVL_0_77:
        case SSD1306_VCOMH_DESELECT_LVL_0_83:
            ssd1306_send_cmd(SSD1306_CMD_SET_VCOMH_DESELECT_LVL);
            ssd1306_send_cmd(deselect_level);
            break;
        default:
            break;
    }
}

void ssd1306_no_operation()
{
    ssd1306_send_cmd(SSD1306_CMD_NO_OPERATION);
}


void ssd1306_set_fade_out_mode(uint8_t fade_out_mode_flag, uint8_t fade_steps)
{
    switch(fade_out_mode_flag)
    {
        case SSD1306_CMD_FLAG_FADE_OUT_OFF:
        case SSD1306_CMD_FLAG_FADE_OUT_ON:
        case SSD1306_CMD_FLAG_FADE_OUT_BLINK:
            ssd1306_send_cmd(SSD1306_CMD_SET_FADE_OUT);
            ssd1306_send_cmd(fade_out_mode_flag | fade_steps);
            break;
        default:
            break;
    }
}

void ssd1306_set_fade_out_off(uint8_t fade_steps)
{
    ssd1306_set_fade_out_mode(SSD1306_CMD_FLAG_FADE_OUT_OFF, fade_steps);
}

void ssd1306_set_fade_out_on(uint8_t fade_steps)
{
    ssd1306_set_fade_out_mode(SSD1306_CMD_FLAG_FADE_OUT_ON, fade_steps);
}

void ssd1306_set_fade_out_blink(uint8_t fade_steps)
{
    ssd1306_set_fade_out_mode(SSD1306_CMD_FLAG_FADE_OUT_BLINK, fade_steps);
}

void ssd1306_set_zoom_in(bool is_enabled)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_ZOOM_IN);

    if(is_enabled)
    {
        ssd1306_send_cmd(SSD1306_ZOOM_IN_ENABLE);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_ZOOM_IN_DISABLE);
    }
}

void ssd1306_zoom_in_enable()
{
    ssd1306_set_zoom_in(true);
}

void ssd1306_zoom_in_disable()
{
    ssd1306_set_zoom_in(false);
}


void ssd1306_set_charge_pump(bool is_enabled)
{
    ssd1306_send_cmd(SSD1306_CMD_SET_CHARGE_PUMP);

    if(is_enabled)
    {
        ssd1306_send_cmd(SSD1306_CHARGE_PUMP_ENABLE);
    }
    else
    {
        ssd1306_send_cmd(SSD1306_CHARGE_PUMP_DISABLE);
    }
}

void ssd1306_charge_pump_enable()
{
    ssd1306_set_charge_pump(true);
}

void ssd1306_charge_pump_disable()
{
    ssd1306_set_charge_pump(false);
}


