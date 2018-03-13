/*
 * rtc_boiler.h
 *
 * Created: 13.03.2018
 * Author: Antipin
 * Работа с часами реального времени
 */ 

#ifndef __rtc_boiler
#define __rtc_boiler

//=============Функция проверки временных условий=============//
void check_time()
{
	//Статус дня "Рабочий"
	if (hol_DOW_RAM[cur_DOW_RAM] == '0')
	{
		//Текущий час находится между часом начла и конца рабочего дня
		if (hour>time_nach_RAM[0] && hour<time_kon_RAM[0])
		{
			flag_N = 0;
			if (skv_s[4] == 'N')
			{
				skv_s[4] = 'D';
				flag_O = 1;
			}
		}
		//Текущий час не находится между часом начла и конца рабочего дня
		else
		{
			flag_N = 1;
			if (skv_s[4] == 'D')
			{
				skv_s[4] = 'N';
				flag_O = 1;
			}
		}
		//Текущий час равен часу начала рабочего дня
		if (hour==time_nach_RAM[0])
		{
			//Текущая минута больше минуты начала рабочего дня
			if (min>=time_nach_RAM[1])
			{
				flag_N = 0;
				if (skv_s[4] == 'N')
				{
					skv_s[4] = 'D';
					flag_O = 1;
				}
			}
			//Текущая минута больше минуты начала рабочего дня
			else
			{
				flag_N = 1;
				if (skv_s[4] == 'D')
				{
					skv_s[4] = 'N';
					flag_O = 1;
				}
			}
		}
		//Текущий час равен часу конца рабочего дня
		if (hour == time_kon_RAM[0])
		{
			//Текущая минута больше минуты конца рабочего дня
			if (min>=time_kon_RAM[1])
			{
				flag_N = 1;
				if (skv_s[4] == 'D')
				{
					skv_s[4] = 'N';
					flag_O = 1;
				}
			}
			//Текущая минута больше минуты конца рабочего дня
			else
			{
				flag_N = 0;
				if (skv_s[4] == 'N')
				{
					skv_s[4] = 'D';
					flag_O = 1;
				}
			}
		}
	}
	//Статус текущего дня "Выходной/праздник"
	else if (hol_DOW_RAM[cur_DOW_RAM] == '1')
	{
		flag_N = 1;
		if (skv_s[4] == 'D')
		{
			skv_s[4] = 'N';
			flag_O = 1;
		}
	}
	
	if(hour==23 && min==59 && sec==59)
	{
		for (int i = 0; i<3; i++) I_prev[i] = 0;
	}
}

void get_time()
{
	if (RTC_Get_Time(&flush, &flush, &flush)==0)
	{
		RTC_Get_Time(&hour, &min, &sec);																					//Вызов функции чтения текущего времени с RTC
		RTC_Get_Date(&cur_DOW_RAM, &flush, &flush, &flush);
		clock_err = 0;
	}
	else clock_err++;
	if (clock_err>10)
	{
		RTC_Init();
		flag_R=1;																											//Установка режима "ручное управление"
		skv_s[0] = 'R';
		flag_O = 1;
	}
	
}

#endif