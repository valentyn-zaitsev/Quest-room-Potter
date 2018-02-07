#ifndef USART_H_
#define USART_H_

//#include "main.h"

#include <avr/io.h>

void USART_Init( unsigned int ubrr);
void USART_Transmit( unsigned char data );



#endif /* USART_H_ */