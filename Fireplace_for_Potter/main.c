#define F_CPU 1000000
#include <AVR/io.h>
#include <util/delay.h>
#include <stdbool.h>
#define nop() {asm("nop");}
	
bool first_brick_0 = false;
bool first_brick_1 = false;
bool second_brick_0 = false;
bool second_brick_1 = false;
bool third_brick_0 = false;
bool third_brick_1 = false;
bool fourth_brick_0 = false;
bool fourth_brick_1 = false;
bool fifth_brick_0 = false;
bool fifth_brick_1 = false;
bool sixth_brick_0 = false;
bool sixth_brick_1 = false;
bool seventh_brick_0 = false;
bool seventh_brick_1 = false;

bool one_right = false;
bool two_right = false;
bool three_right = false;

bool any_brick = false;
bool start_after_any = false;

	
void preset (void) {
	DDRB |= (1<<PINB0)|(1<<PINB1)|(1<<PINB2)|(1<<PINB3)|(1<<PINB4)|(1<<PINB5);
	DDRB &= ~((1<<PINB6)|(1<<PINB7));
	PORTB &= ~((1<<PINB0)|(1<<PINB1)|(1<<PINB2)|(1<<PINB3)|(1<<PINB4)|(1<<PINB5));
	
	DDRD &= ~((1<<PIND0)|(1<<PIND1)|(1<<PIND2)|(1<<PIND3)|(1<<PIND4));
	DDRD |= (1<<PIND5)|(1<<PIND6);
	PORTD &= ~((1<<PIND5)|(1<<PIND6));
	
	DDRC |= (1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5);
	PORTC &= ~((1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5));
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

void eye(void) {
	PORTB = 0b00000010;
	longDelay(5);
	PORTB = 0b00000110;
	longDelay(5);
	PORTB = 0b00000100;
	longDelay(5);
	PORTB = 0b00001100;
	longDelay(5);
	PORTB = 0b00001000;
	longDelay(5);
	PORTB = 0b0001100;
	longDelay(5);
	PORTB = 0b00010000;
	longDelay(5);
	PORTB = 0b00010010;
	longDelay(5);
	PORTB = 0x00;
}
int main(void)
{
	preset();
    while (1) 
    {
		//если нажимается любая кирпичина
		if ((any_brick == false)&&((!(PIND & (1<<0)))||(!(PIND & (1<<1)))||(!(PIND & (1<<2)))||(!(PIND & (1<<3)))||(!(PIND & (1<<4)))||(!(PINB & (1<<6)))||(!(PINB & (1<<7))))){
			_delay_ms(25);
			if ((any_brick == false)&&((!(PIND & (1<<0)))||(!(PIND & (1<<1)))||(!(PIND & (1<<2)))||(!(PIND & (1<<3)))||(!(PIND & (1<<4)))||(!(PINB & (1<<6)))||(!(PINB & (1<<7))))){
			any_brick = true;
			}
		}
		
		
		if (any_brick == true){
			if (!start_after_any){
				//отправить сигнал на центральную плату
				PORTD |= (1<<PIND5);
				//засветить все кирпичи
				PORTC |= (1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5);
				PORTB |= (1<<PINB5);
				_delay_ms(500);
				PORTC &= ~((1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5));
				PORTB &= ~(1<<PINB5);
				for (int i=0; i<5;i++){
					eye();
				}//убрать глаз
			start_after_any = true;
			}
			
			//если нажимаются все кирпичи (все подсвечиваются) - тушим
			if ((PINC & (1<<5))&&(PINC & (1<<4))&&(PINC & (1<<3))&&(PINC & (1<<2))&&(PINC & (1<<1))&&(PINC & (1<<0))&&(PINB & (1<<5))){
				_delay_ms(300);
				PORTC &= ~((1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5));
				PORTB &= ~(1<<PINB5);
			}
			
			if (!(PIND & (1<<7))) {//фототранзистор (юзер посмотрел в картину)
				_delay_ms(1500);
				if (!(PIND & (1<<7))) {//фототранзистор (юзер посмотрел в картину)
					second_brick_0 = false;//нужно, чтобы когда нажата одна из верных кнопок, юзер смотрит в картину (а кнопки потухнут), кнопки продолжили работать
					second_brick_1 = false;
					fifth_brick_0 = false;
					fifth_brick_1 = false;
					seventh_brick_0 = false;
					seventh_brick_1 = false;
					
					_delay_ms(100);
					PORTC |= (1<<PINC4);
					_delay_ms(100);
					PORTC &= ~(1<<PINC4);
					_delay_ms(200);
					PORTC |= (1<<PINC1);
					_delay_ms(100);
					PORTC &= ~(1<<PINC1);
					_delay_ms(200);
					PORTB |= (1<<PINB5);
					_delay_ms(100);
					PORTB &= ~(1<<PINB5);
				}
			}
			
			//верная комбинация
			if ( (three_right == true)&&(!(PINC & (1<<5)))&&(PINC & (1<<4))&&(!(PINC & (1<<3)))&&(!(PINC & (1<<2)))&&(PINC & (1<<1))&&(!(PINC & (1<<0)))&&(PINB & (1<<5)) ){
				//отправить сигнал на центральную плату
				PORTD |= (1<<PIND6);
				
				PORTC &= ~((1<<PINC4)|(1<<PINC1));
				PORTB &= ~(1<<PINB5);
				_delay_ms(150);
				PORTC |= (1<<PINC4)|(1<<PINC1);
				PORTB |= (1<<PINB5);
				_delay_ms(250);
				PORTC |= (1<<PINC4)|(1<<PINC1);
				PORTB |= (1<<PINB5);
				_delay_ms(150);
				PORTC &= ~((1<<PINC4)|(1<<PINC1));
				PORTB &= ~(1<<PINB5);
				
				PORTC &= ~((1<<PINC4)|(1<<PINC1));
				PORTB &= ~(1<<PINB5);
				_delay_ms(150);
				PORTC |= (1<<PINC4)|(1<<PINC1);
				PORTB |= (1<<PINB5);
				_delay_ms(250);
				PORTC |= (1<<PINC4)|(1<<PINC1);
				PORTB |= (1<<PINB5);
				_delay_ms(150);
				PORTC &= ~((1<<PINC4)|(1<<PINC1));
				PORTB &= ~(1<<PINB5);
				
				_delay_ms(1500);
				PORTB |= (1<<PINB0);//открыть картину
				_delay_ms(1200);
				PORTB &= ~(1<<PINB0);
			}
			
			if (!(PIND & (1<<0))) {//1 кирпичина
				_delay_ms(25);
				if (!(PIND & (1<<0))) {
					if ((PINC & (1<<5))&&(first_brick_0 == true)&&(first_brick_1 == false)){
							PORTC &= ~(1<<PINC5);
							first_brick_1 = true;
							first_brick_0 = false;
							_delay_ms(500);
						
					}
					if ( (!(PIND & (1<<0)))&&(first_brick_0 == false)){
							PORTC |= (1<<PINC5);
							first_brick_0 = true;
							first_brick_1 = false;
							_delay_ms(500);
					}
				}
			}

			
			if (!(PIND & (1<<1))) {//2 кипричина ***
				_delay_ms(25);
				if (!(PIND & (1<<1))) {
					if ((PINC & (1<<4))&&(second_brick_0 == true)&&(second_brick_1 == false)){
						PORTC &= ~(1<<PINC4);
						if ((!(PINC & (1<<5)))&&(PINC & (1<<4))&&(!(PINC & (1<<3)))&&(!(PINC & (1<<2)))&&(PINC & (1<<1))&&(!(PINC & (1<<0)))&&(PINB & (1<<5))){
							one_right = false;
						}
						second_brick_1 = true;
						second_brick_0 = false;
						_delay_ms(500);
					}
					if ( (!(PIND & (1<<1)))&&(second_brick_0 == false)){
						PORTC |= (1<<PINC4);
						one_right = true;
						second_brick_0 = true;
						second_brick_1 = false;
						_delay_ms(500);
					}
				}
			}
			
			if (!(PIND & (1<<2))) {//3 кипричина
				_delay_ms(205);
				if (!(PIND & (1<<2))) {
					if ((PINC & (1<<3))&&(third_brick_0 == true)&&(third_brick_1 == false)){
						PORTC &= ~(1<<PINC3);
						third_brick_1 = true;
						third_brick_0 = false;
						_delay_ms(500);
					}
					if ( (!(PIND & (1<<2)))&&(third_brick_0 == false)){
						PORTC |= (1<<PINC3);
						third_brick_0 = true;
						third_brick_1 = false;
						_delay_ms(500);
					}
				}
			}
			
			if (!(PIND & (1<<3))) {//4 кипричина
				_delay_ms(25);
				if (!(PIND & (1<<3))) {
					if ((PINC & (1<<2))&&(fourth_brick_0 == true)&&(fourth_brick_1 == false)){
						PORTC &= ~(1<<PINC2);
						fourth_brick_1 = true;
						fourth_brick_0 = false;
						_delay_ms(500);
					}
					if ( (!(PIND & (1<<3)))&&(fourth_brick_0 == false)){
						PORTC |= (1<<PINC2);
						fourth_brick_0 = true;
						fourth_brick_1 = false;
						_delay_ms(500);
					}
				}
			}
			
			if (!(PIND & (1<<4))) {//5 кипричина ***
				_delay_ms(25);
				if (!(PIND & (1<<4))) {
					if ((PINC & (1<<1))&&(fifth_brick_0 == true)&&(fifth_brick_1 == false)){
						PORTC &= ~(1<<PINC1);
						two_right = false;
						fifth_brick_1 = true;
						fifth_brick_0 = false;
						_delay_ms(500);
					}
					if ( (!(PIND & (1<<4)))&&(fifth_brick_0 == false)){
						PORTC |= (1<<PINC1);
						if (one_right == true){
							two_right = true;
						}
						fifth_brick_0 = true;
						fifth_brick_1 = false;
						_delay_ms(500);
					}
				}
			}
			
			if (!(PINB & (1<<6))) {//6 кипричина
				_delay_ms(25);
				if (!(PINB & (1<<6))) {
					if ((PINC & (1<<0))&&(sixth_brick_0 == true)&&(sixth_brick_1 == false)){
						PORTC &= ~(1<<PINC0);
						sixth_brick_1 = true;
						sixth_brick_0 = false;
						_delay_ms(500);
					}
					if ( (!(PINB & (1<<6)))&&(sixth_brick_0 == false)){
						PORTC |= (1<<PINC0);
						sixth_brick_0 = true;
						sixth_brick_1 = false;
						_delay_ms(500);
					}
				}
			}
			
			if (!(PINB & (1<<7))) {//7 кипричина ***
				_delay_ms(25);
				if (!(PINB & (1<<7))) {
					if ((PINB & (1<<5))&&(seventh_brick_0 == true)&&(seventh_brick_1 == false)){
						PORTB &= ~(1<<PINB5);
						seventh_brick_1 = true;
						seventh_brick_0 = false;
						_delay_ms(500);
					}
					if ( (!(PINB & (1<<7)))&&(sixth_brick_0 == false)){
						PORTB |= (1<<PINB5);
						if (two_right == true){
							three_right = true;
						}
						seventh_brick_0 = true;
						seventh_brick_1 = false;
						_delay_ms(500);
					}
				}
			}
		}
		
    }
}