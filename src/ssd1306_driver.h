/**
 *
 *  @file
 *  @brief Dislpay driver API header
 *
 **/

#ifndef SSD1306_DRIVER_H
#define SSD1306_DRIVER_H

#include <stdint.h>
#include <hardware/i2c.h>

/**
 * @def SSD1306_HEIGHT
 * @brief Height of the SSD1306 display in pixels.
 *
 * @def SSD1306_WIDTH
 * @brief Width of the SSD1306 display in pixels.
 *
 * @def SSD1306_PAGE_HEIGHT
 * @brief Height of each page in the SSD1306 display in pixels.
 *
 * @def SSD1306_PAGE_COUNT
 * @brief Number of pages in the SSD1306 display.
 *
 * @def SSD1306_RAM_BUFF_SIZE
 * @brief Size of the RAM buffer for the SSD1306 display in bytes.
 * 
 * @def SSD1306_I2C_CLK_FREQ_KHZ
 * @brief I2C clock frequency for communication with the SSD1306 display in kilohertz.
 * 
 * @def SSD1306_I2C_ADDRESS
 * @brief I2C address of the SSD1306 display.
 * 
 * @def SSD1306_MIN_MUX_RATIO
 * @brief Minimum multiplex ratio for the SSD1306 display.
 * 
 * @def SSD1306_MAX_MUX_RATIO
 * @brief Maximum multiplex ratio for the SSD1306 display.
 * 
 * @def SSD1306_MAX_DCLK_DIV_RATIO
 * @brief Maximum display clock divider ratio for the SSD1306 display.
 * 
 * @def SSD1306_MAX_OSC_FREQ_LEVEL
 * @brief Maximum oscillator frequency level for the SSD1306 display.
 * 
 * @def SSD1306_MAX_PRECHARGE_PHASE_PERIOD
 * @brief Maximum precharge phase period for the SSD1306 display.
 *
 * @def SSD1306_DEFAULT_DCLK_DIV_RATIO
 * @brief Default display clock divider ratio for the SSD1306 display.
 * 
 * @def SSD1306_DEFAULT_OSC_FREQ_LEVEL
 * @brief Default oscillator frequency level for the SSD1306 display.
 *
 * @def SSD1306_DEFAULT_PRECHARGE_PHASE_PERIOD
 * @brief Default precharge phase period for the SSD1306 display.
 */
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


/**
 * @enum ssd1306_err_t
 * @brief Enumeration of error codes for the SSD1306 display API functions.
 */
typedef enum ssd1306_err_t
{
    SSD1306_ERR_OK,                               /**< No error, operation successful. */
    SSD1306_ERR_INITIALIZED,                      /**< The display is already initialized. */
    SSD1306_ERR_DEINITIALIZED,                    /**< The display is not initialized. */
    SSD1306_ERR_I2C_INSTANCE_INVALID,             /**< Invalid I2C instance. */
    SSD1306_ERR_PINS_DUPLICATED,                  /**< SDA and SCL pins are the same. */
    SSD1306_ERR_PICO_I2C_TIMEOUT,                 /**< Pico reached timeout during I2C operation. */
    SSD1306_ERR_PICO_ERROR_GENERIC,               /**< Generic Pico error. */
    SSD1306_ERR_ZERO_LEN_DATA,                    /**< Zero-lengthr RAM data provided. */
    SSD1306_ERR_NULL_DATA,                        /**< Null RAM data pointer provided. */
    SSD1306_ERR_INVALID_SDA_PIN,                  /**< Invalid SDA pin. */
    SSD1306_ERR_INVALID_SCL_PIN,                  /**< Invalid SCL pin. */
    SSD1306_ERR_INVALID_PAGE,                     /**< Invalid page number.  [0, 7] required  */
    SSD1306_ERR_INVALID_ROW,                      /**< Invalid row number.  [0, 63] required */
    SSD1306_ERR_INVALID_COLUMN,                   /**< Invalid column number. [0, 127] required */
    SSD1306_ERR_INVALID_PAGE_BOUNDS,              /**< Start page is larger than end page. ([A, B] where B < A) */
    SSD1306_ERR_INVALID_COLUMN_BOUNDS,            /**< Start column is larger than end column. ([A, B] where B < A)*/
    SSD1306_ERR_INVALID_SCROLL_FREQ,              /**< Invalid scroll frame frequency. */
    SSD1306_ERR_INVALID_HSCROLL_OFFSET,           /**< Invalid horizontal scroll offset. */
    SSD1306_ERR_INVALID_HSCROLL_POSITION,         /**< Invalid horizontal scroll position. */
    SSD1306_ERR_INVALID_HSCROLL_HEIGHT,           /**< Invalid horizontal scroll height. */
    SSD1306_ERR_INVALID_MUX_RATIO,                /**< Invalid multiplex ratio. [15, 63] required */
    SSD1306_ERR_INVALID_DCLK_DIV_RATIO,           /**< Invalid display clock divider ratio. [0, 15] required */
    SSD1306_ERR_INVALID_OSC_FREQ_LEVEL,           /**< Invalid oscillator frequency level. [0, 15] required */
    SSD1306_ERR_INVALID_PRECHARGE_PHASE_PERIOD,   /**< Invalid precharge phase period. [0, 15] required */
    SSD1306_ERR_INVALID_VCOMH_DESELECT_LEVEL,     /**< Invalid VCOMH deselect level. */
    SSD1306_ERR_INVALID_FADEOUT_MODE,             /**< Invalid fadeout mode. */
    SSD1306_ERR_INVALID_FADEOUT_FREQ,             /**< Invalid fadeout frequency. */
} ssd1306_err_t;


