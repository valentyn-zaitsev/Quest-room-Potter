#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"

int dobby_free = 0;
int anekdot1 = 0;
int anekdot2 = 0;
int anekdot3 = 0;
int anekdot4 = 0;
int kniga = 0;
int shkaf = 0;
int zaslonka_labirinta = 0;
int sunduk = 0;
int banochka_zeliya = 0;
int shkatulka_dobby = 0;
int nagatie_kirpicha = 0;
int kirpichi_razgadany = 0;
int boi_sila_griffindora = 0;
int boi_sila_kogtevran = 0;
int boi_sila_slizerina = 0;
int nachalo_boya = 0;
int boy_prervan = 0;
int podarok = 0;
int second_room = 0;

void port_init (void)
{
	DDRD &= ~((1<<PIND7)|(1<<PIND6)|(1<<PIND5)|(1<<PIND4)|(1<<PIND3)|(1<<PIND2));
	//PORTD &= ~(1<<PIND7);
	
	DDRC &= ~((1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3));
	DDRC |= (1<<PINC7);
	PORTC &= ~(1<<PINC7);
	
	DDRA &= ~((1<<PINA3)|(1<<PINA4)|(1<<PINA5)|(1<<PINA6)|(1<<PINA7));
	DDRA |= (1<<PINA0)|(1<<PINA1)|(1<<PINA2);//RGB казана
	PORTA &= ~((1<<PINA0)|(1<<PINA1)|(1<<PINA2));
	
	DDRB &= ~((1<<PINB1)|(1<<PINB2)|(1<<PINB3));
	DDRB |= (1<<PINB0)|(1<<PINB4)|(1<<PINB7);//подарок, казан, сигнал открыти€ шахмат
	PORTB &= ~((1<<PINB0)|(1<<PINB4)|(1<<PINB7));
	//PORTC &= ~((1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3));
}

ISR(USART_RXC_vect){
	
	
	if (UDR == 5){// азан, ƒобби, шкатулки
		PORTB |= (1<<PINB4);
	}
	
	if (dobby_free == 1 || kniga == 1 || zaslonka_labirinta == 1 || sunduk == 1 || banochka_zeliya == 1 || nagatie_kirpicha == 1 || kirpichi_razgadany == 1){
		if (UDR == 3){//RED
		PORTA |= (1<<PINA0);
		PORTA &= ~((1<<PINA1)|(1<<PINA2));
		}
	
		if (UDR == 1){//GREEN
		PORTA |= (1<<PINA1);
		PORTA &= ~((1<<PINA0)|(1<<PINA2));
		}
		
		if (UDR == 2){//BLUE
		PORTA |= (1<<PINA2);
		PORTA &= ~((1<<PINA0)|(1<<PINA1));
		}
		
		if (UDR == 0){//CLEAR LED
			/*if (PINA & (1<<0)){
				PORTA ^= (1<<PINA0);
			}
		
			if (PINA & (1<<1)){
				PORTA ^= (1<<PINA1);
			}
		
			if (PINA & (1<<2)){
				PORTA ^= (1<<PINA2);
			}*/

			//PORTA &= ~((1<<PINA0)|(1<<PINA1)|(1<<PINA2));
		}	
	}else{
		PORTA &= ~((1<<PINA0)|(1<<PINA1)|(1<<PINA2));
		}
		
	if (UDR == 4){//подарок
		if (podarok == 0){
			PORTB |= (1<<PINB0);
			_delay_ms(500);
			PORTB &= ~(1<<PINB0);
			podarok = 1;
		}
	}
}

