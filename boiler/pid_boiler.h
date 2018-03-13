/*
 * pid_boiler.h
 *
 * Created: 13.03.2018
 * Author: Antipin
 * ������ ���������� ���������� ���-����������
*/

#ifndef __pid_boiler
#define __pid_boiler

//============������� ���������� ���-����������===============//
void PID(uint32_t* time_on, uint8_t i)
{
	//��� ������� �������� �������
	uint8_t cur_steam = 0;
	switch (i)
	{
		// 1 ������
		case 0: cur_steam = PB4; break;
		// 2 ������
		case 1: cur_steam = PB5; break;
		// 3 ������
		case 2: cur_steam = PB7; break;
	}

	//������� ����� ������ ������� �������� �������
	if (time_10_shim<*time_on)
	{
		//������� ����������� ������
		PORTB |= (1<<cur_steam);
		//������� ������ ����������� �������
		PORTB &= ~(1 << PB6);
		if (vis[3] == '1')
		{
			vis[3] = '0';
			flag_O = 1;
		}
		//������ ������ �� �������� ������� � ������ ������� ��������
		clsd_stms[i] = 0;
		clsd_stms[3] = 1;
		if (vis[i] == '0')
		{
			vis[i] = '1';
			flag_O = 1;
		}
	}
	
	//������� ����� ������ ������� �������� �������
	else if (time_10_shim>*time_on && clsd_stms[i]==0)
	{
		//���� ������� ������������ ������
		flag_P = 1;
		//����� 10 ��� ����� �������� ������
		if (time_10_nas>=1000)
		{
			//������� ������
			PORTB &= ~(1<<cur_steam);
			//������ ������� � �������� ������� � ������ ������� ��������
			clsd_stms[i] = 1;
			if (vis[i] == '1')
			{
				vis[i] = '0';
				flag_O = 1;
			}
		}
	}
	
	//���� ��� ������� ������� �������
	if (clsd_stms[0]==1 && clsd_stms[1]==1 && clsd_stms[2]==1)
	{
		//������� ���������� (�����) ������
		PORTB |= (1<<PB6);
		if (vis[3] == '0')
		{
			vis[3] = '1';
			flag_O = 1;
		}
	}
	if (flag_P == 0)
	{
		//���� ��� ������� ������� �������
		if (clsd_stms[0]==1 && clsd_stms[1]==1 && clsd_stms[2]==1)
		{
			//���������� ������ ���� ��� ������� ������� �������
			PORTD &= ~(1<<PD4);
			if (vis[4] == '1')
			{
				vis[4]='0';
				flag_O = 1;
			}
		}
		else
		{
			//��������� ������ ���� ��� ����� ������������
			PORTD |= (1<<PD4);
			if (vis[4] == '0')
			{
				vis[4]='1';
				flag_O = 1;
			}
		}
	}
}

//=====================�������� ���===========================//
void calculate_PID(void)
{
	//������� ������
	int16_t E;
	//������������ ���-����������
	int32_t P, I, D;
	//���������� ��������� ���-����������
	int32_t Y;
	//����� ���-����������
	uint32_t Ton;
	
	for (int i = 0; i<3; i++)
	{
		//���������� ������� ������ (����)
		if (flag_N==0) E = (int16_t)temp_den_RAM[i]*1000 - cur_temp[i];
		//���������� ������� ������ (����)
		else E = (int16_t)temp_noch_RAM[i]*1000 - cur_temp[i];
		
		//���������� ���������������� ������������ ���-����������
		P = (int32_t) K_P_RAM[i]*E;
		//���������� ������������ ������������ ���-����������
		I = (int32_t) I_prev[i] + K_I_RAM[i]*E;
		//���������� ���������������� ������������ ���-����������
		D = (int32_t) K_D_RAM[i]*(E-E_prev[i]);
		
		//������ ������� ������ ��� ���������� ��� ����������� ����������
		E_prev[i] = E;
		//������ �������� �������� ������������ ������������ ��� ����������
		I_prev[i] = I;
		
		//���������� �������� ���-���������� ��� ������� ����������� ������� � ������� ����� ������� ����������
		if (flag_R==0 && snc_err[i]<10)
		{
			//���������� ����������� ��������� ���-���������� � ������� 100%=100000
			Y = (int32_t)((P+I+D)/1000);
			//������ �� ������������
			if (Y>100000) Y = 100000;
			//������ �� ������������� ��������
			if (Y<0) Y = 0;
			T_u[i] = (uint8_t) ((Y/100000)*100 + ((Y%100000)/10000)*10 + (Y%100000%10000)/1000);
			//���������� ������� ���-���������� � �*100
			Ton = (uint32_t) per_shim_RAM*Y/1000;
			
			//����� ������� ���������� ���-����������
			PID(&Ton, i);
		}
		
		//��������� ���������� ������� ������ ���������� � �������� �������� ���-���������� ��� ������� ������������� ������� ��� ��������� ����� ������� ���������� �1
		else if (flag_R==1 || snc_err[i]>=10)
		{
			Y =(int32_t) skv*1000;
			T_u[i] = (uint8_t) ((Y/100000)*100 + ((Y%100000)/10000)*10 + (Y%100000%10000)/1000);
			//���������� ������� ���-���������� � �*100
			Ton = per_shim_RAM*Y/1000;
			//����� ������� ���������� ���-����������
			PID(&Ton, i);
		}
	}
}

#endif