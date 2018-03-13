/*
 * memory_ops.h
 *
 * Created: 13.03.2018
 * Author: Antipin
 * Операции с памятью (EEPROM<=>RAM)
 * Чтение и запись
 * Конвертация данных (INT<=>CHAR)
 */ 

#ifndef __memory_ops
#define __memory_ops


//Функция чтения данных (16) из EEPROM и записи в символные переменные//
//src - указатель на исходные данные, sz - размерность, tgt - указатель на выходную переменную
void inline mems_to_char_16(uint16_t* src, uint8_t sz, uint8_t* tgt)
{
	//Переменная для хранения числового значения исходной переменной для расчета
	uint16_t tmp_src;
	//Чтение параметров из EEPROM (кроме коэффициентов регулятора)
	tmp_src = eeprom_read_word(src);
	
	for (int i = 0; i<sz; i++, tgt++)
	{
		//Опеределение множителя 10 старшего разряда числа
		*tgt = (tmp_src/pow(10, sz-i-1)) + 0x30;
		//Вычитание старшего разряда во избежание наличия остатка от деления для следующей итерации
		tmp_src -= (*tgt-0x30)*pow(10, sz-i-1);
	}
}

//Функция чтения данных (32) из EEPROM и записи в символные переменные//
//src - указатель на исходные данные, sz - размерность, tgt - указатель на выходную переменную
void inline mems_to_char_32(uint32_t* src, uint8_t sz, uint8_t* tgt)
{
	//Переменная для хранения числового значения исходной переменной для расчета
	uint32_t tmp_src;
	//Чтение параметров регулятора из EEPROM
	tmp_src = eeprom_read_dword(src);
	
	for (int i = 0; i<sz; i++, tgt++)
	{
		//Опеределение множителя 10 старшего разряда числа
		*tgt = (tmp_src/pow(10, sz-i-1)) + 0x30;
		//Вычитание старшего разряда во избежание наличия остатка от деления для следующей итерации
		tmp_src -= (*tgt-0x30)*pow(10, sz-i-1);
	}
	//Флаг пакетов K_I (с точкой)
	if (sz == 5)
	{
		//Запись точки
		*(tgt-4) = '.';
	}
	//Флаг пакетов K_P и K_D (с точкой)
	else if (sz == 6)
	{
		*(tgt-7) = *(tgt-6);
		//Сдвиг данных вправо
		*(tgt-6) = *(tgt-5);
		*(tgt-5) = *(tgt-4);
		//Запись точки
		*(tgt-4) = '.';
	}
}

//Функция записи числовых переменных в эквивалентные символьные//
void convert_to_show()
{
	//====Время====//
	cur_time[0]=hour/10+0x30;
	cur_time[1]=hour%10+0x30;
	cur_time[2]=':';
	cur_time[3]=min/10+0x30;
	cur_time[4]=min%10+0x30;
	
	//====День недели====//
	for (int i = 0; i<7; i++)
	{
		disp_DOW[i][0]=i+1;
	}
	disp_DOW[cur_DOW_RAM][0] = 0;
	
	//======Настройки времени=====//
	
	per_dat_s[0] = per_dat_RAM/1000 + 0x30;
	per_dat_s[1] = per_dat_RAM%1000/100 + 0x30;
	per_dat_s[2] = per_dat_RAM%1000%100/10 + 0x30;
	per_dat_s[3] = per_dat_RAM%1000%100%10 + 0x30;
	
	mems_to_char_16(&per_shim, 4, per_shim_s);
	//mems_to_char_16(&per_dat, 4, per_dat_s);
	time_nach_s[2] = ':';
	mems_to_char_16(&time_nach[0], 2, &time_nach_s[0]);
	mems_to_char_16(&time_nach[1], 2, &time_nach_s[3]);
	time_kon_s[2] = ':';
	mems_to_char_16(&time_kon[0], 2, &time_kon_s[0]);
	mems_to_char_16(&time_kon[1], 2, &time_kon_s[3]);
	
	//====Настройки температуры====//
	
	mems_to_char_16(&temp_den[0], 2, temp_den_s[0]);
	mems_to_char_16(&temp_den[1], 2, temp_den_s[1]);
	mems_to_char_16(&temp_den[2], 2, temp_den_s[2]);
	mems_to_char_16(&temp_noch[0], 2, temp_noch_s[0]);
	mems_to_char_16(&temp_noch[1], 2, temp_noch_s[1]);
	mems_to_char_16(&temp_noch[2], 2, temp_noch_s[2]);
	
	//====Настройки регуляторов====//
	
	mems_to_char_32(&K_P[0], 6, K_P_s[0]);
	mems_to_char_32(&K_P[1], 6, K_P_s[1]);
	mems_to_char_32(&K_P[2], 6, K_P_s[2]);
	mems_to_char_32(&K_I[0], 5, K_I_s[0]);
	mems_to_char_32(&K_I[1], 5, K_I_s[1]);
	mems_to_char_32(&K_I[2], 5, K_I_s[2]);
	mems_to_char_32(&K_D[0], 6, K_D_s[0]);
	mems_to_char_32(&K_D[1], 6, K_D_s[1]);
	mems_to_char_32(&K_D[2], 6, K_D_s[2]);
}

