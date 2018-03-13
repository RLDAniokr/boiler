#ifndef _RTC_																					//Защита от повторного включения файла
#define _RTC_

static uint8_t _I2C_Time_Out;																	//Счетчик тайм-аута
static char _I2C_buf[6];																		//Буфер для чтения/записи данных
static uint8_t _I2C_Error;																		//Флаг ошибки

//============= Функция перевода двоичного числа в упакованное двоично-десятичное ==============
unsigned char bin2bcd(unsigned char n)
{
	return (n/10<<4)+(n % 10);
}

//============ Функция перевода упакованного двоично-десятичного числа в двоичное ==============
unsigned char bcd2bin(unsigned char n)
{
	return (n>>4)*10+(n & 0x0F);
}

//======================== Функция генерации условия СТАРТ на шине I2C =========================
//Помимо генерации условия СТАРТ функция передает по I2C адрес подчиненного устройства (адрес должен содержать в себе бит чтения/записи)
//Функция принимает в качестве параметров:
//Adr - Адрес подчиненного устройства (младший бит адреса должен содержать бит чтения/записи (0-запись/1-чтение))

void _I2C_Start(uint8_t Adr)
{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWEA);												//Сброс бита готовности I2C, установка условия "СТАРТ", разрешение работы I2C
	
	_I2C_Time_Out=0;
	while(!(TWCR & (1<<TWINT)))																	//Ожидание завершения передачи
	{
		if (++_I2C_Time_Out>10) {_I2C_Error=1; break;}											//Выход если в течении 1 мс флаг не установился
		_delay_us(100);
	}
	
	TWDR=Adr;																					//Передача адреса и бита чтения/записи
	
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWEA);												//Передача данных
	_I2C_Time_Out=0;
	while(!(TWCR & (1<<TWINT)))																	//Ожидание завершения передачи
	{
		if (++_I2C_Time_Out>10) {_I2C_Error=1; break;}											//Выход если в течении 1 мс флаг не установился
		_delay_us(100);
	}
}

//======================== Функция генерации условия СТОП на шине I2C ==========================
void _I2C_Stop(void)
{
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);														//Остановка приема (генерация условия "СТОП")
}

//========================== Функция записи байта данных на шину I2C ===========================
//Функция принимает в качестве параметров:
//*buf - буфер записываемых данных
//Length - количество записываемых байт

void _I2C_Write(char *buf, uint8_t Length)
{
	uint8_t temp;																				//Вспомогательная переменная
	
	for (temp=0; temp<Length; temp++)
	{
		TWDR=*buf;																				//Запись данных в регистр
		buf++;
		TWCR=(1<<TWINT)|(1<<TWEN);																//Запуск передачи данных
		
		_I2C_Time_Out=0;
		while(!(TWCR & (1<<TWINT)))																//Ожидание завершения передачи
		{
			if (++_I2C_Time_Out>10) {_I2C_Error=1; break;}										//Выход если в течении 1 мс флаг не установился
			_delay_us(100);
		}
	}
	_I2C_Stop();																				//Остановка приема
	_delay_us(10);
}

//========================== Функция чтения байт данных с шины I2C =============================
//Функция принимает в качестве параметров:
//*buf - буфер для чтения
//Length - количество считываемых байт

void _I2C_Read(char *buf, uint8_t Length)
{
	uint8_t temp;																				//Вспомогательная переменная
	
	for (temp=0; temp<Length; temp++)
	{
		if(temp==Length-1) TWCR=(1<<TWINT)|(1<<TWEN);											//Запуск приема
		else TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
		
		_I2C_Time_Out=0;
		while(!(TWCR & (1<<TWINT)))																//Ожидание завершения приема
		{
			if (++_I2C_Time_Out>10) {_I2C_Error=1; break;}										//Выход если в течении 1 мс флаг не установился
			_delay_us(100);
		}
		*buf=TWDR;
		buf++;
		if(temp==Length-1) _I2C_Stop();															//Остановка приема
	}
	_delay_us(10);
}

