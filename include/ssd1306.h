#ifndef __SSD1306_H__
#define __SSD1306_H__

// The ssd1306 will assume that every byte the follows a stream prefix is 
// of
// that type.  For example, every byte from 0x40 to the stop bit is data.
#define SSD1306_CMD_SINGLE  0x80
#define SSD1306_CMD_STREAM  0x00
#define SSD1306_DATA_STREAM 0x40
// Set contrast with following byte between 0x00 and 0xFF.
#define SSD1306_SET_CONTRAST   0x81
#define SSD1306_ENTIRE_ON_RAM    0xA4
#define SSD1306_ENTIRE_ON_NO_RAM 0xA5
#define SSD1306_NORMAL_DISP      0xA6
#define SSD1306_INVERT_DISP      0xA7
#define SSD1306_DISPLAY_SLEEP    0xAE
#define SSD1306_DISPLAY_ON       0xAF
#define SSD1306_LOW_COL_START(x)  (0x00|((x)&0x0F)
#define SSD1306_HIGH_COL_START(x) (0x01|((x)&0x0F))
// Set Addressing mode with following byte.
#define SSD1306_SET_ADDR_MODE   0x20
#define SSD1306_ADDR_MODE_HORZ  0x00
#define SSD1306_ADDR_MODE_VIRT  0x01
#define SSD1306_ADDR_MODE_PAGE  0x02
// Set column range with following two bytes.
// Values from 0x00 to 0x7f are valid.
#define SSD1306_SET_COL_ADDR    0x21
// Set page range with following two bytes.
// Values from 0x00 to 0x07
#define SSD1306_SET_PAGE_ADDR   0x22
#define SSD1306_PAGE_START(x)   (0xB0|((x)&0x07))
#define SSD1306_DISP_START_LINE(x) (0x40|((x)&0x3F))
#define SSD1306_SEG_REMAP(x)       (0xA0|((x)&0x01))
// Set the multiplex ratio with following byte
// Values range from 0x0F to 0x3F
#define SSD1306_SET_MPLEX       0xA8
#define SSD1306_COM_FORWARD     0xC0
#define SSD1306_COM_REVERSE     0xC8
// Set the display offset with the following byte
// Values range from 0x00 to 0x3F
#define SSD1306_DISP_OFFSET     0xD3
// Set the hardware comm configuration with following byte
// Values are 0x02, 0x12, 0x22, 0x32
#define SSD1306_COM_HW_CONF     0xDA
// Set the display clock ratio with following byte
// Values are 0x00 to 0xFF
#define SSD1306_CLOCK_DIVIDE   0xD5
// The charge pump is controlled with the following byte
#define SSD1306_CHARGE_PUMP    0x8D
#define SSD1306_PUMP_ON        0x14
#define SSD1306_PUMP_OFF       0x10
// Set the Vcomh deselect level with the following byte
#define SSD1306_DESELECT_LEVEL 0xDB
#define SSD1306_DESELECT_65VCC 0x00
#define SSD1306_DESELECT_77VCC 0x20
#define SSD1306_DESELECT_83VCC 0x30

#define SSD1306_ADDR_READ_MASK 0xFFFFFF79
#define SSD1306_ADDR_WRITE_MASK 0xFFFFFF78

#define SSD1306_MAX_PRINTABLE_VALUE 10000

#define SSD1306_PAGE_END 0x07
#define SSD1306_COL_END 0x7f

/*
 * SSD1306-Initialize
 *
 * Initializes the display device by sending
 * a sequence of commands to it.
 */

void SSD1306_init();

/* 
 * SSD1306-write-text
 *
 * Prints text to the display device
 */

void SSD1306_write_text(const char* text);

/* 
 * SSD1306-clearr-screen
 *
 * Flags a bool to clear the screen
 * on the next thread execution
 */

void SSD1306_clear_screen();

/*
 * SSD1306-print-number
 *
 * a double to an internal
 * global which is later read
 * from the display thread and
 * printed to the screen
 */


void SSD1306_print_num(double num);

/*
 * SSD1306-Set-Page
 *
 * Set the starting page column.
 * End page is the default
 */

void SSD1306_set_page(unsigned row);

/*
 * SSD1306-Set-Column
 *
 * Similar to the above, just for columns
 */

void SSD1306_set_col(unsigned col);

/*
 * SSD1306-Set-Column-Range
 *
 * You can specify the end if wish.
 */

void SSD1306_set_col_range(unsigned start, unsigned end);

/*
 * SSD1306-Set-Page-Range
 *
 * Set the starting/ending page values
 */

void SSD1306_set_page_range(unsigned start, unsigned end);

/*
 * SSD1306-Display-Thread
 *
 * The main thread for the doisplay
 */

void SSD1306_display_thread();

#endif //
