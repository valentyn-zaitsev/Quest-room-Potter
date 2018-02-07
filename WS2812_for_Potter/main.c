#ifndef F_CPU
  #define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdbool.h>
#include <avr/interrupt.h>
 
#include "ws2812.h"
#include "twi.h"
#include "RTC.h"
#include "usart.h"
 
// Порт, скорость и способ подключения (одна, или две линии) определяется в ws2812.h
 
#define LED_MIDDLE 137 //светодиод который моргает, когда юзеру нужно поднести палочку к точке
#define LED_COUNT 276 // Общее количество светодиодов
#define LED_BRIGHTNESS 5 // Яркость эффекта (1 - 255)
#define DELAY_START 6
#define DELAY_TREMBLE 10
#define OFFSET_TREMBLE 5
 
#define GREEN_G 100.0
#define GREEN_R 30.0
#define GREEN_B 0
 
#define RED_R 100.0
#define RED_G 15.0
#define RED_B 5.0
 
#define LED_DLITELNOST_STOLKNOVENIA 210 //7 sec происходит просто столкновение волн
#define LED_BEFORE_DAMBLDOR 300 //10 sec изменение pzr с 50 до 75
 
#define PROHOZHDENIE_SHAHMAT_AUDIO_DLINA 570 //19sec*30 (30inter. = 1sec)
#define PAUSA_PERED_KRASNOY_KNOPKOY 480 //9sec audio + 7 sec lenta
#define PAUSA_PERED_SINEY_KNOPKOY 420 //7 sec audio + 7 sec lenta
#define PAUSA_PERED_POBEDOI 480 //8 sec audio + 8 sec lenta
#define FINAL 510 //18sec audio Volana (минус секунда, чтобы срабатывало синхронно)

ws2812_rec leds[LED_COUNT]; 
unsigned int count=0;
double currentPzv = -1; // текущий процент красной волны (ПКР)
bool led_started = false; // 
bool led_miganie_active = false; // true, если лента находится в режиме мигания
int led_count_qarter;
int led_count_half;
double red_middle, green_middle;
int period_ot_starta_do_podskazki;

int red_r, red_g, red_b, green_r, green_g; // значения составляющих цвета (RGB) красной и зеленой волны. яркость пока всегда принимаем за LED_BRIGHTNESS
 
bool button_slytherin_active = false;//когда true - можем мигать и воспринимать нажатие на кнопку
bool button_griffindor_active = false;
bool button_kogtevran_active = false;
bool button_slytherin_pressed = false;//когда true - делаем все, что должжно происходить после нажатия юзером кнопки
bool button_griffindor_pressed = false;
bool button_kogtevran_pressed = false;
 
bool boi_activated = false;//false пока аудио Дамблдора о прохождении шахмат не закончится
int gercon_time_to_start = 1810; //1750; //38sec (20 sec - время от прохождения шахмат до начала проигрывания аудио boi_start + 18 sec - длительность этого аудио)
bool boi_started = false;//true когда геркон становится активным
bool user_nachal_boi = false, user_nachal_boi_2 = false; //для обнуления count когда юзер начнет бой (впервые поднесет палочку к точке)
int podskazka_dambldora_zakonchena= 870; //29сек (17с отложеное время + 12с время трека)
bool boi_final = false; //true, когда бой окончен и Волан начинает финальную речь, может снова стать false
bool boi_final_while = false; //true, когда бой окончен и Волан начинает финальную речь, НЕ может снова стать false
bool palochka_u_gerkona = true; // true, если юзер держит палку у геркона, иначе - false
bool miganie_do_boia = false;
bool miganie_do_boia2 = false;
bool boi_idet = false;
bool green = false;
bool red = false;
bool blue = false;
 
unsigned char min,sec,hour,day,date,month,year;
 
void timer0_init(void) {
    TCCR0=(1<<CS00)|(1<<CS02); //1024 предделитель
    TIMSK=(1<<TOIE0); // Запускаем таймер
    //asm("sei");
    sei(); 
}
 