/**
 * @enum ssd1306_scroll_freq_t
 * @brief Enumeration of frame frequency values for scroll setup.
 */
typedef enum ssd1306_scroll_freq_t
{
    SSD1306_SCROLL_FREQ_2       = _u(0b111), /**< Frame frequency 2. */
    SSD1306_SCROLL_FREQ_3       = _u(0b100), /**< Frame frequency 3. */   
    SSD1306_SCROLL_FREQ_4       = _u(0b101), /**< Frame frequency 4. */
    SSD1306_SCROLL_FREQ_5       = _u(0b000), /**< Frame frequency 5. */
    SSD1306_SCROLL_FREQ_25      = _u(0b110), /**< Frame frequency 25. */
    SSD1306_SCROLL_FREQ_64      = _u(0b001), /**< Frame frequency 64. */
    SSD1306_SCROLL_FREQ_128     = _u(0b010), /**< Frame frequency 128. */
    SSD1306_SCROLL_FREQ_256     = _u(0b011), /**< Frame frequency 256. */
    SSD1306_SCROLL_FREQ_COUNT   = _u(8),     /**< Total number of valid values. */
}
ssd1306_scroll_freq_t;

/**
 * @enum ssd1306_vcomh_t
 * @brief Enumeration of VCOMH (VCOMH Deselect Level) settings for the SSD1306 display.
 * These values represent high voltage level of common pins.
 */
typedef enum ssd1306_vcomh_t
{
    SSD1306_VCOMH_DESELECT_LVL_0_65 = _u(0x00), /**< VCOMH Deselect Level x0.65  */
    SSD1306_VCOMH_DESELECT_LVL_0_77 = _u(0x20), /**< VCOMH Deselect Level x0.77. */
    SSD1306_VCOMH_DESELECT_LVL_0_83 = _u(0x30), /**< VCOMH Deselect Level x0.83. */
} ssd1306_vcomh_t;


/**
 * @enum ssd1306_fade_out_mode_t
 * @brief Enumeration of fade-out modes for the SSD1306 display.
*/
typedef enum ssd1306_fade_out_mode_t
{
    SSD1306_FADE_OUT_MODE_OFF   = _u(0x00), /**< Display is in default condition. */
    SSD1306_FADE_OUT_MODE_FADE  = _u(0x20), /**< Fade-out effect. Contrast is decreasing repeatedly. When it becomes 0, initial contrast is restored. Action repeats. */
    SSD1306_FADE_OUT_MODE_BLINK = _u(0x30), /**< Blinking effect. Contrast is decreasing repeatedly. When it becomes 0, it starts to increase until initial contrast. Action repeats. */
} ssd1306_fade_out_mode_t;

/**
 * @enum ssd1306_fade_out_freq_t
 * @brief Enumeration of frame frequencies for fade-out/blinking effect.
 */
typedef enum ssd1306_fade_out_freq_t
{
    SSD1306_FADE_OUT_FREQ_8     = _u(0b0000), /**< Frame frequency 8. */
    SSD1306_FADE_OUT_FREQ_16    = _u(0b0001), /**< Frame frequency 16. */
    SSD1306_FADE_OUT_FREQ_24    = _u(0b0010), /**< Frame frequency 24. */
    SSD1306_FADE_OUT_FREQ_32    = _u(0b0011), /**< Frame frequency 32. */
    SSD1306_FADE_OUT_FREQ_40    = _u(0b0100), /**< Frame frequency 40. */
    SSD1306_FADE_OUT_FREQ_48    = _u(0b0101), /**< Frame frequency 48. */
    SSD1306_FADE_OUT_FREQ_56    = _u(0b0110), /**< Frame frequency 56. */
    SSD1306_FADE_OUT_FREQ_64    = _u(0b0111), /**< Frame frequency 64. */
    SSD1306_FADE_OUT_FREQ_72    = _u(0b1000), /**< Frame frequency 72. */
    SSD1306_FADE_OUT_FREQ_80    = _u(0b1001), /**< Frame frequency 80. */
    SSD1306_FADE_OUT_FREQ_88    = _u(0b1010), /**< Frame frequency 88. */
    SSD1306_FADE_OUT_FREQ_96    = _u(0b1011), /**< Frame frequency 96. */
    SSD1306_FADE_OUT_FREQ_104   = _u(0b1100), /**< Frame frequency 104. */
    SSD1306_FADE_OUT_FREQ_112   = _u(0b1101), /**< Frame frequency 112. */
    SSD1306_FADE_OUT_FREQ_120   = _u(0b1110), /**< Frame frequency 120. */
    SSD1306_FADE_OUT_FREQ_128   = _u(0b1111), /**< Frame frequency 128. */
    SSD1306_FADE_OUT_FREQ_COUNT = _u(16),     /**< Total number of valid values. */
} ssd1306_fade_out_freq_t;


