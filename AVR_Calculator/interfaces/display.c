#include "display.h"

#define F_CPU 8000000

#include <util/delay.h>
#include <inttypes.h>
#include <avr/io.h>

#define LCD_CMD_DDR (DDRF)
#define LCD_DATA_DDR (DDRE)

#define LCD_CMD_PORT (PORTF)
#define LCD_DATA_PORT (PORTE)
#define LCD_DATA_PIN (PINE)

#define LCD_RS (PF1)
#define LCD_RW (PF2)
#define LCD_EN (PF3)

#define LCD_ENABLE 2
#define LCD_CURSOR 1
#define LCD_BLINK 0

static int initialized = 0;

static int display_on = 0;
static int cursor_visible = 0;
static int blinking = 0;

// Helper functions
void LCD_clock()
{
	LCD_CMD_PORT |= (1<<LCD_EN);
	_delay_us(2);
	LCD_CMD_PORT &= ~(1<<LCD_EN);
	_delay_us(2);
}

void LCD_busy(void)
{
	uint8_t busy;
	LCD_DATA_DDR &= ~(1<<PE7);
	LCD_CMD_PORT &= ~(1<<LCD_RS);
	LCD_CMD_PORT |= (1<<LCD_RW);
	
	do
	{
		busy = 0;
		LCD_CMD_PORT |= (1<<LCD_EN);
		_delay_us(1);
		busy = (LCD_DATA_PIN & (1<<PE7));
		LCD_CMD_PORT &= ~(1<<LCD_EN);
		_delay_us(1);
		LCD_CMD_PORT |= (1<<LCD_EN);
		_delay_us(1);
		LCD_CMD_PORT &= ~(1<<LCD_EN);
		_delay_us(1);
	} while (busy);
	
	LCD_CMD_PORT &= ~(1<<LCD_RW);
	LCD_DATA_DDR |= (1<<PE7);
}

void LCD_cmd(uint8_t cmd)
{
	LCD_busy();
	LCD_CMD_PORT &= ~(1<<LCD_RS);
	LCD_CMD_PORT &= ~(1<<LCD_RW);
	LCD_CMD_PORT &= ~(1<<LCD_EN);
	LCD_DATA_PORT &= ~(0xf0);
	LCD_DATA_PORT |= (cmd & 0xf0);
	LCD_clock();
	LCD_DATA_PORT &= ~(0xf0);
	LCD_DATA_PORT |= ((cmd<<4) & 0xf0);
	LCD_clock();
}

void LCD_data(uint8_t data)
{
	LCD_busy();
	LCD_CMD_PORT |= (1<<LCD_RS);
	LCD_CMD_PORT &= ~(1<<LCD_RW);
	LCD_CMD_PORT &= ~(1<<LCD_EN);
	LCD_DATA_PORT &= ~(0xf0);
	LCD_DATA_PORT |= (data & 0xf0);
	LCD_clock();
	LCD_DATA_PORT &= ~(0xf0);
	LCD_DATA_PORT |= ((data<<4) & 0xf0);
	LCD_clock();
}

void LCD_init()
{
	DDRE |= 0xf0;
	DDRF |= (1<<LCD_RS) | (1<<LCD_RW) | (1<<LCD_EN);
	PORTF &= ~(LCD_RW);
	PORTF &= ~(1<<LCD_RS);
	PORTE = 0x20;
	
	LCD_clock();
	LCD_clock();
	LCD_clock();
	
	LCD_cmd(0x28);
	LCD_cmd(0x28);
	LCD_cmd(0x28);
	
	LCD_cmd(0x02);
	LCD_cmd(0x01);
	
	display_on = 0;
	cursor_visible = 0;
	blinking = 0;
	LCD_update();
	
	initialized = 1;
}

void LCD_update()
{
	LCD_cmd(0x08 | (display_on<<LCD_ENABLE) | (cursor_visible<<LCD_CURSOR) | (blinking<<LCD_BLINK));
}

void safe_init()
{
	if (!initialized) LCD_init();
}

// This is where the display interface is implemented
void display_enable()
{
	safe_init();
	display_on = 1;
	LCD_update();
}

void display_disable()
{
	safe_init();
	display_on = 0;
	LCD_update();
}

int display_is_on()
{
	return display_on;
}

void display_enable_cursor()
{
	safe_init();
	cursor_visible = 1;
	LCD_update();
}

void display_disable_cursor()
{
	safe_init();
	cursor_visible = 0;
	LCD_update();
}

int display_is_cursor_visible()
{
	return cursor_visible;
}

void display_enable_blink()
{
	safe_init();
	blinking = 1;
	LCD_update();
}

void display_disable_blink()
{
	safe_init();
	blinking = 0;
	LCD_update();
}

int display_is_blink_enabled()
{
	return blinking;	
}

void display_clear()
{
	safe_init();
	
	LCD_cmd(0x01);	
}

void display_write_char(char ch)
{
	safe_init();
	
	LCD_data(ch);
}

void display_write_str(char* text)
{
	safe_init();
	
	int i = 0;
	while (*(text + i))
	{
		LCD_data(*(text + i));
		i++;
	}
}