#include "sys.h"
#include <STC12C5A60S2.h>
void delay_ms(int count)  
{
        int i,j;
        for(i=0;i<count;i++)
                for(j=0;j<1000;j++);
}

 void delay_us(unsigned int n) //ÑÓÊ±³ÌÐò
{
	int i=0;
	while(i<n+1)
	_nop_();
}
