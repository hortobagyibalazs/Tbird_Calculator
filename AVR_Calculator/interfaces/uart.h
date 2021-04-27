/*
 * uart.h
 *
 * Created: 4/27/2021 7:08:18 PM
 *  Author: Dell
 */ 

unsigned int USART0_def_ubrr();
void USART0_init(unsigned int ubrr);
void USART0_transmit(unsigned char data);
unsigned char USART0_receive();