/*
 * menu_boiler.h
 *
 * Created: 13.03.2018
 * Author: Antipin
 * Создание меню и сохранение данных при изменении
 */ 

#ifndef __menu_boiler
#define __menu_boiler

//============Функция создания элементов меню=================//
void inline create_menu()
{
	Add_New_Menu(0, 2);
	Add_New_Element(0, EMPTY_ELEMENT, 1, PSTR("Настройки"), NONE, NONE);
	Add_New_Element(1, PARAM_ELEMENT, NONE, PSTR("Пароль:"), Password_s, 4);
	
	Add_New_Menu(1, 6);
	
	Add_New_Element(2, EMPTY_ELEMENT, 8, PSTR("Контур №1"), NONE, NONE);
	Add_New_Element(3, EMPTY_ELEMENT, 11, PSTR("Контур №2"), NONE, NONE);
	Add_New_Element(4, EMPTY_ELEMENT, 14, PSTR("Контур №3"), NONE, NONE);
	Add_New_Element(5, EMPTY_ELEMENT, 2, PSTR("Измен. настроек/n времени"), NONE, NONE);
	Add_New_Element(6, EMPTY_ELEMENT, 6, PSTR("Период ШИМ"), NONE, NONE);
	Add_New_Element(7, EMPTY_ELEMENT, 7, PSTR("Период опроса/n датчиков"), NONE, NONE);
	
	//=================================Общие===============================//
	
	Add_New_Menu(2, 3);
	
	Add_New_Element(8, EMPTY_ELEMENT, 3, PSTR("Настройка текущ./n времени"), NONE, NONE);
	Add_New_Element(9, EMPTY_ELEMENT, 4, PSTR("Режимы день/ночь"), NONE, NONE);
	Add_New_Element(10, EMPTY_ELEMENT, 5, PSTR("Статус дней/n недели"), NONE, NONE);
	
	Add_New_Menu (3, 2);
	
	Add_New_Element(11, PARAM_ELEMENT, NONE, PSTR("Время"), cur_time, 5);
	Add_New_Element(12, LIST_ELEMENT, NONE, PSTR("День недели"), disp_DOW[0], 3);
	
	Add_New_Menu(4, 2);
	
	Add_New_Element(13, PARAM_ELEMENT, NONE, PSTR("Начало раб. дня"), time_nach_s, 5);
	Add_New_Element(14, PARAM_ELEMENT, NONE, PSTR("Конец раб. дня"), time_kon_s, 5);
	
	Add_New_Menu(5, 1);
	Add_New_Element(15, WORD_ELEMENT, NONE, PSTR("П В С Ч П С В"), hol_DOW_RAM, 7);
	
	Add_New_Menu (6, 1);
	Add_New_Element(16, PARAM_ELEMENT, NONE, PSTR("Период в сек"), per_shim_s, 4);
	
	Add_New_Menu(7, 1);
	Add_New_Element(17, PARAM_ELEMENT, NONE, PSTR("Период в  сек"), per_dat_s, 4);
	
	
	//====================1 контур=====================================//
	
	Add_New_Menu (8, 2);
	Add_New_Element(18, EMPTY_ELEMENT, 9, PSTR("Измен. настроек/n температуры"), NONE, NONE);
	Add_New_Element(19, EMPTY_ELEMENT, 10, PSTR("Измен. настроек/n регулятора"), NONE, NONE);
	
	Add_New_Menu(9, 2);
	Add_New_Element(20, PARAM_ELEMENT, NONE, PSTR("Температура день"), temp_den_s[0], 2);
	Add_New_Element(21, PARAM_ELEMENT, NONE, PSTR("Температура ночь"), temp_noch_s[0], 2);
	
	Add_New_Menu(10, 3);
	Add_New_Element(22, PARAM_ELEMENT, NONE, PSTR("Кп"), K_P_s[0], 6);
	Add_New_Element(23, PARAM_ELEMENT, NONE, PSTR("Ки"), K_I_s[0], 5);
	Add_New_Element(24, PARAM_ELEMENT, NONE, PSTR("Кд"), K_D_s[0], 6);

	//====================2 контур=====================================//
	
	Add_New_Menu (11, 2);
	Add_New_Element(25, EMPTY_ELEMENT, 12, PSTR("Измен. настроек/n температуры"), NONE, NONE);
	Add_New_Element(26, EMPTY_ELEMENT, 13, PSTR("Измен. настроек/n регулятора"), NONE, NONE);
	
	Add_New_Menu(12, 2);
	Add_New_Element(27, PARAM_ELEMENT, NONE, PSTR("Температура день"), temp_den_s[1], 2);
	Add_New_Element(28, PARAM_ELEMENT, NONE, PSTR("Температура ночь"), temp_noch_s[1], 2);
	
	Add_New_Menu(13, 3);
	Add_New_Element(29, PARAM_ELEMENT, NONE, PSTR("Кп"), K_P_s[1], 6);
	Add_New_Element(30, PARAM_ELEMENT, NONE, PSTR("Ки"), K_I_s[1], 5);
	Add_New_Element(31, PARAM_ELEMENT, NONE, PSTR("Кд"), K_D_s[1], 6);
	
	//====================3 контур=====================================//
	
	Add_New_Menu (14, 2);
	Add_New_Element(32, EMPTY_ELEMENT, 15, PSTR("Измен. настроек/n температуры"), NONE, NONE);
	Add_New_Element(33, EMPTY_ELEMENT, 16, PSTR("Измен. настроек/n регулятора"), NONE, NONE);
	
	Add_New_Menu(15, 2);
	Add_New_Element(34, PARAM_ELEMENT, NONE, PSTR("Температура день"), temp_den_s[2], 2);
	Add_New_Element(35, PARAM_ELEMENT, NONE, PSTR("Температура ночь"), temp_noch_s[2], 2);
	
	Add_New_Menu(16, 3);
	Add_New_Element(36, PARAM_ELEMENT, NONE, PSTR("Кп"), K_P_s[2], 6);
	Add_New_Element(37, PARAM_ELEMENT, NONE, PSTR("Ки"), K_I_s[2], 5);
	Add_New_Element(38, PARAM_ELEMENT, NONE, PSTR("Кд"), K_D_s[2], 6);
	
	//=================================================================//

}


