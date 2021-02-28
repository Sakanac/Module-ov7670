#include <12864.h>
#include <keypad4_3.h>
sbit LCD_RS  =  P3^5;            //�Ĵ���ѡ������ 
sbit LCD_RW  =  P3^6;            //Һ����/д����
sbit LCD_EN  =  P3^4;            //Һ��ʹ�ܿ���
sbit LCD_PSB =  P3^7;            //��/����ʽ����
 		
/*******************************************************************/
/*���LCDæ״̬                                                    */
/*lcd_busyΪ1ʱ��æ���ȴ���lcd-busyΪ0ʱ,�У���дָ�������ݡ�      */
/*******************************************************************/



bit lcd_busy()
 {                          
    bit result;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_EN = 1;
    delayNOP();	 //��ʱ4us
    result = (bit)(P0&0x80);
    LCD_EN = 0;
    return(result); 
 }

void lcd_wcmd(unsigned char cmd)
{                          
    while(lcd_busy());
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;
    _nop_();
    _nop_(); 
    P0 = cmd;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0;  
}

void lcd_wdat(unsigned char dat)
{                          
    while(lcd_busy());
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_EN = 0;
    P0 = dat;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0; 
}

void lcd12864_init()
{ 

    LCD_PSB = 1;         //���ڷ�ʽ
    
		lcd_wcmd(0x34);      //����ָ�����
		delay_ms(5);
    lcd_wcmd(0x30);      //����ָ�����
    delay_ms(5);
    lcd_wcmd(0x0F);      //��ʾ�����ع��
    delay_ms(5);
    lcd_wcmd(0x01);      //���LCD����ʾ����
    delay_ms(5);
}

void lcd12864_pos(unsigned char X,unsigned char Y)
{                          
   unsigned char  pos;
   if (X==0)
     {X=0x80;}
   else if (X==1)
     {X=0x90;}
   else if (X==2)
     {X=0x88;}
   else if (X==3)
     {X=0x98;}
   pos = X+Y ;  
   lcd_wcmd(pos);     //��ʾ��ַ
}

void LCD12864disp(unsigned char *p)//��ʾ����
 {
    unsigned char i;
             
    i = 0;
    while(p[i]!= '\0')
     {                         //��ʾ�ַ�
       lcd_wdat(p[i]);
       i++;
     }		   
}


void LCD12864_backspace()
{
 
	lcd_wcmd(0x10);//�������
	LCD12864disp("");
	lcd_wcmd(0x10);//�������

}