void preset(void){
    //сигнальные провода: white 1-7, blue 8; blue 9 - GND
    DDRD &= ~((1<<PIND1)|(1<<PIND2)|(1<<PIND3)|(1<<PIND4));//на вход: геркон палочки и 3 напольные кнопки
    DDRD |= (1<<PIND5)|(1<<PIND6);//на выход: 2 и 3 светодиоды
    PORTD &= ~((1<<PIND5)|(1<<PIND6));
   
    DDRB |= (1<<PINB0)|(1<<PINB1)|(1<<PINB2)|(1<<PINB4)|(1<<PINB5)|(1<<PINB6)|(1<<PINB7);//на выход: лазерное шоу, сигнал начала боя, прерывания боя, 2 оптопары, вентиляторы и 1 светодиод
    PORTB &= ~((1<<PINB0)|(1<<PINB1)|(1<<PINB2)|(1<<PINB4)|(1<<PINB5)|(1<<PINB6)|(1<<PINB7));
   
    DDRC |= (1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5);
    PORTC &= ~((1<<PINC0)|(1<<PINC1)|(1<<PINC2)|(1<<PINC3)|(1<<PINC4)|(1<<PINC5));
}
 
void presetLedValues(int brightness) {
	red_r = (int) ((double) brightness * RED_R / 100.0);
	red_g = (int) ((double) brightness * RED_G / 100.0);
	red_b = (int) ((double) brightness * RED_B / 100.0);
	green_r = (int) ((double) brightness * GREEN_R / 100.0);
	green_g = (int) ((double) brightness * GREEN_G / 100.0);
	led_count_qarter = LED_COUNT/4;
	led_count_half = LED_COUNT/2;
	red_middle = (double) (RED_R + GREEN_R) / 2.0;
	green_middle = (double) (RED_G + GREEN_G) / 2.0;
	period_ot_starta_do_podskazki = LED_DLITELNOST_STOLKNOVENIA + LED_BEFORE_DAMBLDOR;
}
 
void onClear(ws2812_rec* led) {
    led->r = 0;
    led->g = 0;
    led->b = 0;
}
 
void onRedBr(ws2812_rec* led, int brightness) {
    led->r = red_r;
    led->g = red_g;
    led->b = red_b;
}
 
void onGreenBr(ws2812_rec* led, int brightness) {
    led->g = green_g;
    led->r = green_r;
    led->b = 0;
}
 
void onRed(ws2812_rec* led) {
    onRedBr(led, LED_BRIGHTNESS);
}
 
void onGreen(ws2812_rec* led) {
    onGreenBr(led, LED_BRIGHTNESS);
}
 
void clear() {
    for (int i=0; i<LED_COUNT; i++) {
        onClear(&leds[i]);
    }
	led_data_out(&leds[0], LED_COUNT);
}
 
void start() {
    for (int i=0; i<led_count_qarter; i++) {
        onGreen(&leds[i]);
        onGreen(&leds[LED_COUNT-i-1]);
        onRed(&leds[led_count_half-i-1]);
        onRed(&leds[led_count_half+i]);
       
        led_data_out(&leds[0], LED_COUNT);
        _delay_ms(DELAY_START);
    }
}
 
// % of red
void move(double persent) {
    int maxRed = led_count_half * persent * 0.01;
    for (int i=0; i<led_count_half; i++) {
       
        if (maxRed - i < OFFSET_TREMBLE && maxRed - i > 0) {
            double parts = (double)(OFFSET_TREMBLE - (maxRed - i)) / (double) OFFSET_TREMBLE;
            leds[LED_COUNT-i-1].r = leds[i].r =  red_middle* parts;
            leds[LED_COUNT-i-1].g = leds[i].g = green_middle * parts;
            leds[LED_COUNT-i-1].b = leds[i].b = 0;
        } else if (i - maxRed < OFFSET_TREMBLE && i - maxRed > 0) {
            double parts = (double)(OFFSET_TREMBLE - (i - maxRed)) / (double) OFFSET_TREMBLE;
            leds[LED_COUNT-i-1].r = leds[i].r = red_middle * parts;
            leds[LED_COUNT-i-1].g = leds[i].g = green_middle * parts;
            leds[LED_COUNT-i-1].b = leds[i].b = 0;
        } else if (i == maxRed) {
            leds[LED_COUNT-i-1].r = leds[i].r = red_middle;
            leds[LED_COUNT-i-1].g = leds[i].g = green_middle;
            leds[LED_COUNT-i-1].b = leds[i].b = 0;
        } else {
            if (i < maxRed) {
                onGreen(&leds[i]);
                onGreen(&leds[LED_COUNT-i-1]);
            } else {
                onRed(&leds[i]);
                onRed(&leds[LED_COUNT-i-1]);
            }
        }
    }
    led_data_out(&leds[0], LED_COUNT);
}
 
