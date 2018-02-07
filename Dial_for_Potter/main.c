#define F_CPU 1000000L
#include <AVR/io.h>
#include <util/delay.h>

void preset(void) {
	DDRD = 0x00;
	DDRB = 0b00111110;
	DDRC = 0xff;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
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

int isOn(volatile uint8_t port_diod, int pin_diod) {
	return port_diod & (1 << pin_diod) ? 1 : 0;
}

int main (void)
{
	preset();
	int number1 = 0, number2 = 1, number3 = 2, number4 = 3, number5 = 4, number6 = 6, number7 = 7, number8 = 5, number9 = 6, number10 = 7;
	int switcher1 = 1, switcher2 = 1, switcher3 = 1, switcher4 = 1, switcher5 = 1, switcher6 = 1, switcher7 = 1, switcher8 = 1, switcher9 = 1, switcher10 = 1;
	while(1) {
		if (isOn(PINB, 0) == 1) {
			number(number1, &switcher1, &PIND, &PORTC, PINC4);
			number(number2, &switcher2, &PIND, &PORTC, PINC3);
			number(number3, &switcher3, &PIND, &PORTC, PINC2);
			number(number4, &switcher4, &PIND, &PORTC, PINC1);
			number(number5, &switcher5, &PIND, &PORTC, PINC0);
			number(number6, &switcher6, &PINB, &PORTC, PINC5);
			number(number7, &switcher7, &PINB, &PORTB, PINB5);
			number(number8, &switcher8, &PIND, &PORTB, PINB4);
			number(number9, &switcher9, &PIND, &PORTB, PINB3);
			number(number10, &switcher10, &PIND, &PORTB, PINB2);
			
			if (isOn(PORTC, PINC4) == 0 &&
			isOn(PORTC, PINC3) == 0 &&
			isOn(PORTC, PINC2) == 0 &&
			isOn(PORTC, PINC1) == 1 && // 4 is on
			isOn(PORTC, PINC0) == 0 &&
			isOn(PORTC, PINC5) == 0 &&
			isOn(PORTB, PINB5) == 0 &&
			isOn(PORTB, PINB4) == 1 && // 8 is on
			isOn(PORTB, PINB3) == 0 &&
			isOn(PORTB, PINB2) == 0){
				PORTB = 0x00;
				PORTC = 0x00;
				PORTD = 0x00;
				PORTB |= /*(1 << PINB0)|*/(1 << PINB1);
				PORTB |= (1<<PINB4);
				PORTC |= (1<<PINC1);
				_delay_ms(5000);
				PORTB &= ~(1<<PINB4);
				PORTC &= ~(1<<PINC1);
			}
		}
	}
}
