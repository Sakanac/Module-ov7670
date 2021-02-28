#include <12864.h>
#include <keypad4_3.h>
sbit LCD_RS  =  P3^5;            //寄存器选择输入 
sbit LCD_RW  =  P3^6;            //液晶读/写控制
sbit LCD_EN  =  P3^4;            //液晶使能控制
sbit LCD_PSB =  P3^7;            //串/并方式控制
 		
/*******************************************************************/
/*检查LCD忙状态                                                    */
/*lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。      */
/*******************************************************************/



bit lcd_busy()
 {                          
    bit result;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_EN = 1;
    delayNOP();	 //延时4us
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

    LCD_PSB = 1;         //并口方式
    
		lcd_wcmd(0x34);      //扩充指令操作
		delay_ms(5);
    lcd_wcmd(0x30);      //基本指令操作
    delay_ms(5);
    lcd_wcmd(0x0F);      //显示开，关光标
    delay_ms(5);
    lcd_wcmd(0x01);      //清除LCD的显示内容
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
   lcd_wcmd(pos);     //显示地址
}

void LCD12864disp(unsigned char *p)//显示汉字
 {
    unsigned char i;
             
    i = 0;
    while(p[i]!= '\0')
     {                         //显示字符
       lcd_wdat(p[i]);
       i++;
     }		   
}


void LCD12864_backspace()
{
 
	lcd_wcmd(0x10);//光标左移
	LCD12864disp("");
	lcd_wcmd(0x10);//光标左移

}