void move_tremble(double persent) {
    move(persent);
    _delay_ms(DELAY_TREMBLE);
    for (int i=0; i<OFFSET_TREMBLE; i++) {
        if (persent - i >= 0) {
            move(persent-i);
            _delay_ms(DELAY_TREMBLE);  
        }
    }
    for (int i=0; i<OFFSET_TREMBLE*2; i++) {
        if (persent - OFFSET_TREMBLE + i <= 100) {
            move(persent-OFFSET_TREMBLE+i);
            _delay_ms(DELAY_TREMBLE);
        }
    }
    for (int i=0; i<OFFSET_TREMBLE; i++) {
        if (persent + OFFSET_TREMBLE - i >= 0) {
            move(persent+OFFSET_TREMBLE-i);
            _delay_ms(DELAY_TREMBLE);
        }
    }
}

void final_check(int count) {
	if (boi_final) {
		if (count > FINAL){
			//лазерное шоу включить
			PORTB |= (1<<PINB0);
			boi_final = false;
		}
	}
}
 
ISR (TIMER0_OVF_vect) {
	
	count++;
	
	bool time_half_sec = count % 15 == 0;
	bool time_one_sec = count % 30 == 0;
   
    //ждем пока проиграет трек Дамблдора прохождения шахмат
    if (!boi_activated && PROHOZHDENIE_SHAHMAT_AUDIO_DLINA < count){
        count = 0;
        boi_activated = true;
        return;
    }
    //здесь мы окажемся, только когда доиграет трек
   
    //ждем, пока выполнится отложенное событие проигрывания трека boi_start + пока этот трек проиграется
    if (!boi_started){
        if(--gercon_time_to_start == 0) {
            boi_started = true;
			led_miganie_active = true;
        } else {
            return;
        }
    }
    //здесь мы окажемся, только когда доиграет трек Волана о старте боя
   
    //геркон активирован, юзер может начать бой, но не факт, что начал
	if (boi_started){
		// юзер не держит палочку возле геркона
		if (PIND & (1<<PIND1)) {
			boi_idet = false;
			// изнаально miganie_do_boia2 false, поэтому сюда будет заходить до того момента, пока юзер не поднесет палку
			if (!miganie_do_boia2) {
				miganie_do_boia = true;
			}
			palochka_u_gerkona = false;
			led_miganie_active = true;
			if (boi_final) {
				final_check(count); // если бой закончен, мы ждем, пока Волан закончит прощальную речь
			} else {
				count--; // все процессы устанавливаются на паузу, count остается неизменным на все время разрыва боя
			}
			return;
		} else {
			boi_idet = true;
			miganie_do_boia = false;
			miganie_do_boia2 = true; // установим в true, чтобы miganie_do_boia навсегда осталось false
			// юзер держит палочку возле геркона
			palochka_u_gerkona = true;
			//убираем моргание светодиода
			led_miganie_active = false;
           
			// как только юзер впервые поднес палочку к геркону - count обнуляется, бой начался
			if (!user_nachal_boi && !user_nachal_boi_2){
				count = 0;
				user_nachal_boi = true;
				user_nachal_boi_2 = true; // вспомогательная переменная, чтобы никогда больше не зайти в этот иф
			}     
		}
       
		// заходим в этот иф, только после того, как юзер впервые поднес палочку к геркону
		// продолжаем заходить, пока не закончится подсказка Дамблдора
		if (user_nachal_boi) {
			// LENTA
			// PZV: 50 (столкновение)
			if (!led_started) {
				currentPzv = 50;
				led_started = true; // флаг для WHILE, отправляем сигнал на основную плату и стартуем волны
			}
			
			if (count > LED_DLITELNOST_STOLKNOVENIA && count <= period_ot_starta_do_podskazki) {
				// LENTA
				// PZV: 50 -> 75 (Волан побеждает)
				if (time_one_sec) {
					currentPzv += 2.5; // (75 - 50) / 10 sec
				}
			}
       
			// ждем, пока проиграется отложенная подсказка Дамблдора
			if (--podskazka_dambldora_zakonchena == 0){
				// зеленая кнопка становится активной
				button_slytherin_active = true;
				user_nachal_boi = false; // выключим флаг, чтобы не заходить в иф
			}
		}
       
		if (button_slytherin_active) {
			if (time_half_sec == 0){
				PORTD ^= (1<<PIND5);//моргаем светодиодом под розеткой (слизерин)
			}
			if (!(PIND & (1<<PIND3))){//2 кнопка под розеткой
				PORTC |= (1<<PINC2);//сигнал на центральную плату СЛИЗЕРИН
				PORTD |= (1<<PIND5);//зеленый светодиод всегда светится
				button_slytherin_active = false;
				count = 0;
				button_slytherin_pressed = true;
				green = true;
			}
		}
   
		if (button_slytherin_pressed){
			//LENTA
			// PZV: 75 -> 55 (Волан все еще побеждает, Гарри отвоевывает позиции)
			if (time_one_sec) {
				currentPzv -= 1.25; //(75 - 55) / 16 [9 + 7 sec]
			}
       
			if (count > PAUSA_PERED_KRASNOY_KNOPKOY){
				button_griffindor_active = true;
				button_slytherin_pressed = false;
				red = true;
			}
		}
   
		if (button_griffindor_active){
			if (time_half_sec){
				PORTB ^= (1<<PINB7);//моргает LED под камерой. Цвет: красный
			}
			if (!(PIND & (1<<PIND2))){//юзер стал на кнопку под камерой
				PORTC |= (1<<PINC3);//сигнал на центральную плату ГРИФФИНДОР
				PORTB |= (1<<PINB7);//засвечиваем навсегда LED под камерой
				button_griffindor_active = false;
				count = 0;
				button_griffindor_pressed = true;
			}
		}
   
		if (button_griffindor_pressed){
			//LENTA
			// PZV: 55 -> 35 (Гарри побеждает)
			if (time_one_sec) {
				currentPzv -= 1.43; //(55 - 35) / 14 [7 + 7 sec]
			}
 
			if (count > PAUSA_PERED_SINEY_KNOPKOY){
				button_kogtevran_active = true;
				button_griffindor_pressed = false;
			}
		}
   
		if (button_kogtevran_active){
			if (time_half_sec){
				PORTD ^= (1<<PIND6);//LED под выключателем. Цвет: синий
			}
			if (!(PIND & (1<<PIND4))){//если юзер станет на кнопку под выключателем
				PORTD |= (1<<PIND6);//засвечиваем навсегда LED под выключателем. Цвет: синий
				PORTC |= (1<<PINC1);//сигнал на центральную плату КОГТЕВРАН
				button_kogtevran_active = false;
				count = 0;
				button_kogtevran_pressed = true;
				blue = true;
			}
		}
   
		if (button_kogtevran_pressed){
			//LENTA
			// PZV: 35 -> 0 (Гарри побеждает)
			if (time_one_sec) {
				currentPzv -= 2.1875; // (35 - 0) / 16 [8 + 8 sec]
			}
 
			if (count > PAUSA_PERED_POBEDOI){
				button_kogtevran_pressed = false;
				count = 0;
				boi_final = true;
				boi_final_while = true;
				//LENTA
				currentPzv = -1; // в WHILE лента выключится
				//выключить светодиоды
				PORTD &= ~((1<<PIND5)|(1<<PIND6));
				PORTB &= ~(1<<PINB7);
				//вентиляторы выключить
				PORTB &= ~(1<<PINB6);//вентиляторы выкл
			}
		}
   
		final_check(count);
	}
}
 
