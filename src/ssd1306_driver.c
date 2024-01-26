#include "ssd1306_driver.h"

typedef struct ssd1306_ctx_t
{
    i2c_inst_t* i2c_instance;
    uint8_t i2c_address;
    uint sda_pin;
    uint scl_pin;
    bool is_init;
} 
ssd1306_ctx_t;

typedef enum ssd1306_i2c_header_t
{
    SSD1306_I2C_HEADER_DATA = _u(0x40),
    SSD1306_I2C_HEADER_CMD  = _u(0x00),
}
ssd1306_i2c_header_t;

typedef enum ssd1306_mem_mode_t
{
    SSD1306_MEM_MODE_HORIZONTAL = _u(0b00),
    SSD1306_MEM_MODE_VERTICAL   = _u(0b01),
    SSD1306_MEM_MODE_PAGE       = _u(0b10),
    SSD1306_MEM_MODE_INVALID    = _u(0b11),
}
ssd1306_mem_mode_t;

typedef enum ssd1306_com_pin_config_t
{
    SSD1306_COM_PIN_CONFIG_SEQ_REMAP_OFF    = _u(0x02),
    SSD1306_COM_PIN_CONFIG_ALT_REMAP_OFF    = _u(0x12),
    SSD1306_COM_PIN_CONFIG_SEQ_REMAP_ON     = _u(0x22),
    SSD1306_COM_PIN_CONFIG_ALT_REMAP_ON     = _u(0x32),
}
ssd1306_com_pin_config_t;

typedef enum ssd1306_zoom_in_mode_t
{
    SSD1306_ZOOM_IN_OFF  = _u(0x00),
    SSD1306_ZOOM_IN_ON   = _u(0x01),
}
ssd1306_zoom_in_mode_t;

typedef enum ssd1306_charge_pump_mode_t
{
    SSD1306_CHARGE_PUMP_OFF  = _u(0x10),
    SSD1306_CHARGE_PUMP_ON   = _u(0x14),
}
ssd1306_charge_pump_mode_t;

typedef enum ssd1306_cmd_t
{
    SSD1306_CMD_SET_CONTRAST                   = _u(0x81),
    SSD1306_CMD_FOLLOW_RAM                     = _u(0xA4),
    SSD1306_CMD_IGNORE_RAM                     = _u(0xA5),
    SSD1306_CMD_INVERSION_OFF                  = _u(0xA6),
    SSD1306_CMD_INVERSION_ON                   = _u(0xA7),
    SSD1306_CMD_POWER_OFF                      = _u(0xAE),
    SSD1306_CMD_POWER_ON                       = _u(0xAF),
    SSD1306_CMD_HSCROL_RIGHT                   = _u(0x26),
    SSD1306_CMD_HSCROL_LEFT                    = _u(0x27),
    SSD1306_CMD_VHSCROL_RIGHT                  = _u(0x29),
    SSD1306_CMD_VHSCROL_LEFT                   = _u(0x2A),
    SSD1306_CMD_SCROLLL_OFF                    = _u(0x2E),
    SSD1306_CMD_SCROLLL_ON                     = _u(0x2F),
    SSD1306_CMD_SET_VSCROLL_AREA               = _u(0xA3),
    SSD1306_CMD_SET_PAGE_MODE_START_COL_LOW    = _u(0x00),
    SSD1306_CMD_SET_PAGE_MODE_START_COL_HIGH   = _u(0x10),
    SSD1306_CMD_SET_MEM_MODE                   = _u(0x20),
    SSD1306_CMD_SET_COL_ADR                    = _u(0x21),
    SSD1306_CMD_SET_PAGE_ADR                   = _u(0x22),
    SSD1306_CMD_SET_PAGE_MODE_START_PAGE       = _u(0xB0),
    SSD1306_CMD_SET_DISPLAY_START_LINE         = _u(0x40),
    SSD1306_CMD_SEG_REMAP_OFF                  = _u(0xA0),
    SSD1306_CMD_SEG_REMAP_ON                   = _u(0xA1),
    SSD1306_CMD_SET_MUX_RATIO                  = _u(0xA8),
    SSD1306_CMD_COM_OUT_REMAP_OFF              = _u(0xC0),
    SSD1306_CMD_COM_OUT_REMAP_ON               = _u(0xC8),
    SSD1306_CMD_SET_DISPLAY_OFFSET             = _u(0xD3),
    SSD1306_CMD_SET_COM_PIN_CONFIG             = _u(0xDA),
    SSD1306_CMD_SET_DCLK_DIV_AND_OSC_FREQ      = _u(0xD5),
    SSD1306_CMD_SET_PRECHARGE_PERIOD           = _u(0xD9),
    SSD1306_CMD_SET_VCOMH_DESELECT_LVL         = _u(0xDB),
    SSD1306_CMD_NO_OPERATION                   = _u(0xE3),
    SSD1306_CMD_SET_FADE_OUT_MODE              = _u(0x23),
    SSD1306_CMD_ZOOM_IN_MODE                   = _u(0xD6),
    SSD1306_CMD_CHARGE_PUMP_MODE               = _u(0x8D)
}
ssd1306_cmd_t;


