/*
 * digits.h
 *
 *  Author: CGAleksey
 */


#ifndef DIGITS_H_
#define DIGITS_H_

const double DELAY = 500;

void setNumber(unsigned char num){
	switch(num){
		case 0: PORTB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4) | (1 << PB5) | (1 << PB6);
		break;
		case 1: PORTB = (1 << PB2) | (1 << PB4);
		break;
		case 2: PORTB = (1 << PB0) | (1 << PB2) | (1 << PB3) | (1 << PB6) | (1 << PB5);
		break;
		case 3: PORTB = (1 << PB0) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5);
		break;
		case 4: PORTB = (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);
		break;
		case 5: PORTB = (1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB4) | (1 << PB5);
		break;
		case 6: PORTB = (1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB4) | (1 << PB5) | (1 << PB6);
		break;
		case 7: PORTB = (1 << PB0) | (1 << PB2) | (1 << PB4);
		break;
		case 8: PORTB = (1 << PB0) | (1 << PB1)  | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5) | (1 << PB6);
		break;
		case 9: PORTB = (1 << PB0) | (1 << PB1)  | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5);
		break;
	}
}

void selectDigit(unsigned char digit_pos) {
	// Включаем нужный разряд (ставим 0 на соответствующий бит)
	switch(digit_pos) {
		case 3: PORTA &= ~(1 << PA0); break;  // единицы
		case 2: PORTA &= ~(1 << PA1); break;  // десятки
		case 1: PORTA &= ~(1 << PA2); break;  // сотни
		case 0: PORTA &= ~(1 << PA3); break;  // тысячи
	}
}

void printNum(unsigned int num) {
	unsigned char digits[4];

	// Разбиваем число на цифры
	digits[0] = num % 10;        // единицы
	digits[1] = (num / 10) % 10; // десятки
	digits[2] = (num / 100) % 10;// сотни
	digits[3] = (num / 1000) % 10;// тысячи

	// Мультиплексирование: быстро перебираем все 4 разряда
	for (unsigned char i = 0; i < 4; i++) {
		// Небольшая задержка для "гашения" остаточного свечения
		_delay_us(DELAY);

		// Выключаем ВСЕ разряды (PA0-PA3 = 1)
		PORTA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3);

		PORTB = 0x00;
		// Небольшая задержка для "гашения" остаточного свечения
		_delay_us(DELAY);

		selectDigit(i);           // выбираем разряд
		setNumber(digits[i]);     // показываем цифру
	}
}



#endif /* DIGITS_H_ */
