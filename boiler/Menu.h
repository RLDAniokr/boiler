#ifndef _MENU_																														//Защита от повторного включения файла
#define _MENU_ 1

#ifndef F_CPU
# warning "F_CPU not defined. Frequency by default 14,7456MHz"
#define F_CPU 14745600UL																											//Стандартная константа, задающая частоту микроконтроллера (необходима для корректной работы функций задержек)
#endif

#ifndef MAX_ELEMENTS_MENU
#warning "MAX_ELEMENTS not defined. Elements count by default 50"
#define MAX_ELEMENTS_MENU 50																										//Константа, задающая количество элементов в меню
#endif

#ifndef MAX_SUB_MENUS
#warning "MAX_SUB_MENUS not defined. Elements count by default 10"
#define MAX_SUB_MENUS 10																											//Константа, задающая количество подменю
#endif

#ifndef MAX_HISTORY_ELEMENTS
#warning "MAX_HISTORY_ELEMENTS not defined"
#define MAX_HISTORY_ELEMENTS 16																										//Константа, задающая длину списка внесенных изменений
#endif

#define _MAX_PARAM_LENGHT 8																											//Константа, задающая максимальную длину текстового параметра

#define NONE 0																														//Константа "Нет данных"

#define OUT 0																														//Константа "Выход из меню"
#define IN 1																														//Константа "Вход в меню"
#define EXIT 65535																													//Константа "Выход их меню"

#include <avr/iom128.h>																												//Стандартная библиотека ввода/вывода
#include <util/delay.h>																												//Стандартная библиотека задержки
#include <stdio.h>
#include "Key.h"																													//Библиотека для работы с кнопками
																								
//-------------------------------------- Создание перечисления типа элементов меню -------------------------------------------------//
typedef enum																														//Создание списка типов элементов меню
{
	EMPTY_ELEMENT = 0,																												//Пустой элемент
	PARAM_ELEMENT,																													//Элемент с числовым параметром
	SWITCH_ELEMENT,																													//Элемент-переключатель (вкл/выкл)
	DEFAULT_ELEMENT,																												//Элемент, возвращающий настройки по умолчанию
	HISTORY_ELEMENT,																												//Элемент, отображающий историю изменений
	WORD_ELEMENT,																													//Элемент с текстовым параметром
	BLANK_ELEMENT,																													//Тип элемента, возвращаемый только номер элемента.
	LIST_ELEMENT,																													//
	PASSWORD_ELEMENT,
} menu_Types;

//------------------------------------------- Создание перечисления флагов ---------------------------------------------------------//
enum _Lib_Flags																														//Создание списка флагов
{
	lib_Flag_Error = (1),																											//Флаг ошибки
	lib_Flag_Set_Changes = (1 << 1),																								//Флаг внесения изменений на экране
	lib_Flag_Menu_Enable = (1 << 2),																								//Флаг активации меню
	lib_Flag_Changing_Param = (1 << 3),																								//Флаг изменения параметров
	lib_Flag_Save_Change = (1 << 4),																								//Флаг сохранения изменений
	lib_Flag_FIFO_End = (1 << 5),																									//Флаг заполнения списка внесенных изменений
	lib_Flag_Menu_Out = (1 << 6),
} _my_Lib_Flags = 0;																												//Создание переменной в области Флагов
					
//----------------------------------------- Объявление глобальных переменных -------------------------------------------------------//
uint8_t _temp_Array[8];																												//Буфер для отображения текущего параметра
uint8_t _temp_Switch = 0;																											//Переменная-буфер для отображения текущего параметра
uint16_t _selected_Element = 0;																										//Номер текущего элемента меню
uint16_t _number_In_Submenu = 0;

uint8_t _history_Buf_Param[MAX_HISTORY_ELEMENTS][_MAX_PARAM_LENGHT];																//Список параметров элементов истории внесенных изменений
uint16_t _history_Buf_Number[MAX_HISTORY_ELEMENTS];

uint8_t _line2 = 0;																													//Вспомогательная переменная для отображения элементов истории
uint8_t _line = 0;																													//Вспомогательная переменная для сохранения элементов истории

int8_t _temp_List = 0;
//---------------------------------------- Создание структуры элементов меню -------------------------------------------------------//
typedef struct																											
{
	uint8_t element_Type;																											//Тип элемента
	uint8_t child_Submenu;																											//Номер дочернего подменю
	__flash const char *text;																										//Указатель на начало переданного названия элемента
	uint8_t *data;																													//Указатель на начало массива данных
	uint8_t array_Size;
	uint8_t number_In_Submenu;
} Menu_Elements;																													//Массив элементов подменю, содержащий в себе все перечисленные выше элементы структуры 

Menu_Elements menu_Elements[MAX_ELEMENTS_MENU+1];
//------------------------------------------- Создание структуры подменю -----------------------------------------------------------//
struct submenu
{
	uint8_t elements_Count_In_Submenu;																								//Количество элементов в текущем подменю
	uint8_t parent_Element_Number;																									//Номер родительского элемента
}submenu[MAX_SUB_MENUS+1];																											//Массив подменю, содержащий в себе все перечисленные выше элементы структуры 

