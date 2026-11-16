/*****************************************************************************
 *	    File: Lab08.c
 *  Lab Name: RS232 Communication Server
 *    Author: Dr. Greg Nordstrom
 *   Created: 12/30/17
 *  Mod Date: 3/28/19, 1/17/21
 *  Modified: Jose De Los Santos
 *   ModDate: 11/16/2026
 * 
 * Processor: ATmega128A (on the ReadyAVR board)
 *
 * This program implements a communication server. Normally the server
 * simply echoes what was sent. However, there are two cases where
 * different behavior occurs:
 *    1. Pressing the ESC causes a soft reset (redisplays the title text)
 *    2. Typing the letters 'end' contiguously displays 'Bye' and halts
 *
 *****************************************************************************/

// define CPU speed (7.3728 MHz for the ATmega128A)
#define F_CPU   7372800UL

#define CR          0x0D
#define LF          0x0A
#define ESC         0x1B

#include <avr/io.h>
#include <util/delay.h>
#include "UARTLibrary.h"  // Double quotes for local library.

#define BAUDRATE 9600	  // 9600, 19200, 38400, 57600
#define DATABITS    8	  // 5-8
#define STOPBITS    1	  // 1,2
#define PARITY      0	  // 0=none, 1=odd, 2=even

#define LED0_DDR	DDRC  // LED0 is active-low
#define LED0_PORT	PORTC
#define LED0_BIT	PC0
#define LED0_ON()	(LED0_PORT &= ~ (1 << LED0_BIT) )
#define LED0_OFF()	(LED0_PORT |= (1 << LED0_BIT) )

// acknowledges that a character was received
// turns on for 75ms then off
void blink_led0(void)
{
	LED0_ON();
	_delay_ms(75);
	LED0_OFF();
}

// sends null-terminated string, one character at a time
void print_str(const char *s)
{
		unsigned char i = 0;
		while (s[i] != '\0')				// stop at null terminator
		{
			uart_tx(s[i]);					// character to send
			i++;							// on to the next one
		}
}

// start up display message
// repeats when a hard reset is triggered or "esc" is pressed
void print_startup(void)
{	
	print_str("\r\n\r\n");					// two CR-LF pairs
	print_str("RS232 Communications Server\r\n");
	print_str("Status: Active\r\n");
	print_str("Jose De Los Santos\r\n");
	print_str("EECE3624\r\n");
	print_str("Fall 2026\r\n\r\n");			// two CR-LF pairs: last line followed by another blank line		
}

int main(void)
{
	unsigned char c;						// holds received char
	unsigned char count = 0;				// letters "end" matched in a row 
	
	LED0_DDR |= (1 << LED0_BIT);			// makes LED0 pin an output
	LED0_OFF();

	uart_init(BAUDRATE, DATABITS, STOPBITS, PARITY);
	print_startup();
	while(1)
	{
		c = uart_rx();						// wait for PuTTY input
		blink_led0();						// acknowledges character receipt through LED blink
		
		if (c == ESC)						// Esc --> soft reset
		{
			print_startup();				// outputs the display message again
			count = 0;						// forget any partial letters from "end"
		}
		else
		{
			uart_tx(c);						// echo the input back
			if (c == CR)
			{
				uart_tx(LF);				// include LF, PuTTY only sends CR
			}
		
		// "end" special case exit
			if (count == 0 && c == 'e')
			{
				count = 1;					// an 'e' was detected, set count to 1
			}
			else if (count == 1 && c == 'n')
			{
				count = 2;					// currently at "en"
			}		
			else if(count == 2 && c == 'd')
			{
				count = 3;					// keyword "end" matched
			}
			else if (c == 'e')
			{
				count = 1;					// streak broken BUT starts a new "end"
			}
			else
			{
				count = 0;					// streak broken, restart count
			}
		
			if (count == 3)					// "end" keyword matched
			{
				print_str("\r\n\r\nBye");	// two CR-LF pairs followed by display message "Bye"
				while(1);					// STOP, all inputs ignored until hard reset is pressed
			}
		}
	}
	return 0;
}