//===Функция перезаписи переменных при их изменении из меню===//
//el - номер элемента, в котором были изменены параметры
void static settings_changed(uint8_t el)
{
	//Переменная для хранения промежуточных значений с последющей записью в EEPROM
	uint16_t val_16 = 0;
	uint32_t val_32 = 0;
	//cli();
	switch (el)
	{
		//Корректировка текущего времени
		case 11 :
			hour = (cur_time[0]-0x30)*10 + (cur_time[1]-0x30);
			min = (cur_time[3]-0x30)*10 + (cur_time[4]-0x30);
			RTC_Set_Time(hour, min, sec);
		break;
		
		//Корректировка текущего дня недели
		case 12 :
			//Поиск элемента списка с индексом "ноль"
			for (int i = 0; i<7; i++)
			{
				if (disp_DOW[i][0] == 0)
				{
					cur_DOW_RAM = i;
					//Запись порядкового номера элемента списка в EEPROM
					RTC_Set_Date(cur_DOW_RAM, flush, flush, flush);
				}
			}
			convert_to_show();
		break;
		
		//Корректировка времени начала рабочего дня
		case 13:
			//Корректировка часов
			char_to_dec_16(&time_nach_s[0], 2, &val_16);
			//Запись часов в EEPROM
			eeprom_write_word(&time_nach[0], val_16);
			time_nach_RAM[0] = val_16;
			//Корректировка минут
			char_to_dec_16(&time_nach_s[3], 2, &val_16);
			time_nach_RAM[1] = val_16;
			//Запись минут в EEPROM
			eeprom_write_word(&time_nach[1], val_16);
		break;
		
		//Корректировка времени конца рабочего дня
		case 14:
			//Корректировка часов
			char_to_dec_16(&time_kon_s[0], 2, &val_16);
			//Запись часов в EEPROM
			eeprom_write_word(&time_kon[0], val_16);
			time_kon_RAM[0] = val_16;
			//Корректировка минут
			char_to_dec_16(&time_kon_s[3], 2, &val_16);
			//Запись минут в EEPROM
			eeprom_write_word(&time_kon[1], val_16);
			time_kon_RAM[1] = val_16;
		break;
		
		//Корректировка текущего дня недели в списке
		case 15:
			for (int i = 0; i<7; i++)
			{
				eeprom_write_byte(&hol_DOW[i], hol_DOW_RAM[i]);
			}
		break;

		//Корректировка периода ШИМ
		case 16:
			char_to_dec_16(per_shim_s, 4, &val_16);
			if (val_16<1800)
			{
				LED_SetPos(1, 0);
				LED_Write_String_P(PSTR("ERROR"));
				_delay_ms(1000);
				break;
			}
			else
			{
				//Запись периода ШИМ в EEPROM
				eeprom_write_word(&per_shim, val_16);
				per_shim_RAM = val_16;
				time_10_shim = 0;
				break;
			}
		case 17:
			//Корректировка периода чтения датчиков
			char_to_dec_16(per_dat_s, 4, &val_16);
			//Запись периода чтения датчиков в EEPROM
			eeprom_write_word(&per_dat, val_16);
			per_dat_RAM = val_16;
			time_10_dat = 0;
		break;
		
		//=========Контур 1=============//
		case 20:
			//Корректировка требуемой температуры для дневного режима 1 контура
			char_to_dec_16(temp_den_s[0], 2, &val_16);
			//Запись требуемой температуры для дневного режима 1 контура в EEPROM
			eeprom_write_word(&temp_den[0], val_16);
			temp_den_RAM[0] = val_16;
		break;
		case 21:
			//Корректировка требуемой температуры для ночного режима 1 контура
			char_to_dec_16(temp_noch_s[0], 2, &val_16);
			//Запись требуемой температуры для ночного режима 1 контура в EEPROM
			eeprom_write_word(&temp_noch[0], val_16);
			temp_noch_RAM[0] = val_16;
		break;
		case 22:
			//Корректировка пропорционального коэффициента ПИД-регулятора 1 контура
			char_to_dec_32(K_P_s[0], 5, &val_32);
			//Запись пропорционального коэффициента ПИД-регулятора 1 контура в EEPROM
			eeprom_write_dword(&K_P[0], val_32);
			K_P_RAM[0] = val_32;
		break;
		case 23:
			//Корректировка интегрального коэффициента ПИД-регулятора 1 контура
			char_to_dec_32(K_I_s[0], 5, &val_32);
			//Запись интегрального коэффициента ПИД-регулятора 1 контура в EEPROM
			eeprom_write_dword(&K_I[0], val_32);
			K_I_RAM[0] = val_32;
		break;
		case 24:
			//Корректировка дифференциального коэффициента ПИД-регулятора 1 контура
			char_to_dec_32(K_D_s[0], 5, &val_32);
			//Запись дифференциального коэффициента ПИД-регулятора 1 контура в EEPROM
			eeprom_write_dword(&K_D[0], val_32);
			K_D_RAM[0] = val_32;
		break;
		
		//=========Контур 2=============//
		case 27:
			//Корректировка требуемой температуры для дневного режима 2 контура
			char_to_dec_16(temp_den_s[1], 2, &val_16);
			//Запись требуемой температуры для дневного режима 2 контура в EEPROM
			eeprom_write_word(&temp_den[1], val_16);
			temp_den_RAM[1] = val_16;
		break;
		case 28:
			//Корректировка требуемой температуры для ночного режима 2 контура
			char_to_dec_16(temp_noch_s[1], 2, &val_16);
			//Запись требуемой температуры для ночного режима 2 контура в EEPROM
			eeprom_write_word(&temp_noch[1], val_16);
			temp_noch_RAM[1] = val_16;
		break;
		case 29:
			//Корректировка пропорционального коэффициента ПИД-регулятора 2 контура
			char_to_dec_32(K_P_s[1], 5, &val_32);
			//Запись пропорционального коэффициента ПИД-регулятора 2 контура в EEPROM
			eeprom_write_dword(&K_P[1], val_32);
			K_P_RAM[1] = val_32;
		break;
		case 30:
			//Корректировка интегрального коэффициента ПИД-регулятора 2 контура
			char_to_dec_32(K_I_s[1], 5, &val_32);
			//Запись интегрального коэффициента ПИД-регулятора 2 контура в EEPROM
			eeprom_write_dword(&K_I[1], val_32);
			K_I_RAM[1] = val_32;
		break;
		case 31:
			//Корректировка дифференциального коэффициента ПИД-регулятора 2 контура
			char_to_dec_32(K_D_s[1], 5, &val_32);
			//Запись дифференциального коэффициента ПИД-регулятора 2 контура в EEPROM
			eeprom_write_dword(&K_D[1], val_32);
			K_D_RAM[1] = val_32;
		break;
		
		//=========Контур 3=============//
		case 34:
			//Корректировка требуемой температуры для дневного режима 3 контура
			char_to_dec_16(temp_den_s[2], 2, &val_16);
			//Запись требуемой температуры для дневного режима 3 контура в EEPROM
			eeprom_write_word(&temp_den[2], val_16);
			temp_den_RAM[2] = val_16;
		break;
		case 35:
			//Корректировка требуемой температуры для ночного режима 3 контура
			char_to_dec_16(temp_noch_s[2], 2, &val_16);
			//Запись требуемой температуры для ночного режима 3 контура в EEPROM
			eeprom_write_word(&temp_noch[2], val_16);
			temp_noch_RAM[2] = val_16;
		break;
		case 36:
			//Корректировка пропорционального коэффициента ПИД-регулятора 3 контура
			char_to_dec_32(K_P_s[2], 5, &val_32);
			//Запись пропорционального коэффициента ПИД-регулятора 3 контура в EEPROM
			eeprom_write_dword(&K_P[2], val_32);
			K_P_RAM[2] = val_32;
		break;
		case 37:
			//Корректировка интегрального коэффициента ПИД-регулятора 3 контура
			char_to_dec_32(K_I_s[2], 5, &val_32);
			//Запись интегрального коэффициента ПИД-регулятора 3 контура в EEPROM
			eeprom_write_dword(&K_I[2], val_32);
			K_I_RAM[2] = val_32;
		break;
		case 38:
			//Корректировка дифференциального коэффициента ПИД-регулятора 3 контура
			char_to_dec_32(K_D_s[2], 5, &val_32);
			//Запись дифференциального коэффициента ПИД-регулятора 3 контура в EEPROM
			eeprom_write_dword(&K_D[2], val_32);
			K_D_RAM[2] = val_32;
		break;
	}
}

void menu_in_loop()
{
	//Буфер элемента меню
	static uint8_t buf[8];
	//Тип элемента
	static uint8_t type = 0;
	//Номер элемента
	static uint16_t element = 0;

	//"Антидребезг"
	while(Key(0)==1) {_delay_ms(10);}
	//Флаг входа в меню МС
	flag_M = 1;
					
	while (1)
	{
		//Конвертация чиловых данных из EEPROM в символьные для отображения
		convert_to_show();
		//Отображение текущего элемента
		element = Goto_Menu(buf, &type);
		if (element!=NONE)
		{
			//Выход в родительское меню
			if (element == EXIT) break;
			//Вызов функции сохранения символьной информации
			Save_Changes(element, buf);
			//Вызов функции сохранения конвертации данных для записи в EEPROM
			settings_changed(element);
		}
	}
	// Флаг выхода из меню
	flag_M = 0;
	// Флаг отображения на экране
	flag_O = 1;
	//Обнуление флага отображения информации на главном экране
	flag_T = 0;

}
#endif