//==================================== Функция создания нового элемента меню =======================================================//
//Функция формирует массив структуры menu_Elements. Каждый элемент массива содержит в себе все параметры структуры.
//Функция принимает аргументы:
//           number_Element - не используется в вычислениях, необходим для корректного указания пользователем связей, при создании большого количества элементов меню.
//             element_Type - тип элемента.
//            child_Submenu - номер дочернего подменю.
//                    *text - указатель на начало переданного названия элемента.
//                    *data - указатель на начало массива данных.
//				 array_Size - количество символов в параметре.
void Add_New_Element(const uint8_t number_Element,  menu_Types element_Type,  uint8_t child_Submenu, __flash const char *text, uint8_t *data, uint8_t array_Size)	
{
	static uint16_t count_Element = 0;																								//Счетчик количества элементов меню
	if (count_Element > MAX_ELEMENTS_MENU) return;																					//Проверка на переполнение
	menu_Elements[count_Element].element_Type = element_Type;
	menu_Elements[count_Element].text = text;
	menu_Elements[count_Element].data = data;
	
	menu_Elements[count_Element].child_Submenu = child_Submenu;																		//Сохранение параметров элемента меню
	if (child_Submenu != NONE) 	submenu[child_Submenu].parent_Element_Number = count_Element;
	
	if (array_Size != NONE) menu_Elements[count_Element].array_Size = array_Size;
	else if (element_Type == SWITCH_ELEMENT) menu_Elements[count_Element].array_Size = 1;
	else menu_Elements[count_Element].array_Size = NONE;
	
	menu_Elements[count_Element].number_In_Submenu = _number_In_Submenu;
	_number_In_Submenu++;
	count_Element++;
}

//=========================================== Функция создания нового подменю =======================================================//
//Функция формирует массив структуры submenu.
//Функция принимает аргументы:
//              number_Menu - не используется в вычислениях, необходим для корректного указания пользователем связей, при создании большого количества подменю.
//elements_Count_In_Submenu - количество элементов в подменю.
void Add_New_Menu(const uint8_t number_Menu,uint8_t elements_Count_In_Submenu)
{
	static uint16_t count_Submenu = 0;																								//Счетчик количества подменю
	_number_In_Submenu = 0;
	
	if (count_Submenu > MAX_SUB_MENUS) return;																						//Проверка на переполнение
	submenu[count_Submenu].elements_Count_In_Submenu = elements_Count_In_Submenu;													//Сохранение параметров подменю

	count_Submenu++;
}

//===================================== Функция вычисления порядкового номера элемента =============================================//
//Функция принимает аргументы:
//element_Count - порядковый номер элемента в текущем подменю.
//submenu_Count - порядковый номер подменю.
void _Selected_Element_Calculate(uint8_t element_Count, uint8_t submenu_Count)
{
	_selected_Element = 0;																											//Сброс переменной порядкового номера выбранного элемента
	if (submenu_Count != 0)																											//Проверка на уместное использование
	{
		for (int i = 0; i <submenu_Count; i++) _selected_Element = _selected_Element + submenu[i].elements_Count_In_Submenu;		//Вычисление порядкового номера
	}
	_selected_Element += element_Count;
}

//===================================== Функция вычисления массива параметров =====================================================//
//Функция копирует в буфер temp_Array или temp_Switch параметры текущего элемента меню при навигации по меню.
//Функция принимает аргументы:
//            type - тип текущего элемента меню.
//selected_Element - порядковый номер текущего элемента меню.
//Функция возвращает:
//			 count - количество элементов в массиве параметра. 
void _Temp_Array_First_Out(uint8_t type, uint8_t count)
{
	uint8_t* array_Byte = menu_Elements[_selected_Element].data;																	//Присвоение вспомогательному указателю значения указателя данных из структуры
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	if (type == SWITCH_ELEMENT)
	{
		 _temp_Switch = *array_Byte;
	}
	else if (type == PARAM_ELEMENT || type == WORD_ELEMENT || type == PASSWORD_ELEMENT)
	{
		for (int i = 0; i < count; i++)
		{
			_temp_Array[i] = *array_Byte;
			array_Byte++;
		}
	}
	else if (type == LIST_ELEMENT)
	{
		for (int i = 0; ;i++)
		{
			if (*(array_Byte + i*count) == 255) break;
			if (*(array_Byte + i*count) == 0)
			{
				_temp_List = i;
				for (int j = 0; j < count; j++) _temp_Array[j] = *(array_Byte + i*count + j);
				break;
			}
		}
	}
}


