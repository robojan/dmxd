/*The MIT License (MIT)

Copyright (c) 2015 Robbert-Jan de Jager

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/*
 * spidmx.c
 *
 * Created: 23-12-2014 15:35:28
 *  Author: Robbert-Jan de Jager
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "dmx.h"
#include <string.h>

volatile uint8_t spi_inBuffer[16];
volatile uint8_t spi_outBuffer[16];
volatile uint8_t spi_counter = 0;

void spi_init()
{
	// Three wire mode (SPI)
	//
	DDRB = (DDRB & ~((1<<PB5) | (1<<PB7))) | (1<<PB6);
	// Interrupt on counter overflow
	// Counter value to 0 so 16 edges for an overflow
	USISR = 0;
	// Three wire mode 
	// Shift in on positive edge, counter increment on both edges
	USICR = (1<<USIOIE) | (1 << USIWM0) | (2<<USICS0);
}

ISR(USI_OVERFLOW_vect)
{
	spi_inBuffer[spi_counter] = USIDR;
	USIDR = spi_outBuffer[spi_counter];
	spi_counter = (spi_counter + 1) & 0xF;
	USISR |= (1<<USIOIF);
	TCNT1 = 0;
}

int main(void)
{
	uint8_t address; 
	// Initialize system
	spi_init();
	dmx_init();
	memset(dmxChannels, 0xFF, sizeof(dmxChannels));
	memset(spi_outBuffer, 0x00, sizeof(spi_outBuffer));
	
	// Setup timeout timer
	TCCR1A = 0;
	TCCR1B = (2<<CS10);
	TCCR1C = 0;
	TCNT1L = 0;
	
    while(1)
    {
		if(TCNT1 > 2000)
		{
			spi_counter = 0;
			TCNT1 = 0;
		}
		if(spi_counter >= 10)
		{
			TCNT1 = 0;
			spi_counter = 0;
			switch(spi_inBuffer[0])
			{
				case 0:
					// ignore
					break;
				case 1:
					// setChannel
					USIDR = 1;
					for(uint8_t i = 0; i < 4; i++)
					{
						if(spi_inBuffer[i*2+1] < CHANNELS)
						{
							dmxChannels[spi_inBuffer[i*2+1]-1] = spi_inBuffer[i*2+2];
							spi_outBuffer[i*2] = spi_inBuffer[i*2+1];
							spi_outBuffer[i*2+1] = spi_inBuffer[i*2+2];
						} else {
							spi_outBuffer[i*2] = 0xFF;
							spi_outBuffer[i*2+1] = 0;
						}
					}
					break;
				case 2:
					// getChannels
					USIDR = 2;
					address = spi_inBuffer[1];
					spi_outBuffer[0] = address;
					for(uint8_t i = 0; i< 8; i++)
					{
						if(address + i<=CHANNELS)
						{
							spi_outBuffer[i+1] = dmxChannels[address+i-1];	
						} else {
							spi_outBuffer[i+1] = 0;
						}
					}
					break;
				case 3:
					// set Channels
					USIDR = 3;
					address = spi_inBuffer[1];
					spi_outBuffer[0] = address;
					for(uint8_t i = 0; i< 8; i++)
					{
						if(address + i <= CHANNELS)
						{
							spi_outBuffer[i+1] = spi_inBuffer[i+2];
							dmxChannels[address + i - 1] = spi_inBuffer[i+2];
						} else {
							spi_outBuffer[i+1] = 0;
						}
					}
					break;
				case 4:
					// Get info
					USIDR = 4;
					memset(spi_outBuffer, 0, 10);
					spi_outBuffer[0] = CHANNELS;
					
					break;
			}
		}
    }
}