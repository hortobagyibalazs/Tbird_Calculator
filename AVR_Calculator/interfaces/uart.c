/*
 * uart.c
 *
 * Created: 4/27/2021 6:55:13 PM
 *  Author: Dell
 */ 

#define F_CPU 8000000
#define BAUD 9600
#define BAUD_PRESCALE (F_CPU / 16 / BAUD - 1)

#include <avr/io.h>

unsigned int USART0_def_ubrr()
{
	return BAUD_PRESCALE;
}

void USART0_init(unsigned int ubrr)
{
	// Set baud rate
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;
	// Enabled receiver and transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	// Set frame format: 8 data, 2 stop bit
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);
}

void USART0_transmit(unsigned char data)
{
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1<<UDRE0)));
	
	// Put data into buffer
	UDR0 = data;
}

unsigned char USART0_receive()
{
	
	// Wait for data to be received
	while (!(UCSR0A & (1<<RXC0)));
	
	// Get and return received data from buffer
	return UDR0;
}