//====================================== Функция вывода текста на индикатор ========================================================//
//Функция принимает агрументы:
//element_Count - порядковый номер элемента в текущем подменю.
//submenu_Count - порядковый номер подменю.
//   cursor_Pos - позиция курсора на индикаторе (во время изменения параметров).
void _Display_Out(uint8_t cursor_Pos)
{
	__flash const char *array_Byte = 0;																								//Указатель для чтения текста элементов меню из области памяти FLASH
	uint16_t number = _history_Buf_Number[_line2];																			
	uint8_t count = menu_Elements[_selected_Element].array_Size;
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	uint8_t type = menu_Elements[_selected_Element].element_Type;
	array_Byte = menu_Elements[_selected_Element].text;																				//Присвоение указателю значение адреса текста текущего элемента меню
	
	switch (type)
	{
//--------------------- Вывод на индикатор, если просматривается история изменений -------------------------------------------------//
		case HISTORY_ELEMENT:																										//Если тип элемента - элемент содержащий список внесеных изменений
			if (_line == 0)
			{
				LED_SetPos(0,0);
				array_Byte = menu_Elements[_selected_Element].text;																	//Вывод сообщения, сохраненного в области памяти FLASH
				for (int i = 0; i < 16; i++)
				{
					if (*array_Byte == 0) break;
					_LED_Write_Char(*array_Byte);
					array_Byte++;
				}
			}
			else
			{
				uint8_t count_His = menu_Elements[number].array_Size;																//Количество элементов в массиве параметра элемента меню
				if (menu_Elements[number].element_Type == LIST_ELEMENT) count_His--;
				if (count_His > _MAX_PARAM_LENGHT) count_His = _MAX_PARAM_LENGHT;
				
				char history_Number[2];
				sprintf(history_Number, "%d", _line-_line2);
				LED_SetPos(1, 14);
				LED_Write_String(history_Number);
				
				LED_SetPos(0,0);
				array_Byte = menu_Elements[number].text;
				for (int i = 0; i < 16; i++)																						//Формирование на индикаторе имени выбранного элемента истории
				{
					if (*array_Byte == 0) break;
					_LED_Write_Char(*array_Byte);
					array_Byte++;
				}
				
				LED_SetPos(1,0);
				if (menu_Elements[number].element_Type == SWITCH_ELEMENT)
				{
					if (_history_Buf_Param[_line2][0] != 0) LED_Write_String_P(PSTR("Вкл"));
					else LED_Write_String_P(PSTR("Выкл"));
				}
				else 
				{
					for (int i = 0; i < count_His; i++) _LED_Write_Char(_history_Buf_Param[_line2][i]);
				}
			}
		break;
		
//---------------------------------------- Вывод на индикатор стандартной информации -----------------------------------------------//
		default:																													//Вывод информации на индикатор по умолчанию
			LED_SetPos(0,0);
			for (int i = 0; i < 32; i++)
			{
				if (*array_Byte == 0) 		 																						//Проверка на конец строки
				{
					break;
				}
				else if (*array_Byte == '/')																						//Проверка на перенос строки
				{
					array_Byte++;
					if (*array_Byte == 'n')
					{
						array_Byte++;
						LED_SetPos(1,0);
					}
					else array_Byte--;
				}
				_LED_Write_Char(*array_Byte);																						//Вывод символа на экран
				array_Byte++;
			}
			
 			if (type != EMPTY_ELEMENT && (_my_Lib_Flags & lib_Flag_Save_Change) == 0)												//Если тип элемента - не пустой элемент и если не было произведено сохранения параметра
 			{
 				//Формирование буфера для вывода параметра на индикатор и вычисление длины массива параметра
 				if ((_my_Lib_Flags & lib_Flag_Changing_Param) == 0) _Temp_Array_First_Out(type, count);
 			}
			
 			if (type == SWITCH_ELEMENT)
 			{
				LED_SetPos(1,0);
				if (_temp_Switch == 1) LED_Write_String_P(PSTR("Вкл"));																//Вывод сообщения относительно записанного параметра
				else if (_temp_Switch == 0) LED_Write_String_P(PSTR("Выкл"));
 			}
			else if (type == PARAM_ELEMENT || type == WORD_ELEMENT || type == PASSWORD_ELEMENT)
			{
				LED_SetPos(1,0);
				for (int i = 0; i < count; i++) _LED_Write_Char(_temp_Array[i]);													//Вывод данных на индикатор посимвольно
			}
			else if (type == LIST_ELEMENT)
			{
				LED_SetPos(1,0);
				for (int i = 0; i < count-1; i++) _LED_Write_Char(_temp_Array[i+1]);
			}

			if (_my_Lib_Flags & lib_Flag_Changing_Param)																			//Если происходит изменение параметра
			{
				switch(type)																										//Установка курсора
				{
					case PARAM_ELEMENT: LED_SetPos(1, cursor_Pos); break;
					case WORD_ELEMENT: LED_SetPos(1, cursor_Pos); break;
					case PASSWORD_ELEMENT: LED_SetPos(1, cursor_Pos); break;
				}
			}
	}
}

//================================= Функция формирования списка внесенных изменений ================================================//
//Функция вызывается после подтверждения изменений и после проверки нового параметра на достоверность, но до перезаписи значения параметра.
//В качестве данных функция использует указатель на текущее значения параметров menu_Elements[selectedElement].data и имя элемента menu_Elements[selectedElement].text
void _History_Save(void)
{
	uint8_t *array_Byte_Param = 0;																									//Указатель для чтения параметра
	uint8_t count = menu_Elements[_selected_Element].array_Size;																	//Стандартное количество элементов в массиве параметра
	uint8_t type = menu_Elements[_selected_Element].element_Type;
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	array_Byte_Param = menu_Elements[_selected_Element].data;																		//Присвоение указателям значения элементов структуры

	if ((_my_Lib_Flags & lib_Flag_FIFO_End) == 0)																					//Если массив истории не заполнен
	{
		_history_Buf_Number[_line] = _selected_Element;
		for (int i = 0; i < count; i++)
		{
			if (type != LIST_ELEMENT)
			{
				 _history_Buf_Param[_line][i] = *array_Byte_Param;
				 array_Byte_Param++;
			}
			else
			{
				_history_Buf_Param[_line][i] = _temp_Array[i+1];
				break;
			}
		}
		
		_line++;
		_line2 = _line-1;
	}
	else
	{
		for (int i = 1; i < MAX_HISTORY_ELEMENTS; i++)
		{
			uint16_t temp = _history_Buf_Number[i];
			_history_Buf_Number[i-1] = temp;
			for (int j = 0; j < menu_Elements[temp].array_Size; j++) _history_Buf_Param[i-1][j] = _history_Buf_Param[i][j];
		}
		
		_history_Buf_Number[_line-1] = _selected_Element;
		for (int i = 0; i < count; i++)
		{
			if (type != LIST_ELEMENT)
			{
				_history_Buf_Param[_line][i] = *array_Byte_Param;
				array_Byte_Param++;
			}
			else
			{
				_history_Buf_Param[_line][i] = _temp_Array[i+1];
				break;
			}
		}
		_line2 = _line-1;
	}
	
	if (_line >= MAX_HISTORY_ELEMENTS) _my_Lib_Flags |= lib_Flag_FIFO_End;															//Установка флага, если массив истории заполнен
}