//=====Функция конвертации символьных значений в числовые=====//

//src - указатель на исходные данные, sz - размерность, tgt - указатель на выходную переменную
void inline char_to_dec_16(uint8_t* src, uint8_t sz, uint16_t* tgt)
{
	if (sz==4) *tgt = (uint16_t) (*src-0x30)*1000+(*(src+1)-0x30)*100+(*(src+2)-0x30)*10+(*(src+3)-0x30);
	if (sz==2) *tgt = (uint16_t) (*src-0x30)*10+(*(src+1)-0x30);
}

//src - указатель на исходные данные, sz - размерность, tgt - указатель на выходную переменную
void inline char_to_dec_32(uint8_t* src, uint8_t sz, uint32_t* tgt)
{
	//Переменная для хранения указателя исходной переменной для сравнения
	uint8_t* src_tmp = src;
	//Индекс положения точки в числе
	uint8_t n = 0;
	//Поиск точки в числе
	for (int i = 0; i<sz; i++, src_tmp++)
	{
		if (*src_tmp=='.') n = i;
	}
	
	//При наличии точки в разряде (Nmax-1)
	if (n==1) *tgt = (uint32_t) (*src-0x30)*1000+(*(src+2)-0x30)*100+(*(src+3)-0x30)*10+(*(src+4)-0x30);
	//При наличии точки в разряде (Nmax-2)
	if (n==2) *tgt = (uint32_t) (*src-0x30)*10000+(*(src+1)-0x30)*1000+(*(src+3)-0x30)*100+(*(src+4)-0x30)*10+(*(src+5)-0x30);
}

//=====Функция чтения данных из EEPROM и их запись в RAM======//
void mems_to_RAM()
{
	time_nach_RAM[0] = (uint8_t)eeprom_read_word(&time_nach[0]);
	time_nach_RAM[1] = (uint8_t)eeprom_read_word(&time_nach[1]);
	time_kon_RAM[0] = (uint8_t)eeprom_read_word(&time_kon[0]);
	time_kon_RAM[1] = (uint8_t)eeprom_read_word(&time_kon[1]);
	per_dat_RAM = eeprom_read_word(&per_dat);
	per_shim_RAM = eeprom_read_word(&per_shim);
	for (int i = 0; i<7; i++)
	{
		hol_DOW_RAM[i] = eeprom_read_byte(&hol_DOW[i]);
	}
	for (int i = 0; i<3; i++)
	{
		temp_den_RAM[i] = eeprom_read_word(&temp_den[i]);
		temp_noch_RAM[i] = eeprom_read_word(&temp_noch[i]);
		K_P_RAM[i] = eeprom_read_dword(&K_P[i]);
		K_I_RAM[i] = eeprom_read_dword(&K_I[i]);
		K_D_RAM[i] = eeprom_read_dword(&K_D[i]);
	}
}

#endif
