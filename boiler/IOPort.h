#ifndef _IOPORT_																			//Защита от повторного включения файла
#define _IOPORT_

#include <avr/iom128.h>																		//Стандартная библиотека ввода/вывода
#include <util/delay.h>																		//Стандартная библиотека временных задержек

#ifndef F_CPU
# warning "F_CPU not defined. Frequency by default 14,7456 MHz"
#define F_CPU 14745600UL																	//Стандартная константа задающая частоту микроконтроллера (необходима для корректной работы функций задержек)
#endif

#define LOW		0 																			//Низкое состояние линии
#define HI		1 																			//Высокое состояние линии
#define SW		2 																			//Переключить состояние линии

#include "LED.h"																			//Библиотека работы с LED дисплеем

//==================== Функция инициализации дискретных входов/выходов =====================
void IO_Init(void)
{
	DDRC&=0x80;																				//Настройка линий дискретных входов Din0-Din6
#if _DIN-7																					//Для моделей имеющих больше 7 дискретных входов	
	DDRC&=0x80;																				//Настройка линии дискретного входа Din7
#if _DIN-8																					//Для моделей имеющих больше 7 дискретных входов		
	DDRA&=0x0F;																				//Настройка линий дискретных входов Din8-Din11	
#endif	
#endif	

	DDRB|=0x30;																				//Настройка линий дискретных выходов Dout0-Dout1
#if _DOUT-2																					//Для моделей имеющих больше 2 дискретных вsходов		
	DDRB|=0xC0;																				//Настройка линий дискретных выходов Dout2-Dout3
	DDRD|=0x30;																				//Настройка линий дискретных выходов Dout4-Dout5	
#if _DOUT-6																					//Для моделей имеющих больше 6 дискретных вsходов		
	DDRD|=0xC0;																				//Настройка линий дискретных выходов Dout6-Dout7
#endif
#endif
}

//====================== Функция чтения состояния дискретных входов ========================
uint8_t Input_Read(uint8_t Line)
{
	uint8_t temp=0;																			//Вспомогательная переменная
	
	switch (Line)																			//Определение номера входа
	{
	case 0: temp=((PINC & (1<<PC0))>>PC0) ^ 0x01; break;
	case 1: temp=((PINC & (1<<PC1))>>PC1) ^ 0x01; break;
	case 2: temp=((PINC & (1<<PC2))>>PC2) ^ 0x01; break;
	case 3: temp=((PINC & (1<<PC3))>>PC3) ^ 0x01; break;
	case 4: temp=((PINC & (1<<PC4))>>PC4) ^ 0x01; break;
	case 5: temp=((PINC & (1<<PC5))>>PC5) ^ 0x01; break;
	case 6: temp=((PINC & (1<<PC6))>>PC6) ^ 0x01; break;

#if _DIN-7																					//Для моделей имеющих больше 7 дискретных входов
	case 7: temp=((PINC & (1<<PC7))>>PC7) ^ 0x01; break;
#if _DIN-8																					//Для моделей имеющих больше 8 дискретных входов
	case 8: temp=((PINA & (1<<PA4))>>PA4) ^ 0x01; break;
	case 9: temp=((PINA & (1<<PA5))>>PA5) ^ 0x01; break;
	case 10: temp=((PINA & (1<<PA6))>>PA6) ^ 0x01; break;
	case 11: temp=((PINA & (1<<PA7))>>PA7) ^ 0x01; break;
#endif
#endif	
	}
	return temp;																			//Возврат результата
}

//==================== Функция чтения состояния всех дискретных входов =====================
int Input_Read_All(void)
{
#if !(_DIN-7)																				//Для моделей имеющих 7 дискретных входов	
	return ~PINC & 0x007F;
#elif !(_DIN-8)																				//Для моделей имеющих 8 дискретных входов
	return ~PINC & 0x00FF;
#else																						//Для моделей имеющих больше 8 дискретных входов
	return ((~PINA<<4) & 0x0F00)+(~PINC & 0x00FF);											
#endif
}

//==== Функция вывода информации о текущем состоянии всех дисретных входов на индикатор =====
int Input_Led(void)
{

	uint8_t temp;																			//Вспомогательные переменные
	int data;

	data=Input_Read_All();																	//Чтение состояния входов

	#ifdef TIMER2
	LED_Cycle_Shift(0);																		//Остновить циклическую прокрутку сообщений
	#endif
	LED_SetPos(0,0);																		//Переход на первую строку
	
#if !(_DIN-7)																				//Для моделей имеющих 7 дискретных входов	
	LED_Write_String("0123456         ");													//Вывод подписей к дискретным входам
#elif !(_DIN-8)																				//Для моделей имеющих 8 дискретных входов
	LED_Write_String("01234567        ");													//Вывод подписей к дискретным входам
#else																						//Для моделей имеющих больше 8 дискретных входов
	LED_Write_String("0123456789AB    ");													//Вывод подписей к дискретным входам
#endif
	
	LED_SetPos(1,0);																		//Переход на вторую строку
	for (temp=0; temp<16; temp++)															//Вывод состояния входов
	{
		if (data & (1<<temp)) _LED_Write_Byte(0xFF);										//Если логическая "1", печатать закрашенный прямоугольник
		else _LED_Write_Byte(0x20);															//иначе, символ пробела
	}
	
	return data;																			//Возврат результата
}