//================================================ Функция изменения параметров ===================================================//
//Функция принимает аргументы:
//             key - номер нажатой кнопки. 
//      cursor_Pos - позицию курсора.
//selected_Element - номер текущего элемента меню.
//Функция возвращает:
//cursor_Pos - новое значения позиции курсора (требуется функции _Display_Out()).
uint8_t _New_Param(uint8_t key, uint8_t cursor_Pos, uint8_t selected_Element)
{
	uint8_t max_lenght = menu_Elements[_selected_Element].array_Size;																//Вспомогательная переменная длины массива параметра
	if (max_lenght > _MAX_PARAM_LENGHT) max_lenght = _MAX_PARAM_LENGHT;
	
	uint8_t type = menu_Elements[_selected_Element].element_Type;
	uint8_t point = 0;																												//Вспомогательная переменная наличия точки
	
//---------------------------------------- Действия при нажатии кнопки 0 -----------------------------------------------------------//
	if (key == 0)																													//Если нажата кнопка ВВОД
	{
		switch (type)
		{
			default:																												//Любой другой элемент
				cursor_Pos++;
				if (cursor_Pos >= max_lenght) cursor_Pos = 0;																		//Зацикливание смещение курсора
		}
	}
	else
	{
		for (int i = 0; i < max_lenght; i++) 																						//Проверка отображаемого параметра на содержание точки
		{
			if (_temp_Array[i] == '.') point = 1;
		}
		switch (key)
		{
//---------------------------------------- Действия при нажатии кнопки 1 -----------------------------------------------------------//
			case 1:																													//Если нажата кнопка  ВВЕРХ
				switch (type)
				{
					case SWITCH_ELEMENT:																							//Если выбранный элемент содержит параметр - переключатель
						if (_temp_Switch == 0) _temp_Switch = 1;																	//Изменение параметра с 0 на 1 - с 1 на 0
						else _temp_Switch = 0;
						break;
					
					case PARAM_ELEMENT:																								//Если выбранный элемент содержит числовой параметр
						if (_temp_Array[cursor_Pos] == '+')																			//Проверка выбранного элемента массива на знак + и -
						{
							_temp_Array[cursor_Pos] = '-';
						}
						else if (_temp_Array[cursor_Pos] == '-')
						{
							_temp_Array[cursor_Pos] = '+';
						}
						else
						{
							_temp_Array[cursor_Pos]++;																				//Если выбранный элемент не содержит '+/-' то инкрементируем значение
							if (cursor_Pos == 0 || cursor_Pos == max_lenght-1 || point == 1)										//Если курсор находит на нулевой или последней позиции, или уже поставлено точка
							{	
								if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x30;									//Ввод цифр от 0 до 9
								else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;															
							}
							else
							{
								if (cursor_Pos == 1)																				//Если курсор находится на позиции 1
								{
									if (_temp_Array[0] == '+' || _temp_Array[0] == '-')												//А на нулевой стоит '+/-'
									{
										if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x30;							//То ввод цифр от 0 до 9
										else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;
									}
									else																							//Иначе ввод цифр от 0 до 9 и '.'
									{
										if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x2E;
										else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;
									}
								}
								else																								//Если точка в массиве не найдена
								{
									if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x2E;								//Ввод цифр от 0 до 9 и '.'
									else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;
								}
							}
						}
						break;
					
					case WORD_ELEMENT:																								//Если выбранный элемент содержит текстовой параметр
						_temp_Array[cursor_Pos]++;																					//Инкрементирование элемента массива
						if (_temp_Array[cursor_Pos] == 'Я' + 1) _temp_Array[cursor_Pos] = '0';
						else if (_temp_Array[cursor_Pos] == '9' + 1)  _temp_Array[cursor_Pos] = '.';								//Ввод спец символов после буквы Я
						else if (_temp_Array[cursor_Pos] == '.' + 1)  _temp_Array[cursor_Pos] = '/';
						else if (_temp_Array[cursor_Pos] == '/' + 1)  _temp_Array[cursor_Pos] = '^';
						else if (_temp_Array[cursor_Pos] == '^' + 1)  _temp_Array[cursor_Pos] = ' ';
						else if (_temp_Array[cursor_Pos] == ' ' + 1)  _temp_Array[cursor_Pos] = 'А';
						break;
						
					case LIST_ELEMENT:
						_temp_List++;
						if (*(menu_Elements[selected_Element].data + _temp_List*max_lenght) == 255) _temp_List --;
						
						for (int i = 0; i < max_lenght; i++)
						{
							_temp_Array[i] = *(menu_Elements[selected_Element].data + max_lenght*_temp_List + i);
						}
						break;
						
					case PASSWORD_ELEMENT:
						_temp_Array[cursor_Pos]++;																				//Если выбранный элемент не содержит '+/-' то инкрементируем значение
						if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x30;										//Ввод цифр от 0 до 9
						else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;
						break;
				}
			break;
			
//---------------------------------------- Действия при нажатии кнопки 2 -----------------------------------------------------------//
			case 2:																													//Если нажата кнопка ВНИЗ
				switch (type)
				{
					case SWITCH_ELEMENT:																							//Если выбранный элемент содержит параметр - переключатель
						if (_temp_Switch == 0) _temp_Switch = 1;																	//Изменение параметра с 0 на 1 - с 1 на 0
						else _temp_Switch = 0;
					break;
					
					case PARAM_ELEMENT:																								//Если выбранный элемент содержит числовой параметр
						if (_temp_Array[cursor_Pos] == '+')																			//Проверка выбранного элемента массива на знак + и -
						{
							_temp_Array[cursor_Pos] = '-';
						}
						else if (_temp_Array[cursor_Pos] == '-')
						{
							_temp_Array[cursor_Pos] = '+';
						}
						else
						{
							_temp_Array[cursor_Pos]--;																				//Если выбранный элемент не содержит '+/-' то декрементируем значение
							if (cursor_Pos == 0 || cursor_Pos == max_lenght-1 || point == 1)										//Если курсор находит на нулевой или последней позиции, или уже поставлено точка
							{
								if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x39;									//Ввод цифр от 0 до 9
							}
							else
							{
								if (cursor_Pos == 1)																				//Если курсор находится на позиции 1
								{
									if (_temp_Array[0] == '+' || _temp_Array[0] == '-')												//А на нулевой стоит '+/-'
									{
										if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x39;							//То ввод цифр от 0 до 9
									}
									else																							//Иначе ввод цифр от 0 до 9 и '.'
									{
										if (_temp_Array[cursor_Pos] < 0x2E) _temp_Array[cursor_Pos] = 0x39;
										else if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x2E;
									}
								}
								else																								//Если точка в массиве не найдена
								{
									if (_temp_Array[cursor_Pos] < 0x2E) _temp_Array[cursor_Pos] = 0x39;								//Ввод цифр от 0 до 9 и '.'
									else if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x2E;
								}
							}
						}
						break;
					
					case WORD_ELEMENT:																								//Если выбранный элемент содержит текстовой параметр
						_temp_Array[cursor_Pos]--;																					//Декрементирование элемента массива
						if (_temp_Array[cursor_Pos] == 'А' - 1) _temp_Array[cursor_Pos] = ' ';
						else if (_temp_Array[cursor_Pos] == ' ' - 1) _temp_Array[cursor_Pos] = '^';									//Ввод спец символов после буквы А
						else if (_temp_Array[cursor_Pos] == '^' - 1) _temp_Array[cursor_Pos] = '/';
						else if (_temp_Array[cursor_Pos] == '/' - 1) _temp_Array[cursor_Pos] = '.';
						else if (_temp_Array[cursor_Pos] == '.' - 1) _temp_Array[cursor_Pos] = '9';
						else if (_temp_Array[cursor_Pos] == '0' - 1) _temp_Array[cursor_Pos] = 'Я';
						break;
					
					case LIST_ELEMENT:
						if (_temp_List != 0) _temp_List--;
						
						for (int i = 0; i < max_lenght; i++)
						{
							_temp_Array[i] = *(menu_Elements[selected_Element].data + max_lenght*_temp_List + i);
						}
						break;
						
					case PASSWORD_ELEMENT:
						_temp_Array[cursor_Pos]--;																					//Если выбранный элемент не содержит '+/-' то инкрементируем значение
						if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x39;											//Ввод цифр от 0 до 9
						break;
				}
			break;
		}
	}
	return cursor_Pos;																												//Возврат текущего пложения курсора
}

