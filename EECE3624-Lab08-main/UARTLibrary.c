/*****************************************************************************
 *	    File: USARTLibrary.c
 *  Lab Name: (Needed for Lab07 and Lab08)
 *    Author: Dr. Greg Nordstrom
 *   Created: 3/3/2019
 *  Modified: Jose De Los Santos
 *  Mod Date: 11/16/2026
 * Processor: ATmega128A (on the ReadyAVR board)
 *
 * This is intended to be compiled as an AVR Library Project and
 * used by other programs. After compiling this code, copy the
 * libUSART1Library.a and USART1Library.h files to the project directory,
 * then add the library to the target project by R-clicking on the
 * project's Libraries directory and clicking Add Library. Also,
 * the USART1Library.h file must be #included in the project's .c file.
 *
 * NOTE: This library only works with the default USART1 port
 * on the ReadyAVR (so JP2 must have jumpers on PD3 and PD2).
 */

#include <avr/io.h>
#include "UARTLibrary.h"

// Use USART1_init to initialize the USART1 hardware in the AVR processor.
// The function takes the following arguments:
//    - desired baud rate ( 9600, 19200, 38400, 57600),
//    - number of databits (5-8)
//    - number of stop bits (1,2),
//    - parity (0=no parity, 1=odd parity, 2=even parity).
// No error checking is performed and the function does not return a value.
//
// For example,
//     usart1_init(9600,8,1,0);
// sets the baudrate to 9600, with 8 data bits, 1 stop bit, and no parity
void uart_init(long unsigned int baudrate, unsigned char databits, unsigned char stopbits, unsigned char parity) {

	unsigned int ubrr;							// baud rate register value
	
	// set baud rate (via UBRR1 *H*igh and *L*ow registers)
	ubrr = (F_OSC / (16UL * baudrate)) - 1;		// asynchronous normal mode ( U2X1 = 0 )
	UBRR1H = (unsigned char)(ubrr >> 8);		// upper 4 bits
	UBRR1L = (unsigned char)ubrr;				// lower 8 bits
	
	// set databits (options 7 or 8)
	if (databits == 7)
	{
		UCSR1C = (1 << UCSZ11);					// 7 databits option selected
	}
	else
	{
		UCSR1C = (1 << UCSZ11)|(1 << UCSZ10);	// 8 databits option selected
	}
	    
	// set parity
	if (parity == 1)
	{	
		UCSR1C |= (1 << UPM11)| (1 << UPM10);	// sets bits for odd parity
	}
	else if (parity == 2)
    {
	   UCSR1C |= (1 << UPM11);					// sets bits for even parity
   }
	
	// parity = 0 UPM11:10 remain 0
   
    // set stopbits
	if (stopbits == 2)
	{
		UCSR1C |= (1 << USBS1);					// 2 stop bit option	
	}
	// stop bit = 0 USBS1 remains 0
	
	// receiver and transmitter enable
    UCSR1B = (1 << RXEN1)|(1 << TXEN1);			// enables receiver and transmitter
	// writing UCSR1B clears UCSZ12, which helps set bits for 7/8 data bits
	
}

// This function waits for space in the USART transmit buffer. When space
// is available, the function places an unsigned character value (passed in
// as the function's only argument) in the buffer and returns. No error
	// checking is performed and the function does not return a value.
void uart_tx(unsigned char data) {
	while(!(UCSR1A & (1 << UDRE1)));			// wait until UDR1 is emptY, UDRE1 = 1 
	UDR1 = data;								// load the byte and send it to TXD1
}

// This function blocks until a character is received in the USART input
// buffer. Once a character arrives, it is passed back to the caller as
// an unsigned character. No error checking is performed.
unsigned char uart_rx(void) {
	while (!(UCSR1A & (1 << RXC1)));			// wait until byte arrives, RXC1 = 1
	return UDR1;								// read it and clear it
}