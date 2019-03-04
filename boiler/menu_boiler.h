/*
 * menu_boiler.h
 *
 * Created: 13.03.2018
 * Author: Antipin
 * �������� ���� � ���������� ������ ��� ���������
 */ 

#ifndef __menu_boiler
#define __menu_boiler

//============������� �������� ��������� ����=================//
void inline create_menu()
{
	Add_New_Menu(0, 2);
	Add_New_Element(0, EMPTY_ELEMENT, 1, PSTR("���������"), NONE, NONE);
	Add_New_Element(1, PARAM_ELEMENT, NONE, PSTR("������:"), Password_s, 4);
	
	Add_New_Menu(1, 6);
	
	Add_New_Element(2, EMPTY_ELEMENT, 8, PSTR("������ �1"), NONE, NONE);
	Add_New_Element(3, EMPTY_ELEMENT, 11, PSTR("������ �2"), NONE, NONE);
	Add_New_Element(4, EMPTY_ELEMENT, 14, PSTR("������ �3"), NONE, NONE);
	Add_New_Element(5, EMPTY_ELEMENT, 2, PSTR("�����. ��������/n �������"), NONE, NONE);
	Add_New_Element(6, EMPTY_ELEMENT, 6, PSTR("������ ���"), NONE, NONE);
	Add_New_Element(7, EMPTY_ELEMENT, 7, PSTR("������ ������/n ��������"), NONE, NONE);
	
	//=================================�����===============================//
	
	Add_New_Menu(2, 3);
	
	Add_New_Element(8, EMPTY_ELEMENT, 3, PSTR("��������� �����./n �������"), NONE, NONE);
	Add_New_Element(9, EMPTY_ELEMENT, 4, PSTR("������ ����/����"), NONE, NONE);
	Add_New_Element(10, EMPTY_ELEMENT, 5, PSTR("������ ����/n ������"), NONE, NONE);
	
	Add_New_Menu (3, 2);
	
	Add_New_Element(11, PARAM_ELEMENT, NONE, PSTR("�����"), cur_time, 5);
	Add_New_Element(12, LIST_ELEMENT, NONE, PSTR("���� ������"), disp_DOW[0], 3);
	
	Add_New_Menu(4, 2);
	
	Add_New_Element(13, PARAM_ELEMENT, NONE, PSTR("������ ���. ���"), time_nach_s, 5);
	Add_New_Element(14, PARAM_ELEMENT, NONE, PSTR("����� ���. ���"), time_kon_s, 5);
	
	Add_New_Menu(5, 1);
	Add_New_Element(15, WORD_ELEMENT, NONE, PSTR("� � � � � � �"), hol_DOW_RAM, 7);
	
	Add_New_Menu (6, 1);
	Add_New_Element(16, PARAM_ELEMENT, NONE, PSTR("������ � ���"), per_shim_s, 4);
	
	Add_New_Menu(7, 1);
	Add_New_Element(17, PARAM_ELEMENT, NONE, PSTR("������ �  ���"), per_dat_s, 4);
	
	
	//====================1 ������=====================================//
	
	Add_New_Menu (8, 2);
	Add_New_Element(18, EMPTY_ELEMENT, 9, PSTR("�����. ��������/n �����������"), NONE, NONE);
	Add_New_Element(19, EMPTY_ELEMENT, 10, PSTR("�����. ��������/n ����������"), NONE, NONE);
	
	Add_New_Menu(9, 2);
	Add_New_Element(20, PARAM_ELEMENT, NONE, PSTR("����������� ����"), temp_den_s[0], 2);
	Add_New_Element(21, PARAM_ELEMENT, NONE, PSTR("����������� ����"), temp_noch_s[0], 2);
	
	Add_New_Menu(10, 3);
	Add_New_Element(22, PARAM_ELEMENT, NONE, PSTR("��"), K_P_s[0], 6);
	Add_New_Element(23, PARAM_ELEMENT, NONE, PSTR("��"), K_I_s[0], 5);
	Add_New_Element(24, PARAM_ELEMENT, NONE, PSTR("��"), K_D_s[0], 6);

	//====================2 ������=====================================//
	
	Add_New_Menu (11, 2);
	Add_New_Element(25, EMPTY_ELEMENT, 12, PSTR("�����. ��������/n �����������"), NONE, NONE);
	Add_New_Element(26, EMPTY_ELEMENT, 13, PSTR("�����. ��������/n ����������"), NONE, NONE);
	
	Add_New_Menu(12, 2);
	Add_New_Element(27, PARAM_ELEMENT, NONE, PSTR("����������� ����"), temp_den_s[1], 2);
	Add_New_Element(28, PARAM_ELEMENT, NONE, PSTR("����������� ����"), temp_noch_s[1], 2);
	
	Add_New_Menu(13, 3);
	Add_New_Element(29, PARAM_ELEMENT, NONE, PSTR("��"), K_P_s[1], 6);
	Add_New_Element(30, PARAM_ELEMENT, NONE, PSTR("��"), K_I_s[1], 5);
	Add_New_Element(31, PARAM_ELEMENT, NONE, PSTR("��"), K_D_s[1], 6);
	
	//====================3 ������=====================================//
	
	Add_New_Menu (14, 2);
	Add_New_Element(32, EMPTY_ELEMENT, 15, PSTR("�����. ��������/n �����������"), NONE, NONE);
	Add_New_Element(33, EMPTY_ELEMENT, 16, PSTR("�����. ��������/n ����������"), NONE, NONE);
	
	Add_New_Menu(15, 2);
	Add_New_Element(34, PARAM_ELEMENT, NONE, PSTR("����������� ����"), temp_den_s[2], 2);
	Add_New_Element(35, PARAM_ELEMENT, NONE, PSTR("����������� ����"), temp_noch_s[2], 2);
	
	Add_New_Menu(16, 3);
	Add_New_Element(36, PARAM_ELEMENT, NONE, PSTR("��"), K_P_s[2], 6);
	Add_New_Element(37, PARAM_ELEMENT, NONE, PSTR("��"), K_I_s[2], 5);
	Add_New_Element(38, PARAM_ELEMENT, NONE, PSTR("��"), K_D_s[2], 6);
	
	//=================================================================//

}


//===������� ���������� ���������� ��� �� ��������� �� ����===//
//el - ����� ��������, � ������� ���� �������� ���������
void static settings_changed(uint8_t el)
{
	//���������� ��� �������� ������������� �������� � ���������� ������� � EEPROM
	uint16_t val_16 = 0;
	uint32_t val_32 = 0;
	//cli();
	switch (el)
	{
		//������������� �������� �������
		case 11 :
			hour = (cur_time[0]-0x30)*10 + (cur_time[1]-0x30);
			min = (cur_time[3]-0x30)*10 + (cur_time[4]-0x30);
			RTC_Set_Time(hour, min, sec);
		break;
		
		//������������� �������� ��� ������
		case 12 :
			//����� �������� ������ � �������� "����"
			for (int i = 0; i<7; i++)
			{
				if (disp_DOW[i][0] == 0)
				{
					cur_DOW_RAM = i;
					//������ ����������� ������ �������� ������ � EEPROM
					RTC_Set_Date(cur_DOW_RAM, flush, flush, flush);
				}
			}
			convert_to_show();
		break;
		
		//������������� ������� ������ �������� ���
		case 13:
			//������������� �����
			char_to_dec_16(&time_nach_s[0], 2, &val_16);
			//������ ����� � EEPROM
			eeprom_write_word(&time_nach[0], val_16);
			time_nach_RAM[0] = val_16;
			//������������� �����
			char_to_dec_16(&time_nach_s[3], 2, &val_16);
			time_nach_RAM[1] = val_16;
			//������ ����� � EEPROM
			eeprom_write_word(&time_nach[1], val_16);
		break;
		
		//������������� ������� ����� �������� ���
		case 14:
			//������������� �����
			char_to_dec_16(&time_kon_s[0], 2, &val_16);
			//������ ����� � EEPROM
			eeprom_write_word(&time_kon[0], val_16);
			time_kon_RAM[0] = val_16;
			//������������� �����
			char_to_dec_16(&time_kon_s[3], 2, &val_16);
			//������ ����� � EEPROM
			eeprom_write_word(&time_kon[1], val_16);
			time_kon_RAM[1] = val_16;
		break;
		
		//������������� �������� ��� ������ � ������
		case 15:
			for (int i = 0; i<7; i++)
			{
				eeprom_write_byte(&hol_DOW[i], hol_DOW_RAM[i]);
			}
		break;

		//������������� ������� ���
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
				//������ ������� ��� � EEPROM
				eeprom_write_word(&per_shim, val_16);
				per_shim_RAM = val_16;
				time_10_shim = 0;
				break;
			}
		case 17:
			//������������� ������� ������ ��������
			char_to_dec_16(per_dat_s, 4, &val_16);
			//������ ������� ������ �������� � EEPROM
			eeprom_write_word(&per_dat, val_16);
			per_dat_RAM = val_16;
			time_10_dat = 0;
		break;
		
		//=========������ 1=============//
		case 20:
			//������������� ��������� ����������� ��� �������� ������ 1 �������
			char_to_dec_16(temp_den_s[0], 2, &val_16);
			//������ ��������� ����������� ��� �������� ������ 1 ������� � EEPROM
			eeprom_write_word(&temp_den[0], val_16);
			temp_den_RAM[0] = val_16;
		break;
		case 21:
			//������������� ��������� ����������� ��� ������� ������ 1 �������
			char_to_dec_16(temp_noch_s[0], 2, &val_16);
			//������ ��������� ����������� ��� ������� ������ 1 ������� � EEPROM
			eeprom_write_word(&temp_noch[0], val_16);
			temp_noch_RAM[0] = val_16;
		break;
		case 22:
			//������������� ����������������� ������������ ���-���������� 1 �������
			char_to_dec_32(K_P_s[0], 5, &val_32);
			//������ ����������������� ������������ ���-���������� 1 ������� � EEPROM
			eeprom_write_dword(&K_P[0], val_32);
			K_P_RAM[0] = val_32;
		break;
		case 23:
			//������������� ������������� ������������ ���-���������� 1 �������
			char_to_dec_32(K_I_s[0], 5, &val_32);
			//������ ������������� ������������ ���-���������� 1 ������� � EEPROM
			eeprom_write_dword(&K_I[0], val_32);
			K_I_RAM[0] = val_32;
		break;
		case 24:
			//������������� ����������������� ������������ ���-���������� 1 �������
			char_to_dec_32(K_D_s[0], 5, &val_32);
			//������ ����������������� ������������ ���-���������� 1 ������� � EEPROM
			eeprom_write_dword(&K_D[0], val_32);
			K_D_RAM[0] = val_32;
		break;
		
		//=========������ 2=============//
		case 27:
			//������������� ��������� ����������� ��� �������� ������ 2 �������
			char_to_dec_16(temp_den_s[1], 2, &val_16);
			//������ ��������� ����������� ��� �������� ������ 2 ������� � EEPROM
			eeprom_write_word(&temp_den[1], val_16);
			temp_den_RAM[1] = val_16;
		break;
		case 28:
			//������������� ��������� ����������� ��� ������� ������ 2 �������
			char_to_dec_16(temp_noch_s[1], 2, &val_16);
			//������ ��������� ����������� ��� ������� ������ 2 ������� � EEPROM
			eeprom_write_word(&temp_noch[1], val_16);
			temp_noch_RAM[1] = val_16;
		break;
		case 29:
			//������������� ����������������� ������������ ���-���������� 2 �������
			char_to_dec_32(K_P_s[1], 5, &val_32);
			//������ ����������������� ������������ ���-���������� 2 ������� � EEPROM
			eeprom_write_dword(&K_P[1], val_32);
			K_P_RAM[1] = val_32;
		break;
		case 30:
			//������������� ������������� ������������ ���-���������� 2 �������
			char_to_dec_32(K_I_s[1], 5, &val_32);
			//������ ������������� ������������ ���-���������� 2 ������� � EEPROM
			eeprom_write_dword(&K_I[1], val_32);
			K_I_RAM[1] = val_32;
		break;
		case 31:
			//������������� ����������������� ������������ ���-���������� 2 �������
			char_to_dec_32(K_D_s[1], 5, &val_32);
			//������ ����������������� ������������ ���-���������� 2 ������� � EEPROM
			eeprom_write_dword(&K_D[1], val_32);
			K_D_RAM[1] = val_32;
		break;
		
		//=========������ 3=============//
		case 34:
			//������������� ��������� ����������� ��� �������� ������ 3 �������
			char_to_dec_16(temp_den_s[2], 2, &val_16);
			//������ ��������� ����������� ��� �������� ������ 3 ������� � EEPROM
			eeprom_write_word(&temp_den[2], val_16);
			temp_den_RAM[2] = val_16;
		break;
		case 35:
			//������������� ��������� ����������� ��� ������� ������ 3 �������
			char_to_dec_16(temp_noch_s[2], 2, &val_16);
			//������ ��������� ����������� ��� ������� ������ 3 ������� � EEPROM
			eeprom_write_word(&temp_noch[2], val_16);
			temp_noch_RAM[2] = val_16;
		break;
		case 36:
			//������������� ����������������� ������������ ���-���������� 3 �������
			char_to_dec_32(K_P_s[2], 5, &val_32);
			//������ ����������������� ������������ ���-���������� 3 ������� � EEPROM
			eeprom_write_dword(&K_P[2], val_32);
			K_P_RAM[2] = val_32;
		break;
		case 37:
			//������������� ������������� ������������ ���-���������� 3 �������
			char_to_dec_32(K_I_s[2], 5, &val_32);
			//������ ������������� ������������ ���-���������� 3 ������� � EEPROM
			eeprom_write_dword(&K_I[2], val_32);
			K_I_RAM[2] = val_32;
		break;
		case 38:
			//������������� ����������������� ������������ ���-���������� 3 �������
			char_to_dec_32(K_D_s[2], 5, &val_32);
			//������ ����������������� ������������ ���-���������� 3 ������� � EEPROM
			eeprom_write_dword(&K_D[2], val_32);
			K_D_RAM[2] = val_32;
		break;
	}
}

void menu_in_loop()
{
	//����� �������� ����
	static uint8_t buf[8];
	//��� ��������
	static uint8_t type = 0;
	//����� ��������
	static uint16_t element = 0;

	//"�����������"
	while(Key(0)==1) {_delay_ms(10);}
	//���� ����� � ���� ��
	flag_M = 1;
					
	while (1)
	{
		//����������� ������� ������ �� EEPROM � ���������� ��� �����������
		convert_to_show();
		//����������� �������� ��������
		element = Goto_Menu(buf, &type);
		if (element!=NONE)
		{
			//����� � ������������ ����
			if (element == EXIT) break;
			//����� ������� ���������� ���������� ����������
			Save_Changes(element, buf);
			//����� ������� ���������� ����������� ������ ��� ������ � EEPROM
			settings_changed(element);
		}
	}
	// ���� ������ �� ����
	flag_M = 0;
	// ���� ����������� �� ������
	flag_O = 1;
	//��������� ����� ����������� ���������� �� ������� ������
	flag_T = 0;

}
#endif
