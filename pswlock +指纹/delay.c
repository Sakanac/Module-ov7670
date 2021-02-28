#include"delay.h"

void Delay_Us(int i)
{
	while(--i);
}

void Delay_Ms(int n)
{
 	while(n--)
	{
	 	Delay_Us(125);
	}
}