//==================================== Функция возврата временного значения параметра ==============================================//
//Функция принимает аргументы:
//*array_Byte - ссылку на начало записываемого буффера.
//      *type - ссылку на переменную типа изменяемого параметра.
//Функция возвращяет:
//selected_Element - порядковый номер элемента, параметр которого изменяется.
uint8_t _Menu_Data_Change(uint8_t *array_Byte, uint8_t *type)
{
	uint8_t el_Type = menu_Elements[_selected_Element].element_Type;
	uint8_t count = menu_Elements[_selected_Element].array_Size;
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	if (_my_Lib_Flags & lib_Flag_Save_Change)																						//Если было произведено сохранение изменения
	{	
		_my_Lib_Flags &= ~lib_Flag_Save_Change;																						//Сброс флага сохранения изменения
		switch (el_Type)
		{
			case SWITCH_ELEMENT:																									//Если был изменен параметр - переключатель
				*array_Byte = _temp_Switch;																							//Запись нового значения в буфер
				break;
			
			case PARAM_ELEMENT:	
 			case WORD_ELEMENT:																										//Если был изменен текстовый параметр
			case PASSWORD_ELEMENT:
 				for (int i = 0; i < count; i++)
 				{
 					*array_Byte = _temp_Array[i];																					//Запись в буфер 
 					array_Byte++;
 				}
 				break;
				 
			case LIST_ELEMENT:
				for (int i = 0; i < count; i++)
				{
					*array_Byte = _temp_Array[i];																					//Запись в буфер
					array_Byte++;
				}
				break;
		}
		*type = el_Type;
		return _selected_Element;																									//Возврат порядкового номера измененного элемента меню
 	}
 	else
 	{
 		return NONE;																												//Если изменения не было возврат 0
 	}
}