/*
//=========== Функция чтения байта данных из микросхемы часов реального времени ============
//Функция принимает в качестве параметров:
//LastByte - признак последнего байта в посылке
//Функция возвращает:
//значение прочитанного байта данных

char _RTC_Read(char LastByte)
{
	if(LastByte) TWCR=(1<<TWINT)|(1<<TWEA)|(1<<TWEN);											//Запуск приема
	else TWCR=(1<<TWINT)|(1<<TWEN);												
	uint8_t temp=0;
	while(!(TWCR & (1<<TWINT)))																	//Ожидание приема
	{
		if (++temp>10) {_I2C_Error=1; break;}													//Выход если в течении 1 мс флаг не установился
		_delay_us(100);
	}
	return TWDR;																				//Возврат результата
}
*/

//================= Функция инициализации микросхемы часов реального времени ===================
char RTC_Init(void)
{
	TWSR=0;																						//Настройка предделителя скорости связи
	TWBR=77;																					//Настройка регистра скорости связи
	TWAR=0x01;																					//Адрес микросхемы часов реального времени
	
	_I2C_Error=0;																				//Обнулить признак ошибки
	_I2C_Start(0xD0);

	_I2C_buf[0]=0x07; _I2C_buf[1]=0x91;
	_I2C_Write(_I2C_buf,2);

	if (_I2C_Error) return 1;																	//Если была ошибка
	else return 0;
}

//============================== Функция чтения текущего времени ===============================
char RTC_Get_Time(unsigned char* hour, unsigned char* min, unsigned char* sec)
{
	_I2C_Error=0;																				//Обнулить признак ошибки
	_I2C_Start(0xD0);																			//Конфигурирование микросхемы на чтение времени
	
	_I2C_buf[0]=0x00;
	_I2C_Write(_I2C_buf,1);
	
	_I2C_Start(0xD1);
	
	_I2C_Read(_I2C_buf,3);																		//Чтение секунд, минут, часов
	*sec=bcd2bin(_I2C_buf[0]);																	//Секунды
	*min=bcd2bin(_I2C_buf[1]);																	//Минуты
	*hour=bcd2bin(_I2C_buf[2]);																	//Часы

	if (_I2C_Error) return 1;																	//Если была ошибка
	else return 0;
}

//================================= Функция установки времени ==================================
char RTC_Set_Time(unsigned char hour, unsigned char min, unsigned char sec)
{
	_I2C_Error=0;																				//Обнулить признак ошибки
	_I2C_Start(0xD0);																			//Конфигурирование микросхемы на запись времени
	
	_I2C_buf[0]=0x00;
	_I2C_buf[1]=bin2bcd(sec);
	_I2C_buf[2]=bin2bcd(min);
	_I2C_buf[3]=bin2bcd(hour);
	_I2C_Write(_I2C_buf,4);
	
	if (_I2C_Error) return 1;																	//Если была ошибка
	else return 0;
}

//============================== Функция чтения текущей даты ===============================
char RTC_Get_Date(unsigned char* day, unsigned char* date, unsigned char* month, unsigned char* year)
{
	_I2C_Error=0;																				//Обнулить признак ошибки
	_I2C_Start(0xD0);																			//Конфигурирование микросхемы на чтение даты
	
	_I2C_buf[0]=0x03;
	_I2C_Write(_I2C_buf,1);
	
	_I2C_Start(0xD1);
	
	_I2C_Read(_I2C_buf,4);																		//Чтение дня недели, даты, месяца, года
	*day=bcd2bin(_I2C_buf[0])-1;																	//День недели
	*date=bcd2bin(_I2C_buf[1]);																	//Число
	*month=bcd2bin(_I2C_buf[2]);																//Месяц
	*year=bcd2bin(_I2C_buf[3]);																	//Год
	
	if (_I2C_Error) return 1;																	//Если была ошибка
	else return 0;
}

//================================ Функция установки даты ==================================
char RTC_Set_Date(unsigned char day, unsigned char date, unsigned char month, unsigned char year)
{
	_I2C_Error=0;																				//Обнулить признак ошибки
	_I2C_Start(0xD0);																			//Конфигурирование микросхемы на запись даты

	_I2C_buf[0]=0x03;
	_I2C_buf[1]=bin2bcd(day)+1;
	_I2C_buf[2]=bin2bcd(date);
	_I2C_buf[3]=bin2bcd(month);
	_I2C_buf[4]=bin2bcd(year);
	_I2C_Write(_I2C_buf,5);	
	
	if (_I2C_Error) return 1;																	//Если была ошибка
	else return 0;
}

#endif