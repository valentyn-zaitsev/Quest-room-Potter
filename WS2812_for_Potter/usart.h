#ifndef USART_H_
#define USART_H_
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdbool.h>
#include <avr/interrupt.h>

void USART_Init( unsigned int ubrr);
void USART_Transmit( unsigned char data );



#endif /* USART_H_ */