//==================== Функция установки состояния дискретных выходов ======================
void Output_Write(uint8_t Line, uint8_t Status)
{
	switch(Status)																			//Определение статуса изменения состояния выхода
	{
	case 0:																					//Если Выход переводится в низкое состояние
		switch(Line)																		//Определение номера дискретного выхода
		{
		case 0:	PORTB&=~(1<<PB4); break;
		case 1:	PORTB&=~(1<<PB5); break;
#if _DOUT-2																					//Для моделей имеющих больше 2 дискретных вsходов				
		case 2:	PORTB&=~(1<<PB6); break;
		case 3:	PORTB&=~(1<<PB7); break;
		case 4:	PORTD&=~(1<<PD4); break;
		case 5:	PORTD&=~(1<<PD5); break;
#if _DOUT-6																					//Для моделей имеющих больше 2 дискретных вsходов				
		case 6:	PORTD&=~(1<<PD6); break;
		case 7:	PORTD&=~(1<<PD7); break;		
#endif
#endif		
		}
		break;	
	case 1:																					//Если Выход переводится в высокое состояние
		switch(Line)																		//Определение номера дискретного выхода
		{
		case 0:	PORTB|=(1<<PB4); break;
		case 1:	PORTB|=(1<<PB5); break;
#if _DOUT-2																					//Для моделей имеющих больше 2 дискретных вsходов						
		case 2:	PORTB|=(1<<PB6); break;
		case 3:	PORTB|=(1<<PB7); break;
		case 4:	PORTD|=(1<<PD4); break;
		case 5:	PORTD|=(1<<PD5); break;
#if _DOUT-6																					//Для моделей имеющих больше 2 дискретных вsходов				
		case 6:	PORTD|=(1<<PD6); break;
		case 7:	PORTD|=(1<<PD7); break;
#endif
#endif
		}
		break;		
	case 2:																					//Если Выход меняет свое состояние на противоположное
		switch(Line)																		//Определение номера дискретного выхода
		{
		case 0:	PORTB^=(1<<PB4); break;
		case 1:	PORTB^=(1<<PB5); break;
#if _DOUT-2																					//Для моделей имеющих больше 2 дискретных вsходов				
		case 2:	PORTB^=(1<<PB6); break;
		case 3:	PORTB^=(1<<PB7); break;
		case 4:	PORTD^=(1<<PD4); break;
		case 5:	PORTD^=(1<<PD5); break;
#if _DOUT-6																					//Для моделей имеющих больше 2 дискретных вsходов				
		case 6:	PORTD^=(1<<PD6); break;
		case 7:	PORTD^=(1<<PD7); break;
#endif
#endif
		}
		break;		
	}
}

//================== Функция установки состояния всех дискретных выходов ===================
void Output_Write_All(uint8_t data)
{
	PORTB&=(data<<4) | 0xCF; PORTB|=(data<<4) & 0x30;
#if _DOUT-2																					//Для моделей имеющих больше 2 дискретных вsходов				
	PORTB&=(data<<4) | 0x3F; PORTB|=(data<<4) & 0xC0;
	PORTD&=data | 0xCF; PORTD|=data & 0x30;
#if _DOUT-6																					//Для моделей имеющих больше 2 дискретных вsходов				
	PORTD&=data | 0x3F; PORTD|=data & 0xC0;
#endif
#endif
}

//====================== Функция чтения состояния дискретных выходов =======================
uint8_t Output_Read(uint8_t Line)
{
	uint8_t temp=0;																			//Вспомогательная переменная
	
	switch (Line)																			//Определение номера входа
	{
		case 0: temp=((PORTB & (1<<PB4))>>PB4); break;
		case 1: temp=((PORTB & (1<<PB5))>>PB5); break;
#if _DOUT-2																					//Для моделей имеющих больше 2 дискретных вsходов				
		case 2: temp=((PORTB & (1<<PB6))>>PB6); break;
		case 3: temp=((PORTB & (1<<PB7))>>PB7); break;
		case 4: temp=((PORTD & (1<<PD4))>>PD4); break;
		case 5: temp=((PORTD & (1<<PD5))>>PD5); break;
#if _DOUT-6																					//Для моделей имеющих больше 2 дискретных вsходов				
		case 6: temp=((PORTD & (1<<PD6))>>PD6); break;
		case 7: temp=((PORTD & (1<<PD7))>>PD7); break;
#endif
#endif
	}
	return temp;																			//Возврат результата
}

//==================== Функция чтения состояния всех дискретных выходов ====================
uint8_t Output_Read_All(void)
{
	char temp;
	temp=(PORTB>>4) & 0x03;
#if _DOUT-2																					//Для моделей имеющих больше 2 дискретных вsходов				
	temp|=(PORTB>>4) & 0x0C;
	temp|=(PORTD & 0x30);
#if _DOUT-6																					//Для моделей имеющих больше 2 дискретных вsходов					
	temp|=(PORTD & 0xC0);
#endif
#endif
	return temp;
}

#endif