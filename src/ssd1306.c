const unsigned char ssd1306_init[] = {
	SSD1306_CMD_STREAM,
	SSD1306_SET_MPLEX,     0x3f,
	SSD1306_DISP_OFFSET,   0x00,
	SSD1306_DISP_START_LINE(0),
	SSD1306_SEG_REMAP(1),
	SSD1306_COM_REVERSE,
	SSD1306_COM_HW_CONF,   0x12,
	SSD1306_SET_CONTRAST,  0x7F,
	SSD1306_ENTIRE_ON_RAM,
	SSD1306_NORMAL_DISP,
	SSD1306_CLOCK_DIVIDE,  0x80,
	SSD1306_CHARGE_PUMP,   SSD1306_PUMP_ON,
	SSD1306_DISPLAY_ON,
	SSD1306_SET_COL_ADDR,  0x00, 0x7f,
	SSD1306_SET_PAGE_ADDR, 0x00, 0x07,
	SSD1306_SET_ADDR_MODE, SSD1306_ADDR_MODE_HORZ
};
