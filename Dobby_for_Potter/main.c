#include "main.h"

#define RXUBRR (F_CPU/16/9600)-1

#define BUFFER_SIZE 6
char buffer[BUFFER_SIZE];
unsigned int count = 0;

void timer0_init(void) {
	TCCR0=(1<<CS00)|(1<<CS02); //1024 предделитель
	//asm("sei");
	sei();
}

void port_ini(void)
{
	DDRD |= (1<<PIND2);//соленоидный замок
	PORTD &= ~(1<<PIND2);
	DDRC |= (1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5);//поднесение носка, анектоды
	PORTC &= ~((1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5));
}
 
 ISR (TIMER0_OVF_vect) {
	 count++;
	 if (count > 700){
		PORTD |= (1<<PIND2);//открыть шкатулку
		_delay_ms(250);
		PORTD &= ~(1<<PIND2);//убрать питание с соленоидного замка
		DDRD = 0x00;
		cli();
	 }
 }

int main(void)
{
	int j=0;
	uchar card_num[5];
	port_ini();
	MFRC522_Init();
	timer0_init();
	_delay_ms(10);
	while(1)
	{
		if ( MFRC522_Request( PICC_REQIDL, card_num ) == MI_OK ) {
			if ( MFRC522_Anticoll( card_num ) == MI_OK ) {
				if (card_num[1]==232){//носок
					PORTC |= (1<<PINC1);//сигнал поднесения носка
					TIMSK=(1<<TOIE0); // Запускаем таймер
				} 
				else {
					PORTC &= ~(1<<PINC1);
					PORTD &= ~(1<<PIND2);
				}
				
			}
		}
		
		if ( MFRC522_Request( PICC_REQIDL, card_num ) == MI_OK ) {
			if ( MFRC522_Anticoll( card_num ) == MI_OK ) {
				if (card_num[1]==121){//баночка (анекдоты)
					j++;
					switch (j){
						case 1: PORTC |= (1<<PINC5); _delay_ms(4000); break;
						case 2: PORTC |= (1<<PINC4); _delay_ms(4000); break;
						case 3: PORTC |= (1<<PINC3); _delay_ms(4000); break;
						case 4: PORTC |= (1<<PINC2); _delay_ms(4000); break;
						case 5: PORTC &= ~((1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5)); j = 0; break;
					}
				}
				else{
					PORTC &= ~((1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5));
				}
				
			}
		}
	}
}
