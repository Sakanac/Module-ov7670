//在调用12864相关函数前，需要做12864的初始化
#ifndef _12864_H_
#define _12864_H_
#include<keypad4_3.h>
#include <reg52.h>
#include <intrins.h>
#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};	   //延时4us
void lcd12864_init(void);//12864初始化函数
void lcd12864_pos(unsigned char X,unsigned char Y);//12864光标定位函数,x:0-3,y:0-7
void LCD12864disp(unsigned char *p);//12864显示函数
void lcd12864_clear(void);//12864清屏函数
void LCD12864_backspace(void);//向左删除一个汉字
void lcd_wcmd(unsigned char cmd);
void lcd_wdat(unsigned char dat);
#endif