/**
 * @brief Initializes the SSD1306 display using the specified I2C instance and pin configuration.
 *
 * @param i2c_instance Pointer to the I2C instance to be used for communication.
 * @param sda_pin The SDA pin for I2C communication.
 * @param scl_pin The SCL pin for I2C communication.
 * @return API error code.
 */
ssd1306_err_t ssd1306_init_i2c(
    i2c_inst_t* i2c_instance,  
    uint sda_pin, 
    uint scl_pin
);

/**
 * @brief Deinitializes the SSD1306 display that was previously initialized by ssd1306_init_i2c.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_deinit_i2c();

/**
 * @brief Sends an array of RAM data to show.
 *
 * @param data An array containing RAM (pixel) data.
 * @param data_len The length of the data array.
 * @return API error code.
 */
ssd1306_err_t ssd1306_send_data(
    uint8_t data[], 
    size_t data_len
);


/**
 * @brief Sets the contrast level of the SSD1306 display to the specified value.
 *
 * @param contrast The contrast level to set [0, 127].
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_contrast(
    uint8_t contrast
);

/**
 * @brief Configures the SSD1306 display to follow the RAM content.
 * 
 * @return API error code.
 */
ssd1306_err_t ssd1306_follow_ram();

/**
 * @brief Configures the SSD1306 display to ignore the RAM content and turn on all pixels.
 *
 * @return An error code indicating the result of the configuration process.
 */
ssd1306_err_t ssd1306_ignore_ram();

/**
 * @brief Turns off pixel value inversion.
 * RAM 1 - pixel ON, RAM 0 - pixel OFF, 
 * 
 * @return API error code.
 */
ssd1306_err_t ssd1306_inversion_off();

/**
 * @brief Turn on pixel value inversion.
 * RAM 1 - pixel OFF, RAM 0 - pixel ON, 
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_inversion_on();

/**
 * @brief Power off the SSD1306 display.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_power_off();

/**
 * @brief Power on SSD1306 display.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_power_on();


/**
 * @brief Setup continuous right horizontal scroll.
 *
 * @param page_start Scroll start page address.
 * @param page_end Scroll end page address.
 * @param scroll_freq Scroll frame frequency.
 * @return API error code.
 */
ssd1306_err_t ssd1306_h_scroll_right_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq
);

/**
 * @brief Setup continuous left horizontal scroll.
 *
 * @param page_start Scroll start page address.
 * @param page_end Scroll end page address.
 * @param scroll_freq Scroll frame frequency.
 * @return API error code.
 */
ssd1306_err_t ssd1306_h_scroll_left_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq
);

/**
 * @brief Setup continuous vertical and right horizontal scroll.
 *
 * @param page_start Scroll start page address.
 * @param page_end Scroll end page address.
 * @param scroll_freq Scroll frame frequency.
 * @param row_offset Row shift per one scroll.
 * @return API error code.
 */
ssd1306_err_t ssd1306_vh_scroll_right_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq,
    uint8_t row_offset
);

/**
 * @brief Setup continuous vertical and left horizontal scroll.
 *
 * @param page_start Scroll start page address.
 * @param page_end Scroll end page address.
 * @param scroll_freq Scroll frame frequency.
 * @param row_offset Row shift per one scroll.
 * @return API error code.
 */
ssd1306_err_t ssd1306_vh_scroll_left_setup(
    uint8_t page_start,
    uint8_t page_end,
    ssd1306_scroll_freq_t scroll_freq,
    uint8_t row_offset
);

