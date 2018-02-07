#define F_CPU 1000000
#include <AVR/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#define nop() {asm("nop");}

int ball_count = 4;

void down (void);
void up (void);

unsigned int count=0;
int d = 0;
bool cycle_compl = false;
int n = 0;
bool two_sec = false;
bool reset = false;
bool door = true;

void preset (void) {
	DDRB = 0b01000000;
	//DDRB = 0xff;
	DDRC = 0b00111100;
	DDRD = 0b11110010;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
}

void timer0_init(void) {
	TCCR0=(1<<CS00)|(1<<CS01); //64 предделитель
	TIMSK=(1<<TOIE0); // «апускаем таймер
	//asm("sei");
	sei();
}

void down(){
	cycle_compl = true;
}

void up(){
	if (cycle_compl){
		d++;
		
// 		switch(d){
// 			case 1: PORTB |= (1<<PINB0);break;
// 			case 2: PORTB |= (1<<PINB0)|(1<<PINB1);break;
// 			case 3: PORTB |= (1<<PINB0)|(1<<PINB1)|(1<<PINB2);break;
// 			case 4: PORTB |= (1<<PINB0)|(1<<PINB1)|(1<<PINB2)|(1<<PINB3);break;
// 			case 5: PORTB |= (1<<PINB0)|(1<<PINB1)|(1<<PINB2)|(1<<PINB3)|(1<<PINB4);break;
// 		}
		if (d == 2){
			n = 1;
			reset = true;
		}
		if (d == 6){
			n = 2;
			reset = true;
		}
		if (d == 8){
			n = 3;
			reset = true;
		}
		cycle_compl=false;	
	}
}

void delay(int t) {
	for (int i=0; i<t; i++) {
		nop();
	} 
}

void longDelay(int t) {
	for (int i=0; i<t; i++) {
		delay(300);
	}
}

void ballDelay(void) {
	longDelay(2);
}

ISR (TIMER0_OVF_vect) {
	count++;
	
	switch(n){
	case 1:
		if (count % 16 == 0){
			PORTC ^= (1<<PINC2);//светодиод
		}
		break;
	case 2:
		if (count % 4 == 0){
			PORTC ^= (1<<PINC2);//светодиод
		}
		break;
	case 3:
		PORTC |= (1<<PINC2);//светодиод
		_delay_ms(4000);
		PORTC &= ~(1<<PINC2);//потушить светодиод
		PORTC &= ~(1<<PINC3);//открыть дверь
		cli();
		TIMSK = 0x00;
		two_sec = false;
		door = false;
		break;
	}
	
	if (d == 0){
		if (count % 40 == 0){
			PORTC ^= (1<<PINC2);//светодиод
		}
	}
	
	if (count > 300 && two_sec){//5sec
		two_sec = false;
	}
	
	if (reset){
		count = 0;
		two_sec = true;
		reset = false;
	}
	
	if (count > 1000){//в случае "просто€" обнул€емс€
		d = 0;
		count = 0;
	}
}

void ball(void) {
	PORTD = 0b00010000;
	ballDelay();
	PORTD = 0b00110000;
	ballDelay();
	PORTD = 0b00100000;
	ballDelay();
	PORTD = 0b01100000;
	ballDelay();
	PORTD = 0b01000000;
	ballDelay();
	PORTD = 0b11000000;
	ballDelay();
	PORTD = 0b10000000;
	ballDelay();
	PORTD = 0b10010000;
	ballDelay();
	PORTD = 0x00;
}

void give_next_ball(int *give_ball, int engine_count) {
	if (*give_ball == 1 && ball_count > 0) {
		*give_ball = 0;
		ball_count--;
		for (int i=0; i<engine_count; i++) {
			ball();
		}
	}
}

void number(int pin_gerkon, int *switcher, volatile uint8_t *port_gerkon, volatile uint8_t *port_diod, int pin_diod) {
	if (!(*port_gerkon & (1 << pin_gerkon))) {
		_delay_ms(60);
		if (!(*port_gerkon & (1 << pin_gerkon))) {
			if (*switcher == 1){
				*port_diod |= 1 << pin_diod;
				*switcher = 2;
				} else if (*switcher == 2){
				*port_diod &= ~(1 << pin_diod);
				*switcher = 1;
			}
		}
	}
}

int main (void)
{
	preset();
	timer0_init();
	int give_ball0 = 1, give_ball1 = 1, give_ball2 = 1, give_ball3 = 1, give_ball4 = 1;
	int give_map = 1;
	int is_working = 0;
	int number11 = 5, number12 = 7;
	int switcher11 = 1, switcher12 = 1;
	
	PORTC |= (1<<PINC3);//фейкова€ дверь заперта
	
	while(1){
		if (door){	
			if (two_sec) {
				//PORTB &= ~(1<<PINB7);
				PORTC &= ~(1<<PINC2);//тушим светодиод во врем€ двухсекундной паузы
				continue;
			}
			//PORTB |= (1<<PINB7);
			if (!(PINC & (1<<1))) {
				_delay_ms(30);
				if (!(PINC & (1<<1))) {
					up();
				}
			}
			if (!(PINC & (1<<0))) {
				_delay_ms(30);
				if (!(PINC & (1<<0))) {
					down();
				}
			}
		}
		
		number(number11, &switcher11, &PINB, &PORTC, PINC5);
		number(number12, &switcher12, &PINB, &PORTC, PINC4);
		
		if (PINB & (1<<0)) {
			is_working = 1;
		}
		if (is_working == 1 && !(PINC & (1<<4)) && !(PINC & (1<<5))) {
			PORTB |= (1 << PINB6);//LED_strip_ON
			give_next_ball(&give_ball0, 35);
			if (!(PINB & (1<<1))) {
				give_next_ball(&give_ball1, 27);
			}
			if (!(PINB & (1<<2))) {
				give_next_ball(&give_ball2, 27);
			}
			if (!(PINB & (1<<3))) {
				give_next_ball(&give_ball3, 27);
			}
			if (!(PINB & (1<<4))) {
				give_next_ball(&give_ball4, 27);
			}
			
			if (!(PINB & (1<<1)) && !(PINB & (1<<2)) && !(PINB & (1<<3)) && !(PINB & (1<<4))) {
				
				if (give_map == 1) {
					give_map = 0;
					PORTD |= (1<<PIND1);//подн€ть заслонку
					_delay_ms(300);
					PORTD &= ~(1<<PIND1);//убрать питание с центрального замка
					PORTB &= ~(1<<PINB6);//LED_strip_OFF
					DDRB = 0x00;
				}
			}
		}
	}
}
