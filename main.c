#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

#include "digits.h"

// Глобальные переменные
volatile unsigned long millis = 0;  // счётчик миллисекунд
volatile unsigned int display_number = 0;       // текущее число для отображения

// Состояние транзистора, активирующего реле коммутирующее 220в
bool transistorReley = false;

#define DEVICE_TEST_CONTACT PA5 // контакт индикации работы устройства
#define TRANSISTOR_BASE_PIN PA6

// Функция проверки кнопки (возвращает 1 если нажата)
bool isLampWorking() {
	// Сигнал есть — кнопка замкнута
	// Сигнала нет — кнопка разомкнута
	return (PINA & (1 << DEVICE_TEST_CONTACT)) == 0;
}

void transistorOnOff(){
	if (transistorReley){// 0 - разомкнута . 1 - замкнута
		PORTA |= (1 << TRANSISTOR_BASE_PIN); // зажигаем транзистор
	}else{
		PORTA &= ~(1 << TRANSISTOR_BASE_PIN); // гасим транзистор
	}
}

// Прерывание по переполнению таймера 0
ISR(TIMER0_OVF_vect) {
	static unsigned char counter = 0;
	counter++;
	
	bool deviceOn = transistorReley
	? counter >= 61 * 3.0 && isLampWorking()// не работает 3с
	: counter >= 61 * 0.5 && isLampWorking();// работает 0.5c
	
	// Примерно каждую секунду (61 * 16.384 мс ≈ 1000 мс)
	if (deviceOn) {
		// Lamp On/Off & calculate actions
		counter = 0;
		
		if (transistorReley){
			++display_number;
		}
		transistorOnOff();
		transistorReley = !transistorReley;
		
		if (display_number > 9999) {
			display_number = 0;
		}
	}
}

void timer0_init(void) {
	// Настройка таймера 0:
	// Частота МК = 1 МГц
	// Предделитель = 64 → частота таймера = 15625 Гц
	// Переполнение каждые 256 тиков → 256 / 15625 = 0.016384 сек = 16.384 мс
	// Нам нужно 1 мс → будем использовать счётчик
	
	TCCR0 = (1 << CS01) | (1 << CS00);  // предделитель 64
	TCNT0 = 0;                          // счётчик начинаем с 0
	TIMSK |= (1 << TOIE0);              // разрешаем прерывание по переполнению
}

void initPorts(){
	// Разряды индикатора (PA0-PA3) как выходы
	DDRA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3);
	
	
	// вход (~кнопка)
	DDRA &= ~(1 << DEVICE_TEST_CONTACT);  // вход
	PORTA |= (1 << DEVICE_TEST_CONTACT);  // включаем подтяжку!
	
	
	DDRA |= (1 << TRANSISTOR_BASE_PIN);    // назначаем как выход
	PORTA &= ~(1 << TRANSISTOR_BASE_PIN);  // начинаем с 0в
	
	// Сегменты (PB0-PB7) как выходы индикатора
	DDRB = 0xFF;
	
	// Начальное состояние
	PORTA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3);
	PORTB = 0x00;
}

int main(void) {
	 
	 initPorts();
	
	// Запускаем таймер
	timer0_init();
	
	// Глобально разрешаем прерывания
	sei();
	
	unsigned long last_second = 0;
	
	while(1) {
		printNum(display_number);
	}
}