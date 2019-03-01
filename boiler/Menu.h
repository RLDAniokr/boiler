#ifndef _MENU_																														//������ �� ���������� ��������� �����
#define _MENU_ 1

#ifndef F_CPU
# warning "F_CPU not defined. Frequency by default 14,7456MHz"
#define F_CPU 14745600UL																											//����������� ���������, �������� ������� ���������������� (���������� ��� ���������� ������ ������� ��������)
#endif

#ifndef MAX_ELEMENTS_MENU
#warning "MAX_ELEMENTS not defined. Elements count by default 50"
#define MAX_ELEMENTS_MENU 50																										//���������, �������� ���������� ��������� � ����
#endif

#ifndef MAX_SUB_MENUS
#warning "MAX_SUB_MENUS not defined. Elements count by default 10"
#define MAX_SUB_MENUS 10																											//���������, �������� ���������� �������
#endif

#ifndef MAX_HISTORY_ELEMENTS
#warning "MAX_HISTORY_ELEMENTS not defined"
#define MAX_HISTORY_ELEMENTS 16																										//���������, �������� ����� ������ ��������� ���������
#endif

#define _MAX_PARAM_LENGHT 8																											//���������, �������� ������������ ����� ���������� ���������

#define NONE 0																														//��������� "��� ������"

#define OUT 0																														//��������� "����� �� ����"
#define IN 1																														//��������� "���� � ����"
#define EXIT 65535																													//��������� "����� �� ����"

#include <avr/iom128.h>																												//����������� ���������� �����/������
#include <util/delay.h>																												//����������� ���������� ��������
#include <stdio.h>
#include "Key.h"																													//���������� ��� ������ � ��������
																								
//-------------------------------------- �������� ������������ ���� ��������� ���� -------------------------------------------------//
typedef enum																														//�������� ������ ����� ��������� ����
{
	EMPTY_ELEMENT = 0,																												//������ �������
	PARAM_ELEMENT,																													//������� � �������� ����������
	SWITCH_ELEMENT,																													//�������-������������� (���/����)
	DEFAULT_ELEMENT,																												//�������, ������������ ��������� �� ���������
	HISTORY_ELEMENT,																												//�������, ������������ ������� ���������
	WORD_ELEMENT,																													//������� � ��������� ����������
	BLANK_ELEMENT,																													//��� ��������, ������������ ������ ����� ��������.
	LIST_ELEMENT,																													//
	PASSWORD_ELEMENT,
} menu_Types;

//------------------------------------------- �������� ������������ ������ ---------------------------------------------------------//
enum _Lib_Flags																														//�������� ������ ������
{
	lib_Flag_Error = (1),																											//���� ������
	lib_Flag_Set_Changes = (1 << 1),																								//���� �������� ��������� �� ������
	lib_Flag_Menu_Enable = (1 << 2),																								//���� ��������� ����
	lib_Flag_Changing_Param = (1 << 3),																								//���� ��������� ����������
	lib_Flag_Save_Change = (1 << 4),																								//���� ���������� ���������
	lib_Flag_FIFO_End = (1 << 5),																									//���� ���������� ������ ��������� ���������
	lib_Flag_Menu_Out = (1 << 6),
} _my_Lib_Flags = 0;																												//�������� ���������� � ������� ������
					
//----------------------------------------- ���������� ���������� ���������� -------------------------------------------------------//
uint8_t _temp_Array[8];																												//����� ��� ����������� �������� ���������
uint8_t _temp_Switch = 0;																											//����������-����� ��� ����������� �������� ���������
uint16_t _selected_Element = 0;																										//����� �������� �������� ����
uint16_t _number_In_Submenu = 0;

uint8_t _history_Buf_Param[MAX_HISTORY_ELEMENTS][_MAX_PARAM_LENGHT];																//������ ���������� ��������� ������� ��������� ���������
uint16_t _history_Buf_Number[MAX_HISTORY_ELEMENTS];

uint8_t _line2 = 0;																													//��������������� ���������� ��� ����������� ��������� �������
uint8_t _line = 0;																													//��������������� ���������� ��� ���������� ��������� �������

int8_t _temp_List = 0;
//---------------------------------------- �������� ��������� ��������� ���� -------------------------------------------------------//
typedef struct																											
{
	uint8_t element_Type;																											//��� ��������
	uint8_t child_Submenu;																											//����� ��������� �������
	__flash const char *text;																										//��������� �� ������ ����������� �������� ��������
	uint8_t *data;																													//��������� �� ������ ������� ������
	uint8_t array_Size;
	uint8_t number_In_Submenu;
} Menu_Elements;																													//������ ��������� �������, ���������� � ���� ��� ������������� ���� �������� ��������� 

Menu_Elements menu_Elements[MAX_ELEMENTS_MENU+1];
//------------------------------------------- �������� ��������� ������� -----------------------------------------------------------//
struct submenu
{
	uint8_t elements_Count_In_Submenu;																								//���������� ��������� � ������� �������
	uint8_t parent_Element_Number;																									//����� ������������� ��������
}submenu[MAX_SUB_MENUS+1];																											//������ �������, ���������� � ���� ��� ������������� ���� �������� ��������� 

