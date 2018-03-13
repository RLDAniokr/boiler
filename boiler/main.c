/*
	Прошивка котла: НИЛ АП
	Дата создания: 13.03.2018
	Автор: Антипин С.О. (НИОКР)
*/ 

// Частота микроконтроллера
#define F_CPU 14745600UL

// Параметры СОМ-порта
#define COM_RX_BUFFER_SIZE 32
#define COM_TX_BUFFER_SIZE 32
#define COM1_PROTOCOL DCON

// Параметры меню
#define MAX_ELEMENTS_MENU 38
#define MAX_SUB_MENUS 17
#define MAX_HISTORY_ELEMENTS 16
#define MAX_HISTORY_NAMES 16

// Стандартная библиотека ввода/вывода
#include <avr/io.h>
// Стандартная библиотека символьных констант
#include <stdio.h>
// Стандартная библиотека для хранения данных во FLASH памяти
#include <avr/pgmspace.h>																											
// Стандартная библиотека для хранения данных в EEPROM памяти
#include <avr/eeprom.h>																												
// Стандартная библиотека для математических операций
#include <math.h>																													
// Стандартная библиотека для манипуляций с памятью
#include <stdlib.h>


// Библиотеки плат серии MC

// #include "MC12D4R4O.h"
// #include "MC12D6R.h"
#include "MC8U6R.h"

// Пользовательские библиотеки

// Библиотека для работы с дисплеем
#include "LED.h"
// Библиотека для создания меню
#include "Menu.h"
// Библиотека для работы с кнопками
#include "Key.h"
// Библиотека для работы с СОМ-портами
// NOTE: commented _Frame_pause (CAUSE: Warns on compile)
#include "COM.h"
// Библиотека для работы с часами реального времени
#include "RTC.h"
// Числовые и символьные переменные
#include "params.h"

// Выносные inline'ы
// Чтение, запись и конвертация данных
#include "memory_ops.h"
// Параметры и манипуляции в меню
#include "menu_boiler.h"
// Работа с часами реального времени
#include "rtc_boiler.h"
// Расчет работы регулятора
#include "pid_boiler.h"
// Работа с датчиками
#include "snc_boiler.h"
// Работа с тактовыми кнопками
#include "buttons_boiler.h"
// Работа с дисплеем
#include "led_boiler.h"
// Обработка прервыаний
#include "interrupts_boiler.h"

//============================================================//

int main(void)
{
	_delay_ms(1000);
	
	//Инициализация экрана
	LED_Init();
	//Инициализация RTC
	RTC_Init();
	//Инициализация СОМ
	COM_Init(COM_PORT1, 9600);

	//Переключение портов на вывод (клапаны)
	DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PD7);
	//Преключение порта на вывод (насос)
	DDRD |= (1 << PD4);
		
	//Вызов функции создания элементов меню
	create_menu();
	//Вызов функции запуска датчиков
	start_sencors();
	//Вызов функции чтения данных из EEPROM в RAM
	mems_to_RAM();
	//Инициализация таймера
	Timer0_Init();
	//Переназначение счетчика таймера для первого чтения датчиков
	time_10_dat=per_dat_RAM*100-5;
	//Функция чтения датчиков
	read_sencors();
	//Конвертация чиловых данных из EEPROM в символьные для отображения
	convert_to_show();

    while (1) 
    {
		// Отображение основного экрана
		main_screen();
		// Чтение данных с RTC
		get_time();
		// Проверка времени
		check_time();
		// Обработчик нажатия на кнопки
		check_buttons();
    }
}