int main(void)
{
	port_init();
    USART_Init(34);//28800
	sei();
	PORTA &= ~((1<<PINA0)|(1<<PINA1)|(1<<PINA2));//светодиод в казане
	_delay_ms(1000);
    while (1) 
    {
		//_delay_ms(2000);	
		if (zaslonka_labirinta == 1 && shkatulka_dobby == 1 && kirpichi_razgadany == 1){
			_delay_ms(3000);
			if (zaslonka_labirinta == 1 && shkatulka_dobby == 1 && kirpichi_razgadany == 1){
				//_delay_ms(5000);
				PORTB |= (1<<PINB7);//сигнал открывающий шахматы
				PORTA &= ~((1<<PINA0)|(1<<PINA1)|(1<<PINA2));
				second_room = 1;
				//открыть шахматы
			}
		} else if ((PINA & (1<<5)) || (PIND & (1<<6)) || (PINC & (1<<4))){
			PORTB &= ~(1<<PINB7);//сигнал открывающий шахматы в 0
		}
		
		if (!(PIND & (1<<7))){
			if (dobby_free == 0){
				USART_Transmit('4');
				dobby_free = 1;
			}
		}
		
		if (!(PINC & (1<<0))){
			if (anekdot1 == 0){
				USART_Transmit('5');
				anekdot1 = 1;
			}
		}
		
		if (!(PINC & (1<<1))){
			if (anekdot2 == 0){
				USART_Transmit('6');
				anekdot2 = 1;
			}
		}
		
		if (!(PINC & (1<<2))){
			if (anekdot3 == 0){
				USART_Transmit('7');
				anekdot3 = 1;
			}
		}
		
		if (!(PINC & (1<<3))){
			if (anekdot4 == 0){
				USART_Transmit('8');
				anekdot4 = 1;
			}
		}
		
		if (!(PINA & (1<<3))){
			_delay_ms(200);
			if (!(PINA & (1<<3))){
				if (kniga == 0){
					USART_Transmit('3');
					kniga = 1;
				}
			}
		}
		
		if (!(PINA & (1<<4))){
			_delay_ms(200);
			if (!(PINA & (1<<4))){
				if (shkaf == 0){
					USART_Transmit('d');
					shkaf = 1;
				}
			}
		}
		
		if (!(PINA & (1<<5))){
			_delay_ms(450);
			if (!(PINA & (1<<5))){
				if (zaslonka_labirinta == 0){
					USART_Transmit('2');
					zaslonka_labirinta = 1;
				}
			}
		}
		
		if (!(PINA & (1<<6))){
			_delay_ms(200);
			if (!(PINA & (1<<6))){
				if (sunduk == 0){
					USART_Transmit('1');
					sunduk = 1;
				}
			}
		}
		
		if (!(PINA & (1<<7))){
			_delay_ms(200);
			if (!(PINA & (1<<7))){
				if (banochka_zeliya == 0){
					USART_Transmit('p');
					banochka_zeliya = 1;
					PORTC |= (1<<PINC7);//открыть шкатулку с подсказкой к двери
					_delay_ms(230);
					PORTC &= ~(1<<PINC7);
				}
			}
		}
		
		if (!(PIND & (1<<6))){
			_delay_ms(300);
			if (!(PIND & (1<<6))){
				if (shkatulka_dobby == 0){
					USART_Transmit('9');
					shkatulka_dobby = 1;
				}
			}
		}
		
		if (!(PINC & (1<<5))){
			if (nagatie_kirpicha == 0){
				USART_Transmit('a');
				nagatie_kirpicha = 1;
			}
		}
		
		if (!(PINC & (1<<4))){
			if (kirpichi_razgadany == 0){
				USART_Transmit('b');
				kirpichi_razgadany = 1;
			}
		}
		
		if (second_room == 1){
			//2nd room
			if (!(PINB & (1<<3))){
				if (boi_sila_griffindora == 0){
					USART_Transmit('n');
					boi_sila_griffindora = 1;
				}
			}
			
			if (!(PIND & (1<<2))){
				if (boi_sila_slizerina == 0){
					USART_Transmit('l');;
					boi_sila_slizerina = 1;
				}
			}
			
			if (!(PIND & (1<<3))){
				if (boi_sila_kogtevran == 0){
					USART_Transmit('m');
					boi_sila_kogtevran = 1;
				}
			}
			
			if (!(PINB & (1<<2))){//начало/возобновление бо€
				_delay_ms(350);
				if (!(PINB & (1<<2))){//начало/возобновление бо€
					if (nachalo_boya == 0){
						USART_Transmit('e');
						nachalo_boya = 1;
						boy_prervan = 0;
					}
				}
			}
			
			if (!(PINB & (1<<1))){//прерывание бо€
				_delay_ms(700);
				if (!(PINB & (1<<1))){//прерывание бо€
					if (boy_prervan == 0){
						USART_Transmit('g');
						boy_prervan = 1;
						nachalo_boya = 0;
					}
				}
			}
		}
			
    }
}