static ssd1306_err_t ssd1306_i2c_write(
    uint8_t buffer[], 
    size_t buffer_len
);
static ssd1306_err_t ssd1306_send_cmd(
    ssd1306_cmd_t cmd,
    uint8_t cmd_optios[], 
    size_t cmd_optios_len
);
static ssd1306_err_t ssd1306_h_scroll_common_setup(
    ssd1306_cmd_t cmd,
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq
);
static ssd1306_err_t ssd1306_vh_scroll_common_setup(
    ssd1306_cmd_t cmd,
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq,
    uint8_t row_offset
);
static ssd1306_err_t ssd1306_set_mem_mode(
    ssd1306_mem_mode_t mem_mode
);
static ssd1306_err_t ssd1306_set_com_pin_config(
    ssd1306_com_pin_config_t com_pin_config
);
static ssd1306_err_t ssd1306_set_zoom_in_mode(
    ssd1306_zoom_in_mode_t mode
);
static ssd1306_err_t ssd1306_set_charge_pump_mode(
    ssd1306_charge_pump_mode_t mode
);


ssd1306_ctx_t ssd1306_ctx = {};

ssd1306_err_t ssd1306_init_i2c(
    i2c_inst_t* i2c_instance,  
    uint sda_pin, 
    uint scl_pin
)
{
    if(ssd1306_ctx.is_init)
    {
        return SSD1306_ERR_INITIALIZED;
    }
    if(i2c_instance != i2c0 && i2c_instance != i2c1)
    {
        return SSD1306_ERR_I2C_INSTANCE_INVALID;
    }
    if(sda_pin == scl_pin)
    {
        return SSD1306_ERR_PINS_DUPLICATED;
    }
    if(sda_pin >= NUM_BANK0_GPIOS)
    {
        return SSD1306_ERR_INVALID_SDA_PIN;
    }
    if(scl_pin >= NUM_BANK0_GPIOS)
    {
        return SSD1306_ERR_INVALID_SCL_PIN;
    }

    memset(&ssd1306_ctx, 0, sizeof(ssd1306_ctx_t));
    ssd1306_ctx.i2c_instance = i2c_instance;
    ssd1306_ctx.i2c_address = SSD1306_I2C_ADDRESS;
    ssd1306_ctx.sda_pin = sda_pin;
    ssd1306_ctx.sda_pin = scl_pin;
    ssd1306_ctx.is_init = true;

    i2c_init(i2c_instance, SSD1306_I2C_CLK_FREQ_KHZ * 1000);

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    return SSD1306_ERR_OK;
}

ssd1306_err_t ssd1306_deinit_i2c()
{
    if(!ssd1306_ctx.is_init)
    {
        return SSD1306_ERR_DEINITIALIZED;
    }

    i2c_deinit(ssd1306_ctx.i2c_instance);
    gpio_deinit(ssd1306_ctx.scl_pin);
    gpio_deinit(ssd1306_ctx.sda_pin);

    memset(&ssd1306_ctx, 0, sizeof(ssd1306_ctx_t));

    return SSD1306_ERR_OK;
}

ssd1306_err_t ssd1306_i2c_write(
    uint8_t buffer[], 
    size_t buffer_len
)
{
    int write_res = i2c_write_blocking(i2c_default, ssd1306_ctx.i2c_address, buffer, buffer_len, false);

    switch (write_res)
    {
        case PICO_ERROR_TIMEOUT:
            return SSD1306_ERR_PICO_I2C_TIMEOUT;
        case PICO_ERROR_GENERIC:
            return SSD1306_ERR_PICO_ERROR_GENERIC;
        default:
            return SSD1306_ERR_OK;
    }
}

