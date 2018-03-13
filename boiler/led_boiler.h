/*
 * led_boiler.h
 *
 * Created: 13.03.2018
 * Author: Antipin
 * Работа с дисплеем
*/

#ifndef __led_boiler
#define __led_boiler

//======Функция отображения информации на главном экране======//
void main_screen()
{
	//=============Прокрутка символьных показаний датчиков=================//
	
	//Положение Т1 и Т2
	if (time_10_slide>0 && time_10_slide<=200 && flag_S == 3)
	{
		//Вывод вспомогательных симольных данных на экран
		LED_SetPos(0,0);
		LED_Write_String_P(PSTR("T1="));
		LED_SetPos(1,0);
		LED_Write_String_P(PSTR("T2="));
		//Вывод вспомогательных симольных данных на экран
		LED_SetPos(0,3);
		LED_Write_String(snc1);
		LED_SetPos(1,3);
		LED_Write_String(snc2);
		flag_S = 1;
	}
	
	//Положение Т2 и Т3
	else if (time_10_slide>200 && time_10_slide<=400 && flag_S == 1)
	{
		//Вывод вспомогательных симольных данных на экран
		LED_SetPos(0,0);
		LED_Write_String_P(PSTR("T2="));
		LED_SetPos(1,0);
		LED_Write_String_P(PSTR("T3="));
		//Вывод вспомогательных симольных данных на экран
		LED_SetPos(0,3);
		LED_Write_String(snc2);
		LED_SetPos(1,3);
		LED_Write_String(snc3);
		flag_S = 2;
	}
	
	//Положение Т3 и Т1
	else if (time_10_slide>400 && flag_S == 2)
	{
		//Вывод вспомогательных симольных данных на экран
		LED_SetPos(0,0);
		LED_Write_String_P(PSTR("T3="));
		LED_SetPos(1,0);
		LED_Write_String_P(PSTR("T1="));
		//Вывод вспомогательных симольных данных на экран
		LED_SetPos(0,3);
		LED_Write_String(snc3);
		LED_SetPos(1,3);
		LED_Write_String(snc1);
		flag_S = 3;
	}
	
	//Проверка флага наличия данных на экране во избежание мерцания экрана из-за переписывания
	if (flag_T==0)
	{
		LED_SetPos(0,9);
		LED_Write_String_P(PSTR("ST"));
		//Флаг наличия вышеобозначенных данных на экране
		flag_T=1;
	}
	if (flag_O==1)
	{
		LED_SetPos(0,11);
		//Вывод показаний датчика 2 на экран
		LED_Write_String(vis);
		LED_SetPos(1, 9);
		LED_Write_String(skv_s);
		flag_O = 0;
	}
}

#endif