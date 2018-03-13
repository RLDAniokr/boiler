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
	
	Add_New_Menu(0, 6);
	
	Add_New_Element(0, EMPTY_ELEMENT, 7, PSTR("Контур№1"), NONE, NONE);
	Add_New_Element(1, EMPTY_ELEMENT, 10, PSTR("Контур№2"), NONE, NONE);
	Add_New_Element(2, EMPTY_ELEMENT, 13, PSTR("Контур№3"), NONE, NONE);
	Add_New_Element(3, EMPTY_ELEMENT, 1, PSTR("Измен. настроек/n времени"), NONE, NONE);
	Add_New_Element(4, EMPTY_ELEMENT, 5, PSTR("Период ШИМ"), NONE, NONE);
	Add_New_Element(5, EMPTY_ELEMENT, 6, PSTR("Период опроса/n датчиков"), NONE, NONE);
	
	//=================================Общие===============================//
	
	Add_New_Menu(1, 3);
	
	Add_New_Element(6, EMPTY_ELEMENT, 2, PSTR("Настройка текущ./n времени"), NONE, NONE);
	Add_New_Element(7, EMPTY_ELEMENT, 3, PSTR("Режимы день/ночь"), NONE, NONE);
	Add_New_Element(8, EMPTY_ELEMENT, 4, PSTR("Статус дней/n недели"), NONE, NONE);
	
	Add_New_Menu (2, 2);
	
	Add_New_Element(9, PARAM_ELEMENT, NONE, PSTR("Время"), cur_time, 5);
	Add_New_Element(10, LIST_ELEMENT, NONE, PSTR("День недели"), disp_DOW[0], 3);
	
	Add_New_Menu(3, 2);
	
	Add_New_Element(11, PARAM_ELEMENT, NONE, PSTR("Начало раб. дня"), time_nach_s, 5);
	Add_New_Element(12, PARAM_ELEMENT, NONE, PSTR("Конец раб. дня"), time_kon_s, 5);
	
	Add_New_Menu(4, 1);
	
	Add_New_Element(13, WORD_ELEMENT, NONE, PSTR("П В С Ч П С В"), hol_DOW_RAM, 7);
	
	Add_New_Menu (5, 1);
	
	Add_New_Element(14, PARAM_ELEMENT, NONE, PSTR("Период в сек"), per_shim_s, 4);
	
	Add_New_Menu(6, 1);
	
	Add_New_Element(15, PARAM_ELEMENT, NONE, PSTR("Период в  сек"), per_dat_s, 4);
	
	
	//====================1 контур=====================================//
	
	Add_New_Menu (7, 2);
	
	Add_New_Element(16, EMPTY_ELEMENT, 8, PSTR("Измен. настроек/n температуры"), NONE, NONE);
	Add_New_Element(17, EMPTY_ELEMENT, 9, PSTR("Измен. настроек/n регулятора"), NONE, NONE);
	
	Add_New_Menu(8, 2);
	
	Add_New_Element(18, PARAM_ELEMENT, NONE, PSTR("Температура день"), temp_den_s[0], 2);
	Add_New_Element(19, PARAM_ELEMENT, NONE, PSTR("Температура ночь"), temp_noch_s[0], 2);
	
	Add_New_Menu(9, 3);
	
	Add_New_Element(20, PARAM_ELEMENT, NONE, PSTR("Кп"), K_P_s[0], 6);
	Add_New_Element(21, PARAM_ELEMENT, NONE, PSTR("Ки"), K_I_s[0], 5);
	Add_New_Element(22, PARAM_ELEMENT, NONE, PSTR("Кд"), K_D_s[0], 6);

	//====================2 контур=====================================//
	
	Add_New_Menu (10, 2);
	
	Add_New_Element(23, EMPTY_ELEMENT, 11, PSTR("Измен. настроек/n температуры"), NONE, NONE);
	Add_New_Element(24, EMPTY_ELEMENT, 12, PSTR("Измен. настроек/n регулятора"), NONE, NONE);
	
	Add_New_Menu(11, 2);
	
	Add_New_Element(25, PARAM_ELEMENT, NONE, PSTR("Температура день"), temp_den_s[1], 2);
	Add_New_Element(26, PARAM_ELEMENT, NONE, PSTR("Температура ночь"), temp_noch_s[1], 2);
	
	Add_New_Menu(12, 3);
	
	Add_New_Element(27, PARAM_ELEMENT, NONE, PSTR("Кп"), K_P_s[1], 6);
	Add_New_Element(28, PARAM_ELEMENT, NONE, PSTR("Ки"), K_I_s[1], 5);
	Add_New_Element(29, PARAM_ELEMENT, NONE, PSTR("Кд"), K_D_s[1], 6);
	
	//====================3 контур=====================================//
	
	Add_New_Menu (13, 2);
	
	Add_New_Element(30, EMPTY_ELEMENT, 14, PSTR("Измен. настроек/n температуры"), NONE, NONE);
	Add_New_Element(31, EMPTY_ELEMENT, 15, PSTR("Измен. настроек/n регулятора"), NONE, NONE);
	
	Add_New_Menu(14, 2);
	
	Add_New_Element(32, PARAM_ELEMENT, NONE, PSTR("Температура день"), temp_den_s[2], 2);
	Add_New_Element(33, PARAM_ELEMENT, NONE, PSTR("Температура ночь"), temp_noch_s[2], 2);
	
	Add_New_Menu(15, 3);
	
	Add_New_Element(34, PARAM_ELEMENT, NONE, PSTR("Кп"), K_P_s[2], 6);
	Add_New_Element(35, PARAM_ELEMENT, NONE, PSTR("Ки"), K_I_s[2], 5);
	Add_New_Element(36, PARAM_ELEMENT, NONE, PSTR("Кд"), K_D_s[2], 6);
	
	//=================================================================//

}