ssd1306_err_t ssd1306_send_cmd(
    ssd1306_cmd_t cmd,
    uint8_t cmd_optios[], 
    size_t cmd_optios_len
)
{
    if(!ssd1306_ctx.is_init)
    {
        return SSD1306_ERR_DEINITIALIZED;
    }

    size_t write_buffer_len = cmd_optios_len + 2;
    uint8_t* write_buffer = (uint8_t*)calloc(write_buffer_len, sizeof(uint8_t));
    
    write_buffer[0] = (uint8_t)SSD1306_I2C_HEADER_CMD;
    write_buffer[1] = (uint8_t)cmd;

    if(cmd_optios != NULL)
    {
        memcpy(write_buffer + 2, cmd_optios, cmd_optios_len);
    }
    
    ssd1306_err_t error = ssd1306_i2c_write(
        write_buffer,
        write_buffer_len
    );

    free(write_buffer);
    return error;
}

ssd1306_err_t ssd1306_send_data(
    uint8_t data[], 
    size_t data_len
)
{
    if(!ssd1306_ctx.is_init)
    {
        return SSD1306_ERR_DEINITIALIZED;
    }
    if(data == NULL)
    {
        return SSD1306_ERR_NULL_DATA;
    }
    if(data_len == SSD1306_ERR_ZERO_LEN_DATA)
    {
        return SSD1306_ERR_NULL_DATA;
    }

    size_t write_buffer_len = data_len + 1;
    uint8_t* write_buffer = (uint8_t*)calloc(write_buffer_len, sizeof(uint8_t));
    
    write_buffer[0] = (uint8_t)SSD1306_I2C_HEADER_DATA;
    memcpy(write_buffer + 1, data, data_len);
    
    ssd1306_err_t error = ssd1306_i2c_write(
        write_buffer,
        write_buffer_len
    );

    free(write_buffer);
    return error;
}


ssd1306_err_t ssd1306_set_contrast(
    uint8_t contrast
)
{
    return ssd1306_send_cmd(SSD1306_CMD_SET_CONTRAST, NULL, 0);
}

ssd1306_err_t ssd1306_follow_ram()
{
    return ssd1306_send_cmd(SSD1306_CMD_FOLLOW_RAM, NULL, 0);
}

ssd1306_err_t ssd1306_ignore_ram()
{
    return ssd1306_send_cmd(SSD1306_CMD_IGNORE_RAM, NULL, 0);
}

ssd1306_err_t ssd1306_inversion_off()
{
    return ssd1306_send_cmd(SSD1306_CMD_INVERSION_OFF, NULL, 0);
}

ssd1306_err_t ssd1306_inversion_on()
{
    return ssd1306_send_cmd(SSD1306_CMD_INVERSION_ON, NULL, 0);
}

ssd1306_err_t ssd1306_power_off()
{
    return ssd1306_send_cmd(SSD1306_CMD_POWER_OFF, NULL, 0);
}

ssd1306_err_t ssd1306_power_on()
{
    return ssd1306_send_cmd(SSD1306_CMD_POWER_ON, NULL, 0);
}


ssd1306_err_t ssd1306_h_scroll_common_setup(
    ssd1306_cmd_t cmd,
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq
)
{
    if(page_start >= SSD1306_PAGE_COUNT || page_end >= SSD1306_PAGE_COUNT)
    {
        return SSD1306_ERR_INVALID_PAGE;
    }
    if(page_start > page_end)
    {
        return SSD1306_ERR_INVALID_PAGE_BOUNDS;
    }
    if(scroll_freq > SSD1306_SCROLL_FREQ_COUNT)
    {
        return SSD1306_ERR_INVALID_SCROLL_FREQ;
    }

    const uint8_t DUMMY_0x00 = 0x00;
    const uint8_t DUMMY_0xFF = 0xFF;

    uint8_t cmd_optios[] = {
        DUMMY_0x00,
        page_start,
        (uint8_t)scroll_freq,
        page_end,
        DUMMY_0x00,
        DUMMY_0xFF
    };  

    return ssd1306_send_cmd(cmd, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_h_scroll_right_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq
)
{
    return ssd1306_h_scroll_common_setup(
        SSD1306_CMD_HSCROL_RIGHT,
        page_start,
        page_end,
        scroll_freq
    );
}

ssd1306_err_t ssd1306_h_scroll_left_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq
)
{
    return ssd1306_h_scroll_common_setup(
        SSD1306_CMD_HSCROL_LEFT,
        page_start,
        page_end,
        scroll_freq
    );
}