/**
 * @brief Disable scrolling.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_scroll_off();

/**
 * @brief Enable scrolling.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_scroll_on();


/**
 * @brief Set the start column in page memory addressing mode.
 *
 * @param col The starting column number.
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_page_mode_start_col(
    uint8_t col
);

/**
 * @brief Set horizontal memory addressing mode.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_mem_mode_horizontal();

/**
 * @brief Set vertical memory addressing mode.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_mem_mode_vertical();

/**
 * @brief Set page memory addressing mode.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_mem_mode_page();

/**
 * @brief Set column address range in horizontal/vertical memory addressing mode.
 *
 * @param col_start The starting column address.
 * @param col_end The ending column address.
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_col_address(
    uint8_t col_start, 
    uint8_t col_end
);

/**
 * @brief Set page address range in horizontal/vertical memory addressing mode.
 *
 * @param col_start The starting page address.
 * @param col_end The ending page address.
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_page_address(
    uint8_t page_start, 
    uint8_t page_end
);

/**
 * @brief Set the start page in page memory addressing mode.
 *
 * @param col The starting page number.
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_page_mode_start_page(
    uint8_t page
);

/**
 * @brief Set the display start line on the SSD1306 display.
 *
 * @param row The starting line number.
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_display_start_line(
    uint8_t row
);

/**
 * @brief Disable SEG hardware column address remap.
 * Column address 0 is mapped to SEG0.
 * Don't affect on already stored RAM data
 * 
 * @return API error code.
 */
ssd1306_err_t ssd1306_seg_remap_off();

/**
 * @brief Enable SEG hardware column address remap.
 * Column address 127 is mapped to SEG0.
 * Don't affect already stored RAM data.
 * 
 * @return API error code.
 */
ssd1306_err_t ssd1306_seg_remap_on();

/**
 * @brief Set the multiplex ratio.
 * Allowed values are [15, 63]. To set N multiplex ratio put N-1 as a function argument.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_mux_ratio(
    uint8_t mux_ratio
);

/**
 * @brief Disable COM output scan direction remap.
 * Scans from COM0 to COM[N-1], where N is multiplex ratio.
 * Display output is affected immediately.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_com_out_scan_remap_off();


/**
 * @brief Enable COM output scan direction remap.
 * Scans from COM[N-1] to COM0, where N is multiplex ratio.
 * Display output is affected immediately.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_com_out_scan_remap_on();

/**
 * @brief Vertical shift by COM from 0 to 63.
 * Maps COM0 with N-th row on display
 * @param row Display offset in rows.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_display_offset(
    uint8_t row
);

/**
 * @brief Set COM pins hardware configuration:
 * - Pin configuration: Sequential
 * - COM left/right remap: OFF
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_com_pin_config_seq_remap_off();

/**
 * @brief Set COM pins hardware configuration:
 * - Pin configuration: Alternative
 * - COM left/right remap: OFF
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_com_pin_config_alt_remap_off();

/**
 * @brief Set COM pins hardware configuration:
 * - Pin configuration: Sequential
 * - COM left/right remap: ON
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_com_pin_config_seq_remap_on();

/**
 * @brief Set COM pins hardware configuration:
 * - Pin configuration: Alternative
 * - COM left/right remap: ON
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_com_pin_config_alt_remap_on();

/**
 * @brief Set the display clock divider ratio and oscillator frequency level.
 *
 * @param dclk_div_ratio The display clock divider ratio. 
 * Allowed values are [0, 15]. To set N divider ratio, put N-1 as a function argument.
 * @param osc_freq_level The oscillator frequency level.
 * Allowed values are [0, 15]. Levels distributed uniformly approximately between 230 and 600 kHZ.
 * Frequency increases with the level value. 
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_dclk_div_and_osc_freq(
    uint8_t dclk_div_ratio,
    uint8_t osc_freq_level
);

/**
 * @brief Set the precharge period on the SSD1306 display.
 * Allowed values are [0, 15]. The interval is counted in number of DCLK.
 *
 * @param phase1_peroid The phase 1 precharge period.
 * @param phase2_peroid The phase 2 precharge period.
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_precharge_period(
    uint8_t phase1_peroid,
    uint8_t phase2_peroid
);

/**
 * @brief Set the VCOMH Deselect Level.
 *
 * @param deselect_level The deselect level value.
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_vcomh_deselect_level(
    ssd1306_vcomh_t deselect_level 
);

/**
 * @brief Sends a no operation command to the display.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_no_operation();

/**
 * @brief Set the fade-out mode and fade-out frequency.
 *
 * @param mode The fade-out mode value.
 * @param freq The fade-out frequency value.
 * @return API error code.
 */
ssd1306_err_t ssd1306_set_fade_out_mode(
    ssd1306_fade_out_mode_t mode, 
    ssd1306_fade_out_freq_t freq
);

/**
 * @brief Disable zoom-in mode.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_zoom_in_off();

/**
 * @brief Enable zoom-in mode.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_zoom_in_on();

/**
 * @brief Disable the charge pump.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_charge_pump_off();

/**
 * @brief Enable the charge pump.
 *
 * @return API error code.
 */
ssd1306_err_t ssd1306_charge_pump_on();


#endif //SSD1306_DRIVER_H