//===Функция перезаписи переменных при их изменении из меню===//
void inline settings_changed(uint8_t el)																									//el - номер элемента, в котором были изменены параметры
{
	uint16_t val_16;																												//Переменная для хранения промежуточных значений с последющей записью в EEPROM
	uint32_t val_32;
	//cli();
	switch (el)
	{
		case 9 :																													//Корректировка текущего времени
		hour = (cur_time[0]-0x30)*10 + (cur_time[1]-0x30);
		min = (cur_time[3]-0x30)*10 + (cur_time[4]-0x30);
		RTC_Set_Time(hour, min, sec);
		break;
		case 10 :																													//Корректировка текущего дня недели
		for (int i = 0; i<7; i++)																									//Поиск элемента списка с индексом "ноль"
		{
			if (disp_DOW[i][0] == 0)
			{
				cur_DOW_RAM = i;
				RTC_Set_Date(cur_DOW_RAM, flush, flush, flush);																		//Запись порядкового номера элемента списка в EEPROM
			}
		}
		convert_to_show();
		break;
		case 11:																													//Корректировка времени начала рабочего дня
		char_to_dec_16(&time_nach_s[0], 2, &val_16);																				//Корректировка часов
		eeprom_write_word(&time_nach[0], val_16);																					//Запись часов в EEPROM
		time_nach_RAM[0] = val_16;
		char_to_dec_16(&time_nach_s[3], 2, &val_16);																				//Корректировка минут
		time_nach_RAM[1] = val_16;
		eeprom_write_word(&time_nach[1], val_16);																					//Запись минут в EEPROM
		break;
		case 12:																													//Корректировка времени конца рабочего дня
		char_to_dec_16(&time_kon_s[0], 2, &val_16);																					//Корректировка часов
		eeprom_write_word(&time_kon[0], val_16);																					//Запись часов в EEPROM
		time_kon_RAM[0] = val_16;
		char_to_dec_16(&time_kon_s[3], 2, &val_16);																					//Корректировка минут
		eeprom_write_word(&time_kon[1], val_16);																					//Запись минут в EEPROM
		time_kon_RAM[1] = val_16;
		break;
		case 13:																													//Корректировка текущего дня недели в списке
		for (int i = 0; i<7; i++)
		{
			eeprom_write_byte(&hol_DOW[i], hol_DOW_RAM[i]);
		}
		break;
		case 14:
		char_to_dec_16(per_shim_s, 4, &val_16);																						//Корректировка периода ШИМ
		if (val_16<1800)
		{
			LED_SetPos(1, 0);
			LED_Write_String_P(PSTR("ERROR"));
			break;
		}
		else
		{
			eeprom_write_word(&per_shim, val_16);																					//Запись периода ШИМ в EEPROM
			per_shim_RAM = val_16;
			time_10_shim = 0;
			break;
		}
		case 15:
		char_to_dec_16(per_dat_s, 4, &val_16);																						//Корректировка периода чтения датчиков
		eeprom_write_word(&per_dat, val_16);																						//Запись периода чтения датчиков в EEPROM
		per_dat_RAM = val_16;
		time_10_dat = 0;
		break;
		
		//=========Контур 1=============//
		case 18:
		char_to_dec_16(temp_den_s[0], 2, &val_16);																					//Корректировка требуемой температуры для дневного режима 1 контура
		eeprom_write_word(&temp_den[0], val_16);																					//Запись требуемой температуры для дневного режима 1 контура в EEPROM
		temp_den_RAM[0] = val_16;
		break;
		case 19:
		char_to_dec_16(temp_noch_s[0], 2, &val_16);																					//Корректировка требуемой температуры для ночного режима 1 контура
		eeprom_write_word(&temp_noch[0], val_16);																					//Запись требуемой температуры для ночного режима 1 контура в EEPROM
		temp_noch_RAM[0] = val_16;
		break;
		case 20:
		char_to_dec_32(K_P_s[0], 5, &val_32);																						//Корректировка пропорционального коэффициента ПИД-регулятора 1 контура
		eeprom_write_dword(&K_P[0], val_32);																						//Запись пропорционального коэффициента ПИД-регулятора 1 контура в EEPROM
		K_P_RAM[0] = val_32;
		break;
		case 21:
		char_to_dec_32(K_I_s[0], 5, &val_32);																						//Корректировка интегрального коэффициента ПИД-регулятора 1 контура
		eeprom_write_dword(&K_I[0], val_32);																						//Запись интегрального коэффициента ПИД-регулятора 1 контура в EEPROM
		K_I_RAM[0] = val_32;
		break;
		case 22:
		char_to_dec_32(K_D_s[0], 5, &val_32);																						//Корректировка дифференциального коэффициента ПИД-регулятора 1 контура
		eeprom_write_dword(&K_D[0], val_32);																						//Запись дифференциального коэффициента ПИД-регулятора 1 контура в EEPROM
		K_D_RAM[0] = val_32;
		break;
		//=========Контур 2=============//
		case 25:
		char_to_dec_16(temp_den_s[1], 2, &val_16);																					//Корректировка требуемой температуры для дневного режима 2 контура
		eeprom_write_word(&temp_den[1], val_16);																					//Запись требуемой температуры для дневного режима 2 контура в EEPROM
		temp_den_RAM[1] = val_16;
		break;
		case 26:
		char_to_dec_16(temp_noch_s[1], 2, &val_16);																					//Корректировка требуемой температуры для ночного режима 2 контура
		eeprom_write_word(&temp_noch[1], val_16);																					//Запись требуемой температуры для ночного режима 2 контура в EEPROM
		temp_noch_RAM[1] = val_16;
		break;
		case 27:
		char_to_dec_32(K_P_s[1], 5, &val_32);																						//Корректировка пропорционального коэффициента ПИД-регулятора 2 контура
		eeprom_write_dword(&K_P[1], val_32);																						//Запись пропорционального коэффициента ПИД-регулятора 2 контура в EEPROM
		K_P_RAM[1] = val_32;
		break;
		case 28:
		char_to_dec_32(K_I_s[1], 5, &val_32);																						//Корректировка интегрального коэффициента ПИД-регулятора 2 контура
		eeprom_write_dword(&K_I[1], val_32);																						//Запись интегрального коэффициента ПИД-регулятора 2 контура в EEPROM
		K_I_RAM[1] = val_32;
		break;
		case 29:
		char_to_dec_32(K_D_s[1], 5, &val_32);																						//Корректировка дифференциального коэффициента ПИД-регулятора 2 контура
		eeprom_write_dword(&K_D[1], val_32);																						//Запись дифференциального коэффициента ПИД-регулятора 2 контура в EEPROM
		K_D_RAM[1] = val_32;
		break;
		//=========Контур 3=============//
		case 32:
		char_to_dec_16(temp_den_s[2], 2, &val_16);																					//Корректировка требуемой температуры для дневного режима 3 контура
		eeprom_write_word(&temp_den[2], val_16);																					//Запись требуемой температуры для дневного режима 3 контура в EEPROM
		temp_den_RAM[2] = val_16;
		break;
		case 33:
		char_to_dec_16(temp_noch_s[2], 2, &val_16);																					//Корректировка требуемой температуры для ночного режима 3 контура
		eeprom_write_word(&temp_noch[2], val_16);																					//Запись требуемой температуры для ночного режима 3 контура в EEPROM
		temp_noch_RAM[2] = val_16;
		break;
		case 34:
		char_to_dec_32(K_P_s[2], 5, &val_32);																						//Корректировка пропорционального коэффициента ПИД-регулятора 3 контура
		eeprom_write_dword(&K_P[2], val_32);																						//Запись пропорционального коэффициента ПИД-регулятора 3 контура в EEPROM
		K_P_RAM[2] = val_32;
		break;
		case 35:
		char_to_dec_32(K_I_s[2], 5, &val_32);																						//Корректировка интегрального коэффициента ПИД-регулятора 3 контура
		eeprom_write_dword(&K_I[2], val_32);																						//Запись интегрального коэффициента ПИД-регулятора 3 контура в EEPROM
		K_I_RAM[2] = val_32;
		break;
		case 36:
		char_to_dec_32(K_D_s[2], 5, &val_32);																						//Корректировка дифференциального коэффициента ПИД-регулятора 3 контура
		eeprom_write_dword(&K_D[2], val_32);																						//Запись дифференциального коэффициента ПИД-регулятора 3 контура в EEPROM
		K_D_RAM[2] = val_32;
		break;
	}
	//mems_to_RAM();
	//sei();
}

#endif