ssd1306_err_t ssd1306_vh_scroll_common_setup(
    ssd1306_cmd_t cmd,
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq,
    uint8_t row_offset
)
{
    if(page_start >= SSD1306_PAGE_COUNT || page_end >= SSD1306_PAGE_COUNT)
    {
        return SSD1306_ERR_INVALID_PAGE;
    }
    if(page_start > page_end)
    {
        return SSD1306_ERR_INVALID_PAGE_BOUNDS;
    }
    if(scroll_freq > SSD1306_SCROLL_FREQ_COUNT)
    {
        return SSD1306_ERR_INVALID_SCROLL_FREQ;
    }
    if(row_offset > SSD1306_HEIGHT)
    {
        return SSD1306_ERR_INVALID_HSCROLL_OFFSET;
    }
    
    const uint8_t DUMMY_0x00 = 0x00;

    uint8_t cmd_optios[] = {
        DUMMY_0x00,
        page_start,
        (uint8_t)scroll_freq,
        page_end,
        row_offset
    };  

    return ssd1306_send_cmd(cmd, cmd_optios, count_of(cmd_optios));

}

ssd1306_err_t ssd1306_vh_scroll_right_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq,
    uint8_t row_offset
)
{
    return ssd1306_vh_scroll_common_setup(
        SSD1306_CMD_VHSCROL_RIGHT,
        page_start,
        page_end,
        scroll_freq,
        row_offset
    );
}

ssd1306_err_t ssd1306_vh_scroll_left_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq,
    uint8_t row_offset
)
{
    return ssd1306_vh_scroll_common_setup(
        SSD1306_CMD_VHSCROL_LEFT,
        page_start,
        page_end,
        scroll_freq,
        row_offset
    );
}

ssd1306_err_t ssd1306_scroll_off()
{
    return ssd1306_send_cmd(SSD1306_CMD_SCROLLL_OFF, NULL, 0);
}

ssd1306_err_t ssd1306_scroll_on()
{
    return ssd1306_send_cmd(SSD1306_CMD_SCROLLL_ON, NULL, 0);
}

ssd1306_err_t ssd1306_set_vscroll_area(
    uint8_t row_start, 
    uint8_t row_height
)
{
    if(row_start > SSD1306_HEIGHT || row_height > SSD1306_HEIGHT)
    {
        return SSD1306_ERR_INVALID_ROW;
    }

    uint8_t cmd_optios[] = {
        row_start,
        row_height
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_VSCROLL_AREA, cmd_optios, count_of(cmd_optios));
}


ssd1306_err_t ssd1306_set_page_mode_start_col(
    uint8_t col
)
{
    if(col >= SSD1306_WIDTH)
    {
        return SSD1306_ERR_INVALID_COLUMN;
    }

    const uint8_t LOW_HALFBYTE_MASK = 0x0F;
    const uint8_t HIGH_HALFBYTE_MASK = 0xF0;
    ssd1306_err_t res = SSD1306_ERR_OK;

    res = ssd1306_send_cmd(
        SSD1306_CMD_SET_PAGE_MODE_START_COL_LOW | ((col & LOW_HALFBYTE_MASK) >> 0),
        NULL, 0
    );

    if(res != SSD1306_ERR_OK)
    {
        return res;
    }

    res = ssd1306_send_cmd(
        SSD1306_CMD_SET_PAGE_MODE_START_COL_HIGH | ((col & HIGH_HALFBYTE_MASK) >> 4),
        NULL, 0
    );

    return res;
}


