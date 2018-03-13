/*
 * buttons_boiler.h
 *
 * Created: 13.03.2018
 * Author: Antipin
 * Работа с тактовыми кнопками
*/

#ifndef __buttons_boiler
#define __buttons_boiler

void check_buttons()
{
	//Обработка события нажатия на кнопку "Меню"
	if (Key(0) == 1) menu_in_loop();
			
	while (Key(3) == 1)																											//При зажатии кнопки 4
	{
		cli();
		t_push++;
		if (t_push>=3000)																										//После 3 секунд
		{
			if (flag_R==0)																										//Если текущий режим управления автоматический
			{
				//Led(1);
				flag_R=1;																										//Установка режима "ручное управление"
				skv_s[0] = 'R';
				flag_O = 1;
				break;
			}
			else if (flag_R==1)																									//Если текущий режим управления ручной
			{
				//Led(1);
				flag_R=0;																										//Установка режима "автоматическое управление"
				skv_s[0] = 'A';
				flag_O = 1;
				break;
			}
		}
		_delay_ms(1);
	}
	sei();
	t_push = 0;
			
	if (Key(2)==1)
	{
		//Антидребезг
		while(Key(2)==1) {_delay_ms(10);}
		
		//Процент времени управления в ручном режиме инкрементируется на 5
		if (skv<100) skv+=5;
		skv_s[1]=skv/100+0x30;
		skv_s[2]=skv%100/10+0x30;
		skv_s[3]=skv%100%10+0x30;
		flag_O = 1;
	}
			
	if (Key(1)==1)
	{
		//Антидребезг
		while(Key(1)==1) {_delay_ms(10);}
		
		//Процент времени управления в ручном режиме декрементируется на 5
		if (skv>0) skv-=5;
		skv_s[1]=skv/100+0x30;
		skv_s[2]=skv%100/10+0x30;
		skv_s[3]=skv%100%10+0x30;
		flag_O = 1;
	}
	
}

#endif