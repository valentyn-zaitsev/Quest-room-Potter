#define  F_CPU 1000000
#include <AVR/io.h>
#include <util/delay.h>
#define nop() {asm("nop");}

int chess_back = 0;
int play_switcher = 0;

int king_switcher = 0;
int officer_switcher = 0;
int door_open = 0;
int horse_switcher = 0;

void preset(void) {
	DDRB = 0b10111100;
	DDRC = 0b00111100;
	DDRD = 0b01100001;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;	
}

void delay(int t) {
	for (int i=0; i<t; i++) {
		nop();
	}
}

void longDelay(int t) {
	for (int i=0; i<t; i++) {
		delay(600);//было 300
	}
}

void kingDelay(void) {
	longDelay(5);
}

void officerDelay(void) {
	longDelay(3);
}

void king_forward(void) {
	PORTB = 0b00000100;
	kingDelay();
	PORTB = 0b00001100;
	kingDelay();
	PORTB = 0b00001000;
	kingDelay();
	PORTB = 0b00011000;
	kingDelay();
	PORTB = 0b00010000;
	kingDelay();
	PORTB = 0b00110000;
	kingDelay();
	PORTB = 0b00100000;
	kingDelay();
	PORTB = 0b00100100;
	kingDelay();
	PORTB = 0x00;
}

void king_back(void) {
	PORTB = 0b00100000;
	kingDelay();
	PORTB = 0b00110000;
	kingDelay();
	PORTB = 0b00010000;
	kingDelay();
	PORTB = 0b00011000;
	kingDelay();
	PORTB = 0b00001000;
	kingDelay();
	PORTB = 0b00001100;
	kingDelay();
	PORTB = 0b00000100;
	kingDelay();
	PORTB = 0b00100100;
	kingDelay();
	PORTB = 0x00;
}

void officer_back(void) {
	PORTC = 0b00000100;
	officerDelay();
	PORTC = 0b00001100;
	officerDelay();
	PORTC = 0b00001000;
	officerDelay();
	PORTC = 0b00011000;
	officerDelay();
	PORTC = 0b00010000;
	officerDelay();
	PORTC = 0b00110000;
	officerDelay();
	PORTC = 0b00100000;
	officerDelay();
	PORTC = 0b00100100;
	officerDelay();
	PORTC = 0x00;
}

void officer_forward(void) {
	PORTC = 0b00100000;
	officerDelay();
	PORTC = 0b00110000;
	officerDelay();
	PORTC = 0b00010000;
	officerDelay();
	PORTC = 0b00011000;
	officerDelay();
	PORTC = 0b00001000;
	officerDelay();
	PORTC = 0b00001100;
	officerDelay();
	PORTC = 0b00000100;
	officerDelay();
	PORTC = 0b00100100;
	officerDelay();
	PORTC = 0x00;
}

int main (void)
{
	preset();
	PORTD |= (1 << PIND5)|(1<<PIND6);//электромагниты шкафа и шахмат закрыты
	while(1) {
		//officer_back();
		//king_forward();
		_delay_ms(500);
		if (door_open == 1){
			PORTD &= ~(1<<PIND5);
		}
		if (!(PINB & (1<<6))) {//пазлы (сигнал от шаров)
			_delay_ms(3000);
			if (!(PINB & (1<<6))) {
				play_switcher = 1;
			}
		} 
		if (PINB & (1<<6)){
			play_switcher = 0;
			PORTD |= (1<<PIND6);
		}
		if (play_switcher == 1){
			PORTD &= ~(1<<PIND6);//отпереть электромагнит шахмат
			
			if (!(PIND & (1<<PIND3))) {//тура
				_delay_ms(250);
				if (!(PIND & (1<<PIND3))) {
					_delay_ms(500);
					if (officer_switcher == 0){
						for (int i=0; i<5000; i++) {//2590
							officer_forward();
							if (!(PIND & (1<<PIND7))){//концевик офицера вперед
								break;
							}
						}
						officer_switcher = 1;
					}
					//if ((!(PIND & (1<<PIND2)))&&(!(PIND & (1<<PIND2)))) {//конь
					if ((!(PIND & (1<<PIND2)))&&(!(PIND & (1<<PIND7)))) {//конь
						_delay_ms(250);
						if ((!(PIND & (1<<PIND2)))&&(!(PIND & (1<<PIND7)))) {
							if (horse_switcher == 0){
								PORTD |= (1 << PIND0);//звук коня
								//_delay_ms(5000);
								//PORTD &= ~(1 << PIND0);//выключить звук коня
								horse_switcher = 1;
							}
							if (king_switcher == 0){								
								for (int j=0; j<500; j++) {//492
									king_forward();
									if (!(PINC & (1<<PINC0))){//концевик короля вперед
										break;
									}
								}
							king_switcher = 1;
							}
							if (!(PIND & (1<<PIND1))){//королева
								_delay_ms(150);
								if (!(PIND & (1<<PIND1))){
									PORTD &= ~(1 << PIND0);
									PORTB |= (1 << PINB7);//звук падшего короля
									_delay_ms(1800);
									door_open = 1;
								} 
							}
						}
					}
				}
			}	
		}	
		if (!(PIND & (1<<PIND4))) {//back
			_delay_ms(50);
			if (!(PIND & (1<<PIND4))) {
				for (int i=0; i<5000; i++) {
					if (!(PINB & (1<<PINB0))){//концевик офицера назад
						break;
					}
					officer_back();
				}
				for (int i=0; i<5000; i++) {
					if (!(PINC & (1<<PINC1))){//концевик короля назад
						break;
					}
					king_back();
				}
			}
			PORTD = 0x00;
			PORTD = 0x00;
			DDRD = 0x00;
		}
	}
}