ssd1306_err_t ssd1306_set_mem_mode(
    ssd1306_mem_mode_t mem_mode
)
{
    uint8_t cmd_optios[] = {
        (uint8_t)mem_mode,
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_MEM_MODE, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_set_mem_mode_horizontal()
{
    return ssd1306_set_mem_mode(SSD1306_MEM_MODE_HORIZONTAL);
}

ssd1306_err_t ssd1306_set_mem_mode_vertical()
{
    return ssd1306_set_mem_mode(SSD1306_MEM_MODE_VERTICAL);
}

ssd1306_err_t ssd1306_set_mem_mode_page()
{
    return ssd1306_set_mem_mode(SSD1306_MEM_MODE_PAGE);
}

ssd1306_err_t ssd1306_set_col_address(
    uint8_t col_start, 
    uint8_t col_end
)
{
    if(col_start > SSD1306_WIDTH || col_end > SSD1306_WIDTH)
    {
        return SSD1306_ERR_INVALID_COLUMN;
    }
    if(col_start > col_end)
    {
        return SSD1306_ERR_INVALID_COLUMN_BOUNDS;
    }

    uint8_t cmd_optios[] = {
        (uint8_t)col_start,
        (uint8_t)col_end
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_COL_ADR, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_set_page_address(
    uint8_t page_start, 
    uint8_t page_end
)
{
    if(page_start > SSD1306_PAGE_COUNT || page_end > SSD1306_PAGE_COUNT)
    {
        return SSD1306_ERR_INVALID_PAGE;
    }
    if(page_start > page_end)
    {
        return SSD1306_ERR_INVALID_PAGE_BOUNDS;
    }

    uint8_t cmd_optios[] = {
        page_start,
        page_end
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_PAGE_ADR, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_set_page_mode_start_page(
    uint8_t page
)
{
    if(page > SSD1306_PAGE_COUNT)
    {
        return SSD1306_ERR_INVALID_PAGE;
    }

    return ssd1306_send_cmd(
        SSD1306_CMD_SET_PAGE_MODE_START_PAGE | page,
        NULL, 0
    );
}


ssd1306_err_t ssd1306_set_display_start_line(
    uint8_t row
)
{
    if(row > SSD1306_HEIGHT)
    {
        return SSD1306_ERR_INVALID_ROW;
    }
   
    return ssd1306_send_cmd(
        SSD1306_CMD_SET_DISPLAY_START_LINE | row,
        NULL, 0
    );
}

ssd1306_err_t ssd1306_seg_remap_off()
{
    return ssd1306_send_cmd(SSD1306_CMD_SEG_REMAP_OFF, NULL, 0);
}

ssd1306_err_t ssd1306_seg_remap_on()
{
    return ssd1306_send_cmd(SSD1306_CMD_SEG_REMAP_ON, NULL, 0);
}

ssd1306_err_t ssd1306_set_mux_ratio(
    uint8_t mux_ratio
)
{
    if(mux_ratio < SSD1306_MIN_MUX_RATIO || mux_ratio > SSD1306_MAX_MUX_RATIO)
    {
        return SSD1306_ERR_INVALID_MUX_RATIO;
    }

    uint8_t cmd_optios[] = {
        mux_ratio
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_MUX_RATIO, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_com_out_scan_remap_off()
{
    return ssd1306_send_cmd(SSD1306_CMD_COM_OUT_REMAP_OFF, NULL, 0);
}

ssd1306_err_t ssd1306_com_out_scan_remap_on()
{
    return ssd1306_send_cmd(SSD1306_CMD_COM_OUT_REMAP_ON, NULL, 0);
}

ssd1306_err_t ssd1306_set_display_offset(
    uint8_t row
)
{
    if(row > SSD1306_HEIGHT)
    {
        return SSD1306_ERR_INVALID_ROW;
    }
   
    uint8_t cmd_optios[] = {
        row
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_DISPLAY_OFFSET, cmd_optios, count_of(cmd_optios));
}



ssd1306_err_t ssd1306_set_com_pin_config(
    ssd1306_com_pin_config_t com_pin_config
)
{
    uint8_t cmd_optios[] = {
        (uint8_t)com_pin_config
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_COM_PIN_CONFIG, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_set_com_pin_config_seq_remap_off()
{
    return ssd1306_set_com_pin_config(SSD1306_COM_PIN_CONFIG_SEQ_REMAP_OFF);
}

ssd1306_err_t ssd1306_set_com_pin_config_alt_remap_off()
{
    return ssd1306_set_com_pin_config(SSD1306_COM_PIN_CONFIG_ALT_REMAP_OFF);
}

ssd1306_err_t ssd1306_set_com_pin_config_seq_remap_on()
{
    return ssd1306_set_com_pin_config(SSD1306_COM_PIN_CONFIG_SEQ_REMAP_ON);
}

ssd1306_err_t ssd1306_set_com_pin_config_alt_remap_on()
{
    return ssd1306_set_com_pin_config(SSD1306_COM_PIN_CONFIG_ALT_REMAP_ON);   
}


ssd1306_err_t ssd1306_set_dclk_div_and_osc_freq(
    uint8_t dclk_div_ratio,
    uint8_t osc_freq_level
)
{
    if(dclk_div_ratio > SSD1306_MAX_DCLK_DIV_RATIO)
    {
        return SSD1306_ERR_INVALID_DCLK_DIV_RATIO;
    }
    if(osc_freq_level > SSD1306_MAX_OSC_FREQ_LEVEL)
    {
        return SSD1306_ERR_INVALID_OSC_FREQ_LEVEL;
    }

    uint8_t cmd_optios[] = {
        (osc_freq_level << 4) | (dclk_div_ratio << 0)
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_DCLK_DIV_AND_OSC_FREQ, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_set_precharge_period(
    uint8_t phase1_peroid,
    uint8_t phase2_peroid
)
{
    if(phase1_peroid > SSD1306_MAX_PRECHARGE_PHASE_PERIOD || phase2_peroid > SSD1306_MAX_PRECHARGE_PHASE_PERIOD)
    {
        return SSD1306_ERR_INVALID_PRECHARGE_PHASE_PERIOD;
    }

    uint8_t cmd_optios[] = {
        (phase2_peroid << 4) | (phase1_peroid << 0)
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_PRECHARGE_PERIOD, cmd_optios, count_of(cmd_optios));

}

ssd1306_err_t ssd1306_set_vcomh_deselect_level(
    ssd1306_vcomh_t deselect_level 
)
{
    if (deselect_level != SSD1306_VCOMH_DESELECT_LVL_0_65
     && deselect_level != SSD1306_VCOMH_DESELECT_LVL_0_77
     && deselect_level != SSD1306_VCOMH_DESELECT_LVL_0_83
    )
    {
        return SSD1306_ERR_INVALID_VCOMH_DESELECT_LEVEL;
    }

    uint8_t cmd_optios[] = {
        (uint8_t)deselect_level
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_VCOMH_DESELECT_LVL, cmd_optios, count_of(cmd_optios));    
}

ssd1306_err_t ssd1306_no_operation()
{
    return ssd1306_send_cmd(SSD1306_CMD_NO_OPERATION, NULL, 0);       
}


ssd1306_err_t ssd1306_set_fade_out_mode(
    ssd1306_fade_out_mode_t mode, 
    ssd1306_fade_out_freq_t freq
)
{
    if (mode != SSD1306_FADE_OUT_MODE_OFF
     && mode != SSD1306_FADE_OUT_MODE_FADE
     && mode != SSD1306_FADE_OUT_MODE_BLINK
    )
    {
        return SSD1306_ERR_INVALID_FADEOUT_MODE;
    }

    if(freq > SSD1306_FADE_OUT_FREQ_COUNT)
    {
        return SSD1306_ERR_INVALID_FADEOUT_FREQ;
    }


    uint8_t cmd_optios[] = {
        ((uint8_t)mode << 4) | ((uint8_t)freq << 0)
    };

    return ssd1306_send_cmd(SSD1306_CMD_SET_FADE_OUT_MODE, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_set_zoom_in_mode(
    ssd1306_zoom_in_mode_t mode
)
{
    uint8_t cmd_optios[] = {
        (uint8_t)mode
    };

    return ssd1306_send_cmd(SSD1306_CMD_ZOOM_IN_MODE, cmd_optios, count_of(cmd_optios));
}

ssd1306_err_t ssd1306_zoom_in_off()
{
    return ssd1306_set_zoom_in_mode(SSD1306_ZOOM_IN_OFF);
}

ssd1306_err_t ssd1306_zoom_in_on()
{
    return ssd1306_set_zoom_in_mode(SSD1306_ZOOM_IN_ON);
}


ssd1306_err_t ssd1306_set_charge_pump_mode(
    ssd1306_charge_pump_mode_t mode
)
{
    uint8_t cmd_optios[] = {
        (uint8_t)mode
    };

    return ssd1306_send_cmd(SSD1306_CMD_CHARGE_PUMP_MODE, cmd_optios, count_of(cmd_optios));   
}

ssd1306_err_t ssd1306_charge_pump_off()
{
    return ssd1306_set_charge_pump_mode(SSD1306_CHARGE_PUMP_OFF);
}

ssd1306_err_t ssd1306_charge_pump_on()
{
    return ssd1306_set_charge_pump_mode(SSD1306_CHARGE_PUMP_ON);
}