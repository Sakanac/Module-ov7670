#ifndef _LED_H
#define _LED_H
#include<reg52.h>

sbit led_green=P0^0;
sbit led_yellow=P0^1;

void Led_Init(void);
void Led_Green_On(void);// ¼��ָʾ�� 
void Led_Green_Off(void);

void Led_Yellow_On(void);	 //	ʶ��ָʾ�� 
void Led_Yellow_Off(void);

#endif