//============================================= Функция перезаписи параметра элемента ==============================================//
//Функция принимает указатель на массив с новым значением параметра, номер элемента измененного параметра и перезаписывает параметр элемента 
//Функция принимает аргументы:
//   *ptr - ссылку на начало буффера с новым значением параметра.
//element - номер изменяемого элемента.
void Save_Changes(uint16_t element, uint8_t *ptr)
{
	uint8_t temp = 0;
	uint8_t temp2 = 0;
	uint8_t count = menu_Elements[element].array_Size;
	uint8_t* data_Pointer = menu_Elements[_selected_Element].data;
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	uint8_t *ptr_Data = menu_Elements[element].data;
	_History_Save();																												//Сохранить в список истории предыдущий параметр
																										
	switch (menu_Elements[element].element_Type)																					//Выбор типа редактирования параметра по его типу
	{
		case SWITCH_ELEMENT:																										//Если изменяемый параметр - переключатель
			*ptr_Data = *ptr;
			break;
		
		case PASSWORD_ELEMENT:
		case PARAM_ELEMENT:	
 		case WORD_ELEMENT:																											//Если изменяемый параметр знаковый
 			for (int i = 0; i < count; i++)
 			{
 				*ptr_Data = *ptr;
 				ptr_Data++;
 				ptr++;
 			}
			break;
			
		case LIST_ELEMENT:
			for (int i = 0; ; i++)
			{
				if (*(data_Pointer+i*count) == 255) break;
				
				if (*(data_Pointer+i*count) == 0) temp2 = i;
				if (*(data_Pointer+i*count) == _temp_Array[0]) temp = i;
			}
			
			*(data_Pointer+temp2*count) = _temp_Array[0];
			*(data_Pointer+temp*count) = 0;
			break;
	}
}
//============================================== Функция опроса кнопок =============================================================//
//Функция опроса кнопок внутри меню и вывода на экран информации. Функция циклически повторяется, пока не будет проинициализарован выход из меню.
//Функция возвращает номер измененного элемента.
//Функция принимает ссылку на временный буфер нового значения параметра и его тип.
uint16_t Goto_Menu (uint8_t *array_Byte, uint8_t *type)
{
	_my_Lib_Flags |= lib_Flag_Menu_Enable | lib_Flag_Set_Changes;																	//Установка флагов ("Меню активно" и "Изменения на индикаторе")
	int h=2500;
	while(1)
	{
		h--;
		if (h==1)
		{
			_my_Lib_Flags &= ~lib_Flag_Menu_Enable;																			//Сброс флага "Меню активно"
			_my_Lib_Flags |= lib_Flag_Menu_Out;																				//Установка флага "Выход из меню"
			LED_Clear();
			return EXIT;
		}
		_delay_ms(10);
		static uint8_t element_Count = 0;																							//Порядковый номер элемента меню в текущем подменю
		static uint8_t submenu_Count = 0;																							//Порядковый номер подменю
		static uint8_t cursor_Pos = 255;																							//Позиция курсора
		static uint8_t led_Curs = 0;																								//Вспомогательная переменная для расчета времения мигания блоком текста
		static uint8_t state = 0;																									//Вспомогательная переменная для расчета времения мигания блоком текста
		uint8_t stop = 0;																											//Вспомогательная переменная
		uint8_t *temp_Byte = 0;																										//Указатель на параметр текущего элемента меню
		
		_my_Lib_Flags &= ~lib_Flag_Save_Change;
		
//--------------------------------------- Если был произведен выход их меню --------------------------------------------------------//
		if (_my_Lib_Flags & lib_Flag_Menu_Out)
		{
			_my_Lib_Flags &= ~lib_Flag_Menu_Out;
			element_Count = 0;
			submenu_Count = 0;
		}

//------------------------------------------------- Обработка кнопки 0 -------------------------------------------------------------//
		_Selected_Element_Calculate(element_Count, submenu_Count);																	//Расчет порядкового номера элемента меню
		if (Key(0) == 1)																											//Если нажата кнопка 0
		{
			h=2500;
			uint16_t time_Down = 0;																									//Счетчик времени нажатия
			while(Key(0) == 1)
			{
				 
				_delay_ms(3);																										//Антидребезг
				if (_my_Lib_Flags & lib_Flag_Changing_Param)
				{
					if (time_Down < 300)
					{
						time_Down++;																								//Увеличение счетчика нажатия пока время нажатия не достигнет 3 сек
					}
					else
					{
						LED_SetPos(1, 17);																							//Вывод курсора за область индикатора
						cursor_Pos = 255;																							//Перевод курсора в стандартное положение
						stop = 1;																									//Остановить мигание блоком текста
						_my_Lib_Flags |= lib_Flag_Save_Change;																		//Установить флаг сохраненного изменения
						_my_Lib_Flags &= ~lib_Flag_Changing_Param;																	//Сбросить флаг редактирвоания параметра
						Led(1);																										//Зажечь светодиод
					}
				}
				else if (menu_Elements[_selected_Element].element_Type == DEFAULT_ELEMENT)											//Если не производится изменение параметра, но выбран элемент дефолтных настроек
				{
					if (time_Down < 300)
					{
						time_Down++;																								//Увеличение счетчика нажатия пока время нажатия не достигнет 3 сек
					}
					else
					{
						_my_Lib_Flags |= lib_Flag_Save_Change;																		//Установить флаг сохраненного изменения
						_my_Lib_Flags &= ~lib_Flag_Menu_Enable;																		//Сбросить флаг редактирвоания параметра
						element_Count = 0;																							//Выход их меню
						submenu_Count = 0;
						Led(1);																										//Зажечь светодиод
					}
				}
				else if (menu_Elements[_selected_Element].element_Type == BLANK_ELEMENT)
				{
					if (time_Down < 300)
					{
						time_Down++;																								//Увеличение счетчика нажатия пока время нажатия не достигнет 3 сек
					}
					else
					{
						_my_Lib_Flags |= lib_Flag_Save_Change;																		//Установить флаг сохраненного изменения
						_my_Lib_Flags &= ~lib_Flag_Menu_Enable;																		//Сбросить флаг редактирвоания параметра
						Led(1);																										//Зажечь светодиод
					}
				}
				
				if (_my_Lib_Flags & lib_Flag_Changing_Param)	//Если производится изменение параметра и выбран элемент с параметром - переключателем
				{
					if (menu_Elements[_selected_Element].element_Type == SWITCH_ELEMENT)
					{
						if (led_Curs < 70)
						{
							_delay_ms(1);
							led_Curs++;																									//Инкрементирование счетчика
						}
						else																											//Если счетчик достиг 500 мс
						{
							led_Curs = 0;																								//Сброс счетчика
							LED_SetPos(1,0);
							if (state != 0)																								//Если текста нет
							{
								state = 0;
								if (_temp_Switch != 0) LED_Write_String_P(PSTR("Вкл"));													//Вывод текста
								else LED_Write_String_P(PSTR("Выкл"));
							}
							else																										//Если текст выведен
							{
								state = 1;
								LED_Write_String_P(PSTR("    "));																		//Затирание текста
							}
						}
						LED_SetPos(1,17);
					}
					else if (menu_Elements[_selected_Element].element_Type == LIST_ELEMENT)
					{
						if (led_Curs < 70)
						{
							_delay_ms(1);
							led_Curs++;																									//Инкрементирование счетчика
						}
						else																											//Если счетчик достиг 500 мс
						{
							led_Curs = 0;																								//Сброс счетчика
							LED_SetPos(1,0);
							if (state != 0)																								//Если текста нет
							{
								state = 0;
								for (int i = 1; i < menu_Elements[_selected_Element].array_Size; i++)
								{
									_LED_Write_Char(_temp_Array[i]);
								}
							}
							else																										//Если текст выведен
							{
								state = 1;
								LED_Write_String_P(PSTR("    "));																		//Затирание текста
							}
						}
						LED_SetPos(1,5);
					}
				}
				else if ((_my_Lib_Flags & lib_Flag_Save_Change) && menu_Elements[_selected_Element].element_Type == SWITCH_ELEMENT && stop == 1)//Если вспомогательная переменная stop == 1
				{
					LED_SetPos(1,0);
					if (_temp_Switch != 0) LED_Write_String_P(PSTR("Вкл"));															//Всегда отображать текст
					else LED_Write_String_P(PSTR("Выкл"));
					LED_SetPos(1,17);
					stop = 0;																										//Сброс переменной
				}
			}
			
			if (_my_Lib_Flags & lib_Flag_Save_Change && (menu_Elements[_selected_Element].element_Type == DEFAULT_ELEMENT || menu_Elements[_selected_Element].element_Type == BLANK_ELEMENT))
			{
				*type = BLANK_ELEMENT;
				Led(0);
				return _selected_Element;
			}
			
			if (menu_Elements[_selected_Element].child_Submenu != 0)																//Проверка на на наличие дочернего подменю
			{
				element_Count = 0;
				submenu_Count = menu_Elements[_selected_Element].child_Submenu;														//Переход в дочернее подменю
			}
			else
			{
				if (_my_Lib_Flags & lib_Flag_Save_Change)																			//Если установлен флаг сохраненного изменения
				{
					_my_Lib_Flags &= ~lib_Flag_Changing_Param;																		//Сброс флага внесения изменений
				}
				//Если выбранный элемент содержит числой, текстовой, или переключаемый параметр
				else if (menu_Elements[_selected_Element].element_Type == SWITCH_ELEMENT || menu_Elements[_selected_Element].element_Type == PARAM_ELEMENT || menu_Elements[_selected_Element].element_Type == WORD_ELEMENT || menu_Elements[_selected_Element].element_Type == LIST_ELEMENT || menu_Elements[_selected_Element].element_Type == PASSWORD_ELEMENT)
				{
					_my_Lib_Flags |= lib_Flag_Changing_Param;																		//Установка флага внесения изменений
					if (menu_Elements[_selected_Element].element_Type == PARAM_ELEMENT) cursor_Pos = _New_Param(0, cursor_Pos, _selected_Element);//Установка курсора
					if (menu_Elements[_selected_Element].element_Type == WORD_ELEMENT) cursor_Pos = _New_Param(0, cursor_Pos, _selected_Element);
					if (menu_Elements[_selected_Element].element_Type == PASSWORD_ELEMENT) cursor_Pos = _New_Param(0, cursor_Pos, _selected_Element);
				}
			}
			_my_Lib_Flags |= lib_Flag_Set_Changes;																					//Установка флага вывода на индикатор
			Led(0);																													//Отключение светодиода
		}

//------------------------------------------------------ Обработка кнопки 3 --------------------------------------------------------//
		if (Key(3) == 1)																											//Если нажата кнопка НАЗАД
		{
			h=2500;
			while(Key(3) == 1)																										//Антидребезг
			{
				for (int i = 0; i < 20; i++)
				{
					_delay_us(500);
					 
				}
			}																						

			if (submenu_Count != 0)																									//Если текущее подменю не нулевое
			{
				if (_my_Lib_Flags & lib_Flag_Changing_Param)																		//Если производится изменение параметра
				{
					
				}
				else																												//Если не производится изменение значения
				{
					_selected_Element = submenu[submenu_Count].parent_Element_Number;												//Переход в подменю
					element_Count = menu_Elements[_selected_Element].number_In_Submenu;
					uint16_t temp = submenu_Count;
					uint16_t temp2 = 0;
					
					for (int i = 0; i < temp; i++)
					{
						temp2 += submenu[i].elements_Count_In_Submenu;
						if (temp2 > _selected_Element)
						{
							submenu_Count = i;
							break;
						}
					}
				}
			}
			else																													//Если текущее подменю нулевое
			{
				if (_my_Lib_Flags & lib_Flag_Changing_Param)																		//Если производится изменение параметра
				{
					switch(menu_Elements[_selected_Element].element_Type)
					{
						case PARAM_ELEMENT:																							//Если выбран элемент с числовым параметром
						temp_Byte = menu_Elements[_selected_Element].data;															//Присвоение указателю адрес на массив текущего сохраннего значения
						for (int i = 0; i < sizeof(_temp_Array); i++)
						{
							_temp_Array[i] = *temp_Byte;																			//Восстановление прежнего значения
							temp_Byte++;
						}
						break;
						
						case SWITCH_ELEMENT:																						//Если выбран элемент с параметром переключателем
						_temp_Switch = *menu_Elements[_selected_Element].data;														//Восстановление прежнего значения
						break;
					}
				}
				else																												//Если не производится изменение значения
				{
					_my_Lib_Flags &= ~lib_Flag_Menu_Enable;																			//Сброс флага "Меню активно"
					_my_Lib_Flags |= lib_Flag_Menu_Out;																				//Установка флага "Выход из меню"
					LED_Clear();																									//Очистка дисплея
					return EXIT;																									//Возврат в основную программу
				}
			}
			
			cursor_Pos = 255;																										//Стандартная позиция курсора
			_line2 = _line-1;																										//Определение отображаемого элемента массива истории (Последний внесенный)
			_my_Lib_Flags &= ~lib_Flag_Changing_Param;																				//Сброс флага внесения изменений
			_my_Lib_Flags |= lib_Flag_Set_Changes;																					//Установка флага вывода на индикатор
		}
		
//--------------------------------------------------- Обработка кнопки 1 -----------------------------------------------------------//
		if (Key(2) == 1)																											//Если нажата кнопка ВВЕРХ
		{
			h=2500;
			while(Key(2) == 1)
			{
				for (int i = 0; i < 20; i++)
				{
					_delay_us(500);
					 
				}
			}																						//Антидребезг
			
			if (_my_Lib_Flags & lib_Flag_Changing_Param)																			//Если флаг изменения параметров установлен
			{
				_New_Param(1, cursor_Pos, _selected_Element);																		//Вызов функции внесения изменений в параметр
			}
			else
			{
				if (menu_Elements[_selected_Element].element_Type == HISTORY_ELEMENT)												//Если выбранные элемент содержит список внесенных изменений
				{
					_line2--;
					if (_line2 == 255) _line2 = _line-1;																			//Зацикливание списка
				}
				else
				{
					element_Count++;																								//Навигация по подменю
					if (element_Count > submenu[submenu_Count].elements_Count_In_Submenu-1) element_Count = 0;						//Зацикливание списка отображаемых элементов подменю
				}
			}
			_my_Lib_Flags |= lib_Flag_Set_Changes;																					//Установка флага внесения изменений на индикатор
		}

//--------------------------------------------------- Обработка кнопки 2 -----------------------------------------------------------//
		if (Key(1) == 1)																											//Если нажата кнопка ВНИЗ
		{
			h=2500;
			while(Key(1) == 1)
			{
				for (int i = 0; i < 20; i++)
				{
					_delay_us(500);
					 
				}
			}																						//Антидребезг
			
			if (_my_Lib_Flags & lib_Flag_Changing_Param)																			//Если установлен флаг внесения изменений
			{
				_New_Param(2, cursor_Pos, _selected_Element);																		//Вызов функции внесения изменений
			}
			else
			{
				if (menu_Elements[_selected_Element].element_Type == HISTORY_ELEMENT)												//Если выбранные элемент содержит список внесенных изменений
				{

					_line2++;
					if (_line2 > _line-1) _line2 = 0;																				//Зацикливание списка
				}
				else
				{
					if (element_Count == 0) element_Count = submenu[submenu_Count].elements_Count_In_Submenu-1;						//Зацикливание списка отображаемых элементов подменю
					else element_Count--;																							//Навигация по подменю
				}
			}
			_my_Lib_Flags |= lib_Flag_Set_Changes;																					//Установка флага внесения изменений на индикатор
		}
		
//----------------------------------------- Мигание блоком текста ------------------------------------------------------------------//
		if ((_my_Lib_Flags & lib_Flag_Changing_Param))			//Если установлен флаг изменения параметров и выбран элемент с параметром - переключателем
		{
			if (menu_Elements[_selected_Element].element_Type == SWITCH_ELEMENT)
			{
				if (led_Curs < 100)
				{
					_delay_ms(2);
					led_Curs++;																											//Инкрементирование счетчика
				}
				else																													//Если счетчик достиг 500 мс
				{
					led_Curs = 0;																										//Сброс счетчика
					LED_SetPos(1,0);
					if (state != 0)																										//Если текста нет
					{
						state = 0;
						if (_temp_Switch != 0) LED_Write_String_P(PSTR("Вкл"));															//Вывод текста
						else LED_Write_String_P(PSTR("Выкл"));
					}
					else																												//Если текст выведен
					{
						state = 1;
						LED_Write_String_P(PSTR("    "));																				//Затирание текста
					}
				}
				LED_SetPos(1,5);
			}
			else if (menu_Elements[_selected_Element].element_Type == LIST_ELEMENT)
			{
				if (led_Curs < 100)
				{
					_delay_ms(2);
					led_Curs++;																											//Инкрементирование счетчика
				}
				else																													//Если счетчик достиг 500 мс
				{
					led_Curs = 0;																										//Сброс счетчика
					LED_SetPos(1,0);
					if (state != 0)																										//Если текста нет
					{
						state = 0;
						for (int i = 1; i < menu_Elements[_selected_Element].array_Size; i++)
						{
							_LED_Write_Char(_temp_Array[i]);
						}
					}
					else																												//Если текст выведен
					{
						state = 1;
						LED_Write_String_P(PSTR("    "));																				//Затирание текста
					}
				}
				LED_SetPos(1,5);
			}
		}
		
		 
//---------------------------------------- Вывод изменений на индикатор ------------------------------------------------------------//
		if ( _my_Lib_Flags & lib_Flag_Set_Changes)																					//Если установлен флаг внесения изменений на индикатор
		{
			LED_Clear();																											//Очистка экрана
			_my_Lib_Flags &= ~lib_Flag_Set_Changes;																					//Сброс флага внесения изменений на индикатор
			_Selected_Element_Calculate(element_Count, submenu_Count);																//Вычисление порядкового номера текущего элемента меню
			_Display_Out(cursor_Pos);																								//Вызов функции отображения информации на индикаторе
		}
		
		uint16_t ret = _Menu_Data_Change(array_Byte, type);																			//Поиск номера измененного элемента
		if (ret != NONE) return ret;
	}
}
#endif
