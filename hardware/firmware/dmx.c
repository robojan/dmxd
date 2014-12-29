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
 * dmx.c
 *
 * Created: 25-12-2012 19:43:48
 *  Author: Robbert-Jan de Jager
 */ 

// DMX baudrate
#define BAUD 250000UL

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include "dmx.h"

// Keep track of the current state
enum DmxState{BREAK, MAB, DATA};
enum DmxState dmxState = BREAK;

// Dmx channels
uint8_t dmxChannels[CHANNELS];
volatile uint8_t channelCounter = 0;

void usart_init();
void usart_enable();
void usart_disable();
void timer_init();
void timer_enable();
void timer_disable();
void timer_delay(uint8_t wait);
	
// Initialize the uart module
void usart_init()
{
	DDRD |= (1<<PD1);
	UCSRA = (USE_2X << U2X);
	UCSRB = 0;
	UCSRC = (1<<USBS)|(3<<UCSZ0);
	UBRRL = UBRRL_VALUE;
	UBRRH = UBRRH_VALUE;
}

void usart_enable()
{
	UCSRB |= (1<<TXEN) | (1<<UDRIE);
}

void usart_disable()
{
	UCSRB &= ~((1<<TXEN) | (1<<UDRIE));
}

void timer_init()
{
	TCNT0 = 0;
	TCCR0A = 0x00;
	TCCR0B = 0x00; 
	OCR0A = 0;
	TIMSK |= (1<<OCIE0A);
}

void timer_delay(uint8_t wait)
{
	OCR0A = wait;
	timer_enable();
}

void timer_disable()
{
	TCNT0 = 0;
	TCCR0B &= ~(1<<CS01);
}

void timer_enable()
{
	TCNT0 = 0;
	TCCR0B |= (1<<CS01); // F_CPU/8	
}

ISR(TIMER0_COMPA_vect)
{
	switch(dmxState)
	{
	case BREAK: // execute break
		PORTD &= ~(1<<PD1); 
		timer_delay(176);
		dmxState = MAB;
		break;
	case MAB: // execute mark after break
		PORTD |= (1<<PD1);
		timer_delay(15);
		dmxState = DATA;
		break;
	case DATA: // transmit data
		timer_disable();
		usart_enable();
		UDR = 0;
		channelCounter = 0;
		break;
	}
}

ISR(USART_UDRE_vect)
{
	if(dmxState != DATA)
		return;
		
	UDR = dmxChannels[channelCounter];
	channelCounter++;
	if(channelCounter == CHANNELS)
	{
		dmxState = BREAK;
		usart_disable();
		timer_delay(10);
	}
}

void dmx_init()
{
	memset(dmxChannels, 0, sizeof(dmxChannels));
	
	usart_init();
	timer_init();
	dmxState = BREAK;
	sei();
	timer_delay(100);
}

uint8_t dmx_canChangeData()
{
	return dmxState == BREAK;
}