int main(void)
{
	preset();
	presetLedValues(LED_BRIGHTNESS);
	
	_delay_ms(500);
	led_data_init();
	clear();
  
	timer0_init(); 
  
	bool last_led_started = led_started;
	int last_current_pzv = currentPzv;
	int while_count = 0;
	long need_to_send_signal_counter = 0;
	long need_to_send_vozobn_counter = 0;
	int next_led_await = 30; // ждем 3 условных итерации вайла (не забываем, что в вайл заходит только каждую 20ю реальную итерацию, 20 реальных = 1 условная)
	// после чего тушим/засвечиваем диод
	bool miganie_finished = false;
	bool led_miganie_running = true;
	
	while(1) {   
	
		if (boi_final_while) {
			continue;
		}
	
		// искусственный ограничитель частого выполнения кода работы с лентой
		if (while_count++ % 20 != 0) {
			continue;
		} else if (while_count >= 1000000) {
			// в теории, мы можем выйти за пределы максимального значения int, это предотвращение
			while_count = 0;
			continue;
		}
		
		if (!palochka_u_gerkona || miganie_do_boia) {
			// юзер убрал палочку или бой не начался (тогда точка должна индицировать место прикосновения палки)
			need_to_send_signal_counter++;
			need_to_send_vozobn_counter = 0;
		} 
		if (palochka_u_gerkona) {
			need_to_send_signal_counter = 0;
		}
		if (boi_idet) {
			need_to_send_vozobn_counter++;
		}
		if (need_to_send_signal_counter == 1) {
			// юзер убрал палочку, этот иф вызовется 1 раз за прерывание боя
			PORTB &= ~(1<<PINB6);//вентиляторы выкл			
			PORTB |= (1<<PINB2); // сигнал на центральную плату о прерывании боя
			PORTB &= ~(1<<PINB1);//убирает сигнал о начале/возобновлении боя 
			PORTD &= ~((1<<PIND5)|(1<<PIND6));
			PORTB &= ~(1<<PINB7);//выключить светодиоды
		}
		if (need_to_send_vozobn_counter == 1) {
			// юзер поднес палочку, этот иф вызовется 1 раз 
			PORTB |= (1<<PINB6);//вентиляторы вкл
			PORTB |= (1<<PINB1); // сигнал на центральную плату о начале боя
			PORTB &= ~(1<<PINB2);//убирает сигнал о прерывании боя
			
			//если светилась зеленая
			if (green){
				PORTD |= (1<<PIND5);
			}
			//если светилась красная
			if (red){
				PORTB |= (1<<PINB7);
			}
			//если светилась синяя
			if (blue) {
				PORTD |= (1<<PIND6);
			}
		}
	
		// МИГАНИЕ
		if (led_miganie_active) {
			if (!miganie_finished
				clear(); // мигание только началось, потушим всю ленту
				miganie_finished = true; // этот флаг еще понадобится после того, как мигание закончится
			}
			
			if (next_led_await == 30) {
				if (led_miganie_running) {
					// если светодиод сейчас светится
					onRed(&leds[LED_MIDDLE]);
					led_data_out(&leds[0], LED_COUNT); 
				} else {
					clear();
				}
				led_miganie_running = !led_miganie_running;
			} 
			if (--next_led_await == 0) {
				next_led_await = 30;
			}
			
			continue; // если мы в режиме мигания, то работать с лентой не надо
		}
		
		if (miganie_finished) {
			miganie_finished = false;
			clear(); // мигание закончилось, очистим ленту перед работой с ней	
		}
		
		
		
		
		
		
		// СТАРТ
		// красная и зеленая волны сходятся посередине
		// PZV: 50 (ничья)
		if (last_led_started != led_started && led_started) {
			PORTB |= (1<<PINB1); // сигнал на центральную плату о начале боя 
			PORTB &= ~(1<<PINB2);
			start(); // красная и зеленая волны сходятся посередине
			last_led_started = led_started;
		}
		
		// если значение pzv изменилось или нужно поддержать текущий led вывод на ленту (pzv в допустимых пределах)
		if (last_current_pzv != currentPzv || (currentPzv > -1 && currentPzv <= 100)) {
			if (currentPzv == -1) {
				// бой закончился
				clear();
			} else {
				if (palochka_u_gerkona) {
					move_tremble(currentPzv); 
				} else {
					move(currentPzv); 
				}
			}
			last_current_pzv = currentPzv;
		}
	}
}
 
/*
#ifdef LED_DATA2
led_data_out2(&leds[0], &leds[LED_COUNT / 2], LED_COUNT / 2);
#else
led_data_out(&leds[0], LED_COUNT);
#endif
*/