//==================================== ������� �������� ������ �������� ���� =======================================================//
//������� ��������� ������ ��������� menu_Elements. ������ ������� ������� �������� � ���� ��� ��������� ���������.
//������� ��������� ���������:
//           number_Element - �� ������������ � �����������, ��������� ��� ����������� �������� ������������� ������, ��� �������� �������� ���������� ��������� ����.
//             element_Type - ��� ��������.
//            child_Submenu - ����� ��������� �������.
//                    *text - ��������� �� ������ ����������� �������� ��������.
//                    *data - ��������� �� ������ ������� ������.
//				 array_Size - ���������� �������� � ���������.
void Add_New_Element(const uint8_t number_Element,  menu_Types element_Type,  uint8_t child_Submenu, __flash const char *text, uint8_t *data, uint8_t array_Size)	
{
	static uint16_t count_Element = 0;																								//������� ���������� ��������� ����
	if (count_Element > MAX_ELEMENTS_MENU) return;																					//�������� �� ������������
	menu_Elements[count_Element].element_Type = element_Type;
	menu_Elements[count_Element].text = text;
	menu_Elements[count_Element].data = data;
	
	menu_Elements[count_Element].child_Submenu = child_Submenu;																		//���������� ���������� �������� ����
	if (child_Submenu != NONE) 	submenu[child_Submenu].parent_Element_Number = count_Element;
	
	if (array_Size != NONE) menu_Elements[count_Element].array_Size = array_Size;
	else if (element_Type == SWITCH_ELEMENT) menu_Elements[count_Element].array_Size = 1;
	else menu_Elements[count_Element].array_Size = NONE;
	
	menu_Elements[count_Element].number_In_Submenu = _number_In_Submenu;
	_number_In_Submenu++;
	count_Element++;
}

//=========================================== ������� �������� ������ ������� =======================================================//
//������� ��������� ������ ��������� submenu.
//������� ��������� ���������:
//              number_Menu - �� ������������ � �����������, ��������� ��� ����������� �������� ������������� ������, ��� �������� �������� ���������� �������.
//elements_Count_In_Submenu - ���������� ��������� � �������.
void Add_New_Menu(const uint8_t number_Menu,uint8_t elements_Count_In_Submenu)
{
	static uint16_t count_Submenu = 0;																								//������� ���������� �������
	_number_In_Submenu = 0;
	
	if (count_Submenu > MAX_SUB_MENUS) return;																						//�������� �� ������������
	submenu[count_Submenu].elements_Count_In_Submenu = elements_Count_In_Submenu;													//���������� ���������� �������

	count_Submenu++;
}

//===================================== ������� ���������� ����������� ������ �������� =============================================//
//������� ��������� ���������:
//element_Count - ���������� ����� �������� � ������� �������.
//submenu_Count - ���������� ����� �������.
void _Selected_Element_Calculate(uint8_t element_Count, uint8_t submenu_Count)
{
	_selected_Element = 0;																											//����� ���������� ����������� ������ ���������� ��������
	if (submenu_Count != 0)																											//�������� �� �������� �������������
	{
		for (int i = 0; i <submenu_Count; i++) _selected_Element = _selected_Element + submenu[i].elements_Count_In_Submenu;		//���������� ����������� ������
	}
	_selected_Element += element_Count;
}

//===================================== ������� ���������� ������� ���������� =====================================================//
//������� �������� � ����� temp_Array ��� temp_Switch ��������� �������� �������� ���� ��� ��������� �� ����.
//������� ��������� ���������:
//            type - ��� �������� �������� ����.
//selected_Element - ���������� ����� �������� �������� ����.
//������� ����������:
//			 count - ���������� ��������� � ������� ���������. 
void _Temp_Array_First_Out(uint8_t type, uint8_t count)
{
	uint8_t* array_Byte = menu_Elements[_selected_Element].data;																	//���������� ���������������� ��������� �������� ��������� ������ �� ���������
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


//====================================== ������� ������ ������ �� ��������� ========================================================//
//������� ��������� ���������:
//element_Count - ���������� ����� �������� � ������� �������.
//submenu_Count - ���������� ����� �������.
//   cursor_Pos - ������� ������� �� ���������� (�� ����� ��������� ����������).
void _Display_Out(uint8_t cursor_Pos)
{
	__flash const char *array_Byte = 0;																								//��������� ��� ������ ������ ��������� ���� �� ������� ������ FLASH
	uint16_t number = _history_Buf_Number[_line2];																			
	uint8_t count = menu_Elements[_selected_Element].array_Size;
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	uint8_t type = menu_Elements[_selected_Element].element_Type;
	array_Byte = menu_Elements[_selected_Element].text;																				//���������� ��������� �������� ������ ������ �������� �������� ����
	
	switch (type)
	{
//--------------------- ����� �� ���������, ���� ��������������� ������� ��������� -------------------------------------------------//
		case HISTORY_ELEMENT:																										//���� ��� �������� - ������� ���������� ������ �������� ���������
			if (_line == 0)
			{
				LED_SetPos(0,0);
				array_Byte = menu_Elements[_selected_Element].text;																	//����� ���������, ������������ � ������� ������ FLASH
				for (int i = 0; i < 16; i++)
				{
					if (*array_Byte == 0) break;
					_LED_Write_Char(*array_Byte);
					array_Byte++;
				}
			}
			else
			{
				uint8_t count_His = menu_Elements[number].array_Size;																//���������� ��������� � ������� ��������� �������� ����
				if (menu_Elements[number].element_Type == LIST_ELEMENT) count_His--;
				if (count_His > _MAX_PARAM_LENGHT) count_His = _MAX_PARAM_LENGHT;
				
				char history_Number[2];
				sprintf(history_Number, "%d", _line-_line2);
				LED_SetPos(1, 14);
				LED_Write_String(history_Number);
				
				LED_SetPos(0,0);
				array_Byte = menu_Elements[number].text;
				for (int i = 0; i < 16; i++)																						//������������ �� ���������� ����� ���������� �������� �������
				{
					if (*array_Byte == 0) break;
					_LED_Write_Char(*array_Byte);
					array_Byte++;
				}
				
				LED_SetPos(1,0);
				if (menu_Elements[number].element_Type == SWITCH_ELEMENT)
				{
					if (_history_Buf_Param[_line2][0] != 0) LED_Write_String_P(PSTR("���"));
					else LED_Write_String_P(PSTR("����"));
				}
				else 
				{
					for (int i = 0; i < count_His; i++) _LED_Write_Char(_history_Buf_Param[_line2][i]);
				}
			}
		break;
		
//---------------------------------------- ����� �� ��������� ����������� ���������� -----------------------------------------------//
		default:																													//����� ���������� �� ��������� �� ���������
			LED_SetPos(0,0);
			for (int i = 0; i < 32; i++)
			{
				if (*array_Byte == 0) 		 																						//�������� �� ����� ������
				{
					break;
				}
				else if (*array_Byte == '/')																						//�������� �� ������� ������
				{
					array_Byte++;
					if (*array_Byte == 'n')
					{
						array_Byte++;
						LED_SetPos(1,0);
					}
					else array_Byte--;
				}
				_LED_Write_Char(*array_Byte);																						//����� ������� �� �����
				array_Byte++;
			}
			
 			if (type != EMPTY_ELEMENT && (_my_Lib_Flags & lib_Flag_Save_Change) == 0)												//���� ��� �������� - �� ������ ������� � ���� �� ���� ����������� ���������� ���������
 			{
 				//������������ ������ ��� ������ ��������� �� ��������� � ���������� ����� ������� ���������
 				if ((_my_Lib_Flags & lib_Flag_Changing_Param) == 0) _Temp_Array_First_Out(type, count);
 			}
			
 			if (type == SWITCH_ELEMENT)
 			{
				LED_SetPos(1,0);
				if (_temp_Switch == 1) LED_Write_String_P(PSTR("���"));																//����� ��������� ������������ ����������� ���������
				else if (_temp_Switch == 0) LED_Write_String_P(PSTR("����"));
 			}
			else if (type == PARAM_ELEMENT || type == WORD_ELEMENT || type == PASSWORD_ELEMENT)
			{
				LED_SetPos(1,0);
				for (int i = 0; i < count; i++) _LED_Write_Char(_temp_Array[i]);													//����� ������ �� ��������� �����������
			}
			else if (type == LIST_ELEMENT)
			{
				LED_SetPos(1,0);
				for (int i = 0; i < count-1; i++) _LED_Write_Char(_temp_Array[i+1]);
			}

			if (_my_Lib_Flags & lib_Flag_Changing_Param)																			//���� ���������� ��������� ���������
			{
				switch(type)																										//��������� �������
				{
					case PARAM_ELEMENT: LED_SetPos(1, cursor_Pos); break;
					case WORD_ELEMENT: LED_SetPos(1, cursor_Pos); break;
					case PASSWORD_ELEMENT: LED_SetPos(1, cursor_Pos); break;
				}
			}
	}
}

//================================= ������� ������������ ������ ��������� ��������� ================================================//
//������� ���������� ����� ������������� ��������� � ����� �������� ������ ��������� �� �������������, �� �� ���������� �������� ���������.
//� �������� ������ ������� ���������� ��������� �� ������� �������� ���������� menu_Elements[selectedElement].data � ��� �������� menu_Elements[selectedElement].text
void _History_Save(void)
{
	uint8_t *array_Byte_Param = 0;																									//��������� ��� ������ ���������
	uint8_t count = menu_Elements[_selected_Element].array_Size;																	//����������� ���������� ��������� � ������� ���������
	uint8_t type = menu_Elements[_selected_Element].element_Type;
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	array_Byte_Param = menu_Elements[_selected_Element].data;																		//���������� ���������� �������� ��������� ���������

	if ((_my_Lib_Flags & lib_Flag_FIFO_End) == 0)																					//���� ������ ������� �� ��������
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
	
	if (_line >= MAX_HISTORY_ELEMENTS) _my_Lib_Flags |= lib_Flag_FIFO_End;															//��������� �����, ���� ������ ������� ��������
}


//================================================ ������� ��������� ���������� ===================================================//
//������� ��������� ���������:
//             key - ����� ������� ������. 
//      cursor_Pos - ������� �������.
//selected_Element - ����� �������� �������� ����.
//������� ����������:
//cursor_Pos - ����� �������� ������� ������� (��������� ������� _Display_Out()).
uint8_t _New_Param(uint8_t key, uint8_t cursor_Pos, uint8_t selected_Element)
{
	uint8_t max_lenght = menu_Elements[_selected_Element].array_Size;																//��������������� ���������� ����� ������� ���������
	if (max_lenght > _MAX_PARAM_LENGHT) max_lenght = _MAX_PARAM_LENGHT;
	
	uint8_t type = menu_Elements[_selected_Element].element_Type;
	uint8_t point = 0;																												//��������������� ���������� ������� �����
	
//---------------------------------------- �������� ��� ������� ������ 0 -----------------------------------------------------------//
	if (key == 0)																													//���� ������ ������ ����
	{
		switch (type)
		{
			default:																												//����� ������ �������
				cursor_Pos++;
				if (cursor_Pos >= max_lenght) cursor_Pos = 0;																		//������������ �������� �������
		}
	}
	else
	{
		for (int i = 0; i < max_lenght; i++) 																						//�������� ������������� ��������� �� ���������� �����
		{
			if (_temp_Array[i] == '.') point = 1;
		}
		switch (key)
		{
//---------------------------------------- �������� ��� ������� ������ 1 -----------------------------------------------------------//
			case 1:																													//���� ������ ������  �����
				switch (type)
				{
					case SWITCH_ELEMENT:																							//���� ��������� ������� �������� �������� - �������������
						if (_temp_Switch == 0) _temp_Switch = 1;																	//��������� ��������� � 0 �� 1 - � 1 �� 0
						else _temp_Switch = 0;
						break;
					
					case PARAM_ELEMENT:																								//���� ��������� ������� �������� �������� ��������
						if (_temp_Array[cursor_Pos] == '+')																			//�������� ���������� �������� ������� �� ���� + � -
						{
							_temp_Array[cursor_Pos] = '-';
						}
						else if (_temp_Array[cursor_Pos] == '-')
						{
							_temp_Array[cursor_Pos] = '+';
						}
						else
						{
							_temp_Array[cursor_Pos]++;																				//���� ��������� ������� �� �������� '+/-' �� �������������� ��������
							if (cursor_Pos == 0 || cursor_Pos == max_lenght-1 || point == 1)										//���� ������ ������� �� ������� ��� ��������� �������, ��� ��� ���������� �����
							{	
								if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x30;									//���� ���� �� 0 �� 9
								else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;															
							}
							else
							{
								if (cursor_Pos == 1)																				//���� ������ ��������� �� ������� 1
								{
									if (_temp_Array[0] == '+' || _temp_Array[0] == '-')												//� �� ������� ����� '+/-'
									{
										if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x30;							//�� ���� ���� �� 0 �� 9
										else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;
									}
									else																							//����� ���� ���� �� 0 �� 9 � '.'
									{
										if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x2E;
										else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;
									}
								}
								else																								//���� ����� � ������� �� �������
								{
									if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x2E;								//���� ���� �� 0 �� 9 � '.'
									else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;
								}
							}
						}
						break;
					
					case WORD_ELEMENT:																								//���� ��������� ������� �������� ��������� ��������
						_temp_Array[cursor_Pos]++;																					//����������������� �������� �������
						if (_temp_Array[cursor_Pos] == '�' + 1) _temp_Array[cursor_Pos] = '0';
						else if (_temp_Array[cursor_Pos] == '9' + 1)  _temp_Array[cursor_Pos] = '.';								//���� ���� �������� ����� ����� �
						else if (_temp_Array[cursor_Pos] == '.' + 1)  _temp_Array[cursor_Pos] = '/';
						else if (_temp_Array[cursor_Pos] == '/' + 1)  _temp_Array[cursor_Pos] = '^';
						else if (_temp_Array[cursor_Pos] == '^' + 1)  _temp_Array[cursor_Pos] = ' ';
						else if (_temp_Array[cursor_Pos] == ' ' + 1)  _temp_Array[cursor_Pos] = '�';
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
						_temp_Array[cursor_Pos]++;																				//���� ��������� ������� �� �������� '+/-' �� �������������� ��������
						if (_temp_Array[cursor_Pos] > 0x39) _temp_Array[cursor_Pos] = 0x30;										//���� ���� �� 0 �� 9
						else if(_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x30;
						break;
				}
			break;
			
//---------------------------------------- �������� ��� ������� ������ 2 -----------------------------------------------------------//
			case 2:																													//���� ������ ������ ����
				switch (type)
				{
					case SWITCH_ELEMENT:																							//���� ��������� ������� �������� �������� - �������������
						if (_temp_Switch == 0) _temp_Switch = 1;																	//��������� ��������� � 0 �� 1 - � 1 �� 0
						else _temp_Switch = 0;
					break;
					
					case PARAM_ELEMENT:																								//���� ��������� ������� �������� �������� ��������
						if (_temp_Array[cursor_Pos] == '+')																			//�������� ���������� �������� ������� �� ���� + � -
						{
							_temp_Array[cursor_Pos] = '-';
						}
						else if (_temp_Array[cursor_Pos] == '-')
						{
							_temp_Array[cursor_Pos] = '+';
						}
						else
						{
							_temp_Array[cursor_Pos]--;																				//���� ��������� ������� �� �������� '+/-' �� �������������� ��������
							if (cursor_Pos == 0 || cursor_Pos == max_lenght-1 || point == 1)										//���� ������ ������� �� ������� ��� ��������� �������, ��� ��� ���������� �����
							{
								if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x39;									//���� ���� �� 0 �� 9
							}
							else
							{
								if (cursor_Pos == 1)																				//���� ������ ��������� �� ������� 1
								{
									if (_temp_Array[0] == '+' || _temp_Array[0] == '-')												//� �� ������� ����� '+/-'
									{
										if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x39;							//�� ���� ���� �� 0 �� 9
									}
									else																							//����� ���� ���� �� 0 �� 9 � '.'
									{
										if (_temp_Array[cursor_Pos] < 0x2E) _temp_Array[cursor_Pos] = 0x39;
										else if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x2E;
									}
								}
								else																								//���� ����� � ������� �� �������
								{
									if (_temp_Array[cursor_Pos] < 0x2E) _temp_Array[cursor_Pos] = 0x39;								//���� ���� �� 0 �� 9 � '.'
									else if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x2E;
								}
							}
						}
						break;
					
					case WORD_ELEMENT:																								//���� ��������� ������� �������� ��������� ��������
						_temp_Array[cursor_Pos]--;																					//����������������� �������� �������
						if (_temp_Array[cursor_Pos] == '�' - 1) _temp_Array[cursor_Pos] = ' ';
						else if (_temp_Array[cursor_Pos] == ' ' - 1) _temp_Array[cursor_Pos] = '^';									//���� ���� �������� ����� ����� �
						else if (_temp_Array[cursor_Pos] == '^' - 1) _temp_Array[cursor_Pos] = '/';
						else if (_temp_Array[cursor_Pos] == '/' - 1) _temp_Array[cursor_Pos] = '.';
						else if (_temp_Array[cursor_Pos] == '.' - 1) _temp_Array[cursor_Pos] = '9';
						else if (_temp_Array[cursor_Pos] == '0' - 1) _temp_Array[cursor_Pos] = '�';
						break;
					
					case LIST_ELEMENT:
						if (_temp_List != 0) _temp_List--;
						
						for (int i = 0; i < max_lenght; i++)
						{
							_temp_Array[i] = *(menu_Elements[selected_Element].data + max_lenght*_temp_List + i);
						}
						break;
						
					case PASSWORD_ELEMENT:
						_temp_Array[cursor_Pos]--;																					//���� ��������� ������� �� �������� '+/-' �� �������������� ��������
						if (_temp_Array[cursor_Pos] < 0x30) _temp_Array[cursor_Pos] = 0x39;											//���� ���� �� 0 �� 9
						break;
				}
			break;
		}
	}
	return cursor_Pos;																												//������� �������� �������� �������
}

//==================================== ������� �������� ���������� �������� ��������� ==============================================//
//������� ��������� ���������:
//*array_Byte - ������ �� ������ ������������� �������.
//      *type - ������ �� ���������� ���� ����������� ���������.
//������� ����������:
//selected_Element - ���������� ����� ��������, �������� �������� ����������.
uint8_t _Menu_Data_Change(uint8_t *array_Byte, uint8_t *type)
{
	uint8_t el_Type = menu_Elements[_selected_Element].element_Type;
	uint8_t count = menu_Elements[_selected_Element].array_Size;
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	if (_my_Lib_Flags & lib_Flag_Save_Change)																						//���� ���� ����������� ���������� ���������
	{	
		_my_Lib_Flags &= ~lib_Flag_Save_Change;																						//����� ����� ���������� ���������
		switch (el_Type)
		{
			case SWITCH_ELEMENT:																									//���� ��� ������� �������� - �������������
				*array_Byte = _temp_Switch;																							//������ ������ �������� � �����
				break;
			
			case PARAM_ELEMENT:	
 			case WORD_ELEMENT:																										//���� ��� ������� ��������� ��������
			case PASSWORD_ELEMENT:
 				for (int i = 0; i < count; i++)
 				{
 					*array_Byte = _temp_Array[i];																					//������ � ����� 
 					array_Byte++;
 				}
 				break;
				 
			case LIST_ELEMENT:
				for (int i = 0; i < count; i++)
				{
					*array_Byte = _temp_Array[i];																					//������ � �����
					array_Byte++;
				}
				break;
		}
		*type = el_Type;
		return _selected_Element;																									//������� ����������� ������ ����������� �������� ����
 	}
 	else
 	{
 		return NONE;																												//���� ��������� �� ���� ������� 0
 	}
}

//============================================= ������� ���������� ��������� �������� ==============================================//
//������� ��������� ��������� �� ������ � ����� ��������� ���������, ����� �������� ����������� ��������� � �������������� �������� �������� 
//������� ��������� ���������:
//   *ptr - ������ �� ������ ������� � ����� ��������� ���������.
//element - ����� ����������� ��������.
void Save_Changes(uint16_t element, uint8_t *ptr)
{
	uint8_t temp = 0;
	uint8_t temp2 = 0;
	uint8_t count = menu_Elements[element].array_Size;
	uint8_t* data_Pointer = menu_Elements[_selected_Element].data;
	if (count > _MAX_PARAM_LENGHT) count = _MAX_PARAM_LENGHT;
	
	uint8_t *ptr_Data = menu_Elements[element].data;
	_History_Save();																												//��������� � ������ ������� ���������� ��������
																										
	switch (menu_Elements[element].element_Type)																					//����� ���� �������������� ��������� �� ��� ����
	{
		case SWITCH_ELEMENT:																										//���� ���������� �������� - �������������
			*ptr_Data = *ptr;
			break;
		
		case PASSWORD_ELEMENT:
		case PARAM_ELEMENT:	
 		case WORD_ELEMENT:																											//���� ���������� �������� ��������
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
//============================================== ������� ������ ������ =============================================================//
//������� ������ ������ ������ ���� � ������ �� ����� ����������. ������� ���������� �����������, ���� �� ����� ������������������ ����� �� ����.
//������� ���������� ����� ����������� ��������.
//������� ��������� ������ �� ��������� ����� ������ �������� ��������� � ��� ���.
uint16_t Goto_Menu (uint8_t *array_Byte, uint8_t *type)
{
	_my_Lib_Flags |= lib_Flag_Menu_Enable | lib_Flag_Set_Changes;																	//��������� ������ ("���� �������" � "��������� �� ����������")
	int h=2500;
	while(1)
	{
		h--;
		if (h==1)
		{
			_my_Lib_Flags &= ~lib_Flag_Menu_Enable;																			//����� ����� "���� �������"
			_my_Lib_Flags |= lib_Flag_Menu_Out;																				//��������� ����� "����� �� ����"
			LED_Clear();
			return EXIT;
		}
		_delay_ms(10);
		static uint8_t element_Count = 0;																							//���������� ����� �������� ���� � ������� �������
		static uint8_t submenu_Count = 0;																							//���������� ����� �������
		static uint8_t cursor_Pos = 255;																							//������� �������
		static uint8_t led_Curs = 0;																								//��������������� ���������� ��� ������� �������� ������� ������ ������
		static uint8_t state = 0;																									//��������������� ���������� ��� ������� �������� ������� ������ ������
		uint8_t stop = 0;																											//��������������� ����������
		uint8_t *temp_Byte = 0;																										//��������� �� �������� �������� �������� ����
		
		_my_Lib_Flags &= ~lib_Flag_Save_Change;
		
//--------------------------------------- ���� ��� ���������� ����� �� ���� --------------------------------------------------------//
		if (_my_Lib_Flags & lib_Flag_Menu_Out)
		{
			_my_Lib_Flags &= ~lib_Flag_Menu_Out;
			element_Count = 0;
			submenu_Count = 0;
		}

//------------------------------------------------- ��������� ������ 0 -------------------------------------------------------------//
		_Selected_Element_Calculate(element_Count, submenu_Count);																	//������ ����������� ������ �������� ����
		if (Key(0) == 1)																											//���� ������ ������ 0
		{
			h=2500;
			uint16_t time_Down = 0;																									//������� ������� �������
			while(Key(0) == 1)
			{
				 
				_delay_ms(3);																										//�����������
				if (_my_Lib_Flags & lib_Flag_Changing_Param)
				{
					if (time_Down < 300)
					{
						time_Down++;																								//���������� �������� ������� ���� ����� ������� �� ��������� 3 ���
					}
					else
					{
						LED_SetPos(1, 17);																							//����� ������� �� ������� ����������
						cursor_Pos = 255;																							//������� ������� � ����������� ���������
						stop = 1;																									//���������� ������� ������ ������
						_my_Lib_Flags |= lib_Flag_Save_Change;																		//���������� ���� ������������ ���������
						_my_Lib_Flags &= ~lib_Flag_Changing_Param;																	//�������� ���� �������������� ���������
						Led(1);																										//������ ���������
					}
				}
				else if (menu_Elements[_selected_Element].element_Type == DEFAULT_ELEMENT)											//���� �� ������������ ��������� ���������, �� ������ ������� ��������� ��������
				{
					if (time_Down < 300)
					{
						time_Down++;																								//���������� �������� ������� ���� ����� ������� �� ��������� 3 ���
					}
					else
					{
						_my_Lib_Flags |= lib_Flag_Save_Change;																		//���������� ���� ������������ ���������
						_my_Lib_Flags &= ~lib_Flag_Menu_Enable;																		//�������� ���� �������������� ���������
						element_Count = 0;																							//����� �� ����
						submenu_Count = 0;
						Led(1);																										//������ ���������
					}
				}
				else if (menu_Elements[_selected_Element].element_Type == BLANK_ELEMENT)
				{
					if (time_Down < 300)
					{
						time_Down++;																								//���������� �������� ������� ���� ����� ������� �� ��������� 3 ���
					}
					else
					{
						_my_Lib_Flags |= lib_Flag_Save_Change;																		//���������� ���� ������������ ���������
						_my_Lib_Flags &= ~lib_Flag_Menu_Enable;																		//�������� ���� �������������� ���������
						Led(1);																										//������ ���������
					}
				}
				
				if (_my_Lib_Flags & lib_Flag_Changing_Param)	//���� ������������ ��������� ��������� � ������ ������� � ���������� - ��������������
				{
					if (menu_Elements[_selected_Element].element_Type == SWITCH_ELEMENT)
					{
						if (led_Curs < 70)
						{
							_delay_ms(1);
							led_Curs++;																									//����������������� ��������
						}
						else																											//���� ������� ������ 500 ��
						{
							led_Curs = 0;																								//����� ��������
							LED_SetPos(1,0);
							if (state != 0)																								//���� ������ ���
							{
								state = 0;
								if (_temp_Switch != 0) LED_Write_String_P(PSTR("���"));													//����� ������
								else LED_Write_String_P(PSTR("����"));
							}
							else																										//���� ����� �������
							{
								state = 1;
								LED_Write_String_P(PSTR("    "));																		//��������� ������
							}
						}
						LED_SetPos(1,17);
					}
					else if (menu_Elements[_selected_Element].element_Type == LIST_ELEMENT)
					{
						if (led_Curs < 70)
						{
							_delay_ms(1);
							led_Curs++;																									//����������������� ��������
						}
						else																											//���� ������� ������ 500 ��
						{
							led_Curs = 0;																								//����� ��������
							LED_SetPos(1,0);
							if (state != 0)																								//���� ������ ���
							{
								state = 0;
								for (int i = 1; i < menu_Elements[_selected_Element].array_Size; i++)
								{
									_LED_Write_Char(_temp_Array[i]);
								}
							}
							else																										//���� ����� �������
							{
								state = 1;
								LED_Write_String_P(PSTR("    "));																		//��������� ������
							}
						}
						LED_SetPos(1,5);
					}
				}
				else if ((_my_Lib_Flags & lib_Flag_Save_Change) && menu_Elements[_selected_Element].element_Type == SWITCH_ELEMENT && stop == 1)//���� ��������������� ���������� stop == 1
				{
					LED_SetPos(1,0);
					if (_temp_Switch != 0) LED_Write_String_P(PSTR("���"));															//������ ���������� �����
					else LED_Write_String_P(PSTR("����"));
					LED_SetPos(1,17);
					stop = 0;																										//����� ����������
				}
			}
			
			if (_my_Lib_Flags & lib_Flag_Save_Change && (menu_Elements[_selected_Element].element_Type == DEFAULT_ELEMENT || menu_Elements[_selected_Element].element_Type == BLANK_ELEMENT))
			{
				*type = BLANK_ELEMENT;
				Led(0);
				return _selected_Element;
			}
			
			if (menu_Elements[_selected_Element].child_Submenu != 0)																//�������� �� �� ������� ��������� �������
			{
				element_Count = 0;
				submenu_Count = menu_Elements[_selected_Element].child_Submenu;														//������� � �������� �������
			}
			else
			{
				if (_my_Lib_Flags & lib_Flag_Save_Change)																			//���� ���������� ���� ������������ ���������
				{
					_my_Lib_Flags &= ~lib_Flag_Changing_Param;																		//����� ����� �������� ���������
				}
				//���� ��������� ������� �������� ������, ���������, ��� ������������� ��������
				else if (menu_Elements[_selected_Element].element_Type == SWITCH_ELEMENT || menu_Elements[_selected_Element].element_Type == PARAM_ELEMENT || menu_Elements[_selected_Element].element_Type == WORD_ELEMENT || menu_Elements[_selected_Element].element_Type == LIST_ELEMENT || menu_Elements[_selected_Element].element_Type == PASSWORD_ELEMENT)
				{
					_my_Lib_Flags |= lib_Flag_Changing_Param;																		//��������� ����� �������� ���������
					if (menu_Elements[_selected_Element].element_Type == PARAM_ELEMENT) cursor_Pos = _New_Param(0, cursor_Pos, _selected_Element);//��������� �������
					if (menu_Elements[_selected_Element].element_Type == WORD_ELEMENT) cursor_Pos = _New_Param(0, cursor_Pos, _selected_Element);
					if (menu_Elements[_selected_Element].element_Type == PASSWORD_ELEMENT) cursor_Pos = _New_Param(0, cursor_Pos, _selected_Element);
				}
			}
			_my_Lib_Flags |= lib_Flag_Set_Changes;																					//��������� ����� ������ �� ���������
			Led(0);																													//���������� ����������
		}

//------------------------------------------------------ ��������� ������ 3 --------------------------------------------------------//
		if (Key(3) == 1)																											//���� ������ ������ �����
		{
			h=2500;
			while(Key(3) == 1)																										//�����������
			{
				for (int i = 0; i < 20; i++)
				{
					_delay_us(500);
					 
				}
			}																						

			if (submenu_Count != 0)																									//���� ������� ������� �� �������
			{
				if (_my_Lib_Flags & lib_Flag_Changing_Param)																		//���� ������������ ��������� ���������
				{
					
				}
				else																												//���� �� ������������ ��������� ��������
				{
					_selected_Element = submenu[submenu_Count].parent_Element_Number;												//������� � �������
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
			else																													//���� ������� ������� �������
			{
				if (_my_Lib_Flags & lib_Flag_Changing_Param)																		//���� ������������ ��������� ���������
				{
					switch(menu_Elements[_selected_Element].element_Type)
					{
						case PARAM_ELEMENT:																							//���� ������ ������� � �������� ����������
						temp_Byte = menu_Elements[_selected_Element].data;															//���������� ��������� ����� �� ������ �������� ���������� ��������
						for (int i = 0; i < sizeof(_temp_Array); i++)
						{
							_temp_Array[i] = *temp_Byte;																			//�������������� �������� ��������
							temp_Byte++;
						}
						break;
						
						case SWITCH_ELEMENT:																						//���� ������ ������� � ���������� ��������������
						_temp_Switch = *menu_Elements[_selected_Element].data;														//�������������� �������� ��������
						break;
					}
				}
				else																												//���� �� ������������ ��������� ��������
				{
					_my_Lib_Flags &= ~lib_Flag_Menu_Enable;																			//����� ����� "���� �������"
					_my_Lib_Flags |= lib_Flag_Menu_Out;																				//��������� ����� "����� �� ����"
					LED_Clear();																									//������� �������
					return EXIT;																									//������� � �������� ���������
				}
			}
			
			cursor_Pos = 255;																										//����������� ������� �������
			_line2 = _line-1;																										//����������� ������������� �������� ������� ������� (��������� ���������)
			_my_Lib_Flags &= ~lib_Flag_Changing_Param;																				//����� ����� �������� ���������
			_my_Lib_Flags |= lib_Flag_Set_Changes;																					//��������� ����� ������ �� ���������
		}
		
//--------------------------------------------------- ��������� ������ 1 -----------------------------------------------------------//
		if (Key(2) == 1)																											//���� ������ ������ �����
		{
			h=2500;
			while(Key(2) == 1)
			{
				for (int i = 0; i < 20; i++)
				{
					_delay_us(500);
					 
				}
			}																						//�����������
			
			if (_my_Lib_Flags & lib_Flag_Changing_Param)																			//���� ���� ��������� ���������� ����������
			{
				_New_Param(1, cursor_Pos, _selected_Element);																		//����� ������� �������� ��������� � ��������
			}
			else
			{
				if (menu_Elements[_selected_Element].element_Type == HISTORY_ELEMENT)												//���� ��������� ������� �������� ������ ��������� ���������
				{
					_line2--;
					if (_line2 == 255) _line2 = _line-1;																			//������������ ������
				}
				else
				{
					element_Count++;																								//��������� �� �������
					if (element_Count > submenu[submenu_Count].elements_Count_In_Submenu-1) element_Count = 0;						//������������ ������ ������������ ��������� �������
				}
			}
			_my_Lib_Flags |= lib_Flag_Set_Changes;																					//��������� ����� �������� ��������� �� ���������
		}

//--------------------------------------------------- ��������� ������ 2 -----------------------------------------------------------//
		if (Key(1) == 1)																											//���� ������ ������ ����
		{
			h=2500;
			while(Key(1) == 1)
			{
				for (int i = 0; i < 20; i++)
				{
					_delay_us(500);
					 
				}
			}																						//�����������
			
			if (_my_Lib_Flags & lib_Flag_Changing_Param)																			//���� ���������� ���� �������� ���������
			{
				_New_Param(2, cursor_Pos, _selected_Element);																		//����� ������� �������� ���������
			}
			else
			{
				if (menu_Elements[_selected_Element].element_Type == HISTORY_ELEMENT)												//���� ��������� ������� �������� ������ ��������� ���������
				{

					_line2++;
					if (_line2 > _line-1) _line2 = 0;																				//������������ ������
				}
				else
				{
					if (element_Count == 0) element_Count = submenu[submenu_Count].elements_Count_In_Submenu-1;						//������������ ������ ������������ ��������� �������
					else element_Count--;																							//��������� �� �������
				}
			}
			_my_Lib_Flags |= lib_Flag_Set_Changes;																					//��������� ����� �������� ��������� �� ���������
		}
		
//----------------------------------------- ������� ������ ������ ------------------------------------------------------------------//
		if ((_my_Lib_Flags & lib_Flag_Changing_Param))			//���� ���������� ���� ��������� ���������� � ������ ������� � ���������� - ��������������
		{
			if (menu_Elements[_selected_Element].element_Type == SWITCH_ELEMENT)
			{
				if (led_Curs < 100)
				{
					_delay_ms(2);
					led_Curs++;																											//����������������� ��������
				}
				else																													//���� ������� ������ 500 ��
				{
					led_Curs = 0;																										//����� ��������
					LED_SetPos(1,0);
					if (state != 0)																										//���� ������ ���
					{
						state = 0;
						if (_temp_Switch != 0) LED_Write_String_P(PSTR("���"));															//����� ������
						else LED_Write_String_P(PSTR("����"));
					}
					else																												//���� ����� �������
					{
						state = 1;
						LED_Write_String_P(PSTR("    "));																				//��������� ������
					}
				}
				LED_SetPos(1,5);
			}
			else if (menu_Elements[_selected_Element].element_Type == LIST_ELEMENT)
			{
				if (led_Curs < 100)
				{
					_delay_ms(2);
					led_Curs++;																											//����������������� ��������
				}
				else																													//���� ������� ������ 500 ��
				{
					led_Curs = 0;																										//����� ��������
					LED_SetPos(1,0);
					if (state != 0)																										//���� ������ ���
					{
						state = 0;
						for (int i = 1; i < menu_Elements[_selected_Element].array_Size; i++)
						{
							_LED_Write_Char(_temp_Array[i]);
						}
					}
					else																												//���� ����� �������
					{
						state = 1;
						LED_Write_String_P(PSTR("    "));																				//��������� ������
					}
				}
				LED_SetPos(1,5);
			}
		}
		
		 
//---------------------------------------- ����� ��������� �� ��������� ------------------------------------------------------------//
		if ( _my_Lib_Flags & lib_Flag_Set_Changes)																					//���� ���������� ���� �������� ��������� �� ���������
		{
			LED_Clear();																											//������� ������
			_my_Lib_Flags &= ~lib_Flag_Set_Changes;																					//����� ����� �������� ��������� �� ���������
			_Selected_Element_Calculate(element_Count, submenu_Count);																//���������� ����������� ������ �������� �������� ����
			_Display_Out(cursor_Pos);																								//����� ������� ����������� ���������� �� ����������
		}
		
		uint16_t ret = _Menu_Data_Change(array_Byte, type);																			//����� ������ ����������� ��������
		if (ret != NONE) return ret;
	}
}
#endif
