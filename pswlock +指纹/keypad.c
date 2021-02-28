
#include<keypad4_3.h>
sbit P10 = P1^0;
sbit P20 = P2^0;
#define uchar unsigned char
#define uint unsigned int
void delayms(uint xms)
	{
	uint i,j;
	for(i=xms;i>0;i--)
		for(j=110;j>0;j--);
	}

	
void led(uint key)
	{
	while(key)
		{
		P10=0;
		delayms(50000);
		P10=1;
		delayms(50000);
		while(1);
		}
	}	
	
	

unsigned char matrixkeyscan()
	{
	uchar temp1,temp2,key;
	//扫描第一行
	P0=0xfe;	//第一行低电平，其余行高电平
	P20=1;
	temp1=P0;	//将当前端口状态值赋给临时变量temp
	temp2=P20;
	temp1=temp1&0xf0;
	if(temp1 != 0xf0||temp2==0)			//若第一行的列有被按下
		{
		delayms(10);
		temp1=P0;
		temp2=P20;	//重新读取一遍端口数据
		temp1=temp1&0xf0;
		if(temp1 != 0xf0||temp2==0)		//确实是否真的被按下
			{
				temp1=P0;
				temp2=P20;
				switch(temp1)			//验证是哪一列被按下并记录
					{
					case 0xde:    key=19;
												break;
					case 0xbe:    key=18;
												break;
					case 0x7e:    key=17;
												break;
					case 0xfe:    if(temp2==0)
													{
													key=16;
													}
												break;
					}
				while(temp1 != 0xf0||temp2==0)		//等待按键被释放
					{
					temp1=P0;
					temp2=P2^0;
					temp1=temp1&0xf0;
					}
			}
		}
		
		
	//扫描第二行	
	P0=0xfd;	//第二行低电平，其余行高电平
	P20=1;
	temp1=P0;	//将当前端口状态值赋给临时变量temp
	temp2=P2^0;
	temp1=temp1&0xf0;
	if(temp1 != 0xf0||temp2==0)			//若第二行的列有被按下
		{
		delayms(10);
		temp1=P0;
		temp2=P20;	//重新读取一遍端口数据
		temp1=temp1&0xf0;
		if(temp1 != 0xf0||temp2==0)		//确实是否真的被按下
			{
				temp1=P0;
				temp2=P20;
				switch(temp1)			//验证是哪一列被按下并记录
					{
					case 0xdd:		key=1;
												break;
					case 0xbd:    key=2;
												break;
					case 0x7d:    key=3;
												break;
					case 0xfd:    if(temp2==0)
													{
													key=13;
													}
												break;
					}
				while(temp1 != 0xf0||temp2==0)		//等待按键被释放
					{
					temp1=P0;
					temp2=P20;
					temp1=temp1&0xf0;
					}
			}
		} 
		
	//扫描第三行	
	P0=0xfb;	//第三行低电平，其余行高电平
	P20=1;
	temp1=P0;	//将当前端口状态值赋给临时变量temp
	temp2=P20;
	temp1=temp1&0xf0;
	if(temp1 != 0xf0||temp2==0)			//若第三行的列有被按下
		{
		delayms(10);
		temp1=P0;
		temp2=P20;	//重新读取一遍端口数据
		temp1=temp1&0xf0;
		if(temp1 != 0xf0||temp2==0)		//确实是否真的被按下
			{
				temp1=P0;
				temp2=P20;
				switch(temp1)			//验证是哪一列被按下并记录
					{
					case 0xdb:		key=4;
												break;
					case 0xbb:    key=5;
												break;
					case 0x7b:    key=6;
												break;
					case 0xfb:    if(temp2==0)
													{
													key=12;
													}
												break;
					}
				while(temp1 != 0xf0||temp2==0)		//等待按键被释放
					{
					temp1=P0;
					temp2=P20;
					temp1=temp1&0xf0;
					}
			}
		}
		
	//扫描第四行	
	P0=0xf7;	//第四行低电平，其余行高电平
	P20=1;
	temp1=P0;	//将当前端口状态值赋给临时变量temp
	temp2=P2^0;
	temp1=temp1&0xf0;
	if(temp1 != 0xf0||temp2==0)			//若第四行的列有被按下
		{
		delayms(10);
		temp1=P0;
		temp2=P2^0;	//重新读取一遍端口数据
		temp1=temp1&0xf0;
		if(temp1 != 0xf0||temp2==0)		//确实是否真的被按下
			{
				temp1=P0;
				temp2=P2^0;
				switch(temp1)			//验证是哪一列被按下并记录
					{
					case 0xd7:		key=7;
												break;
					case 0xb7:    key=8;
												break;
					case 0x77:    key=9;
												break;
					case 0xf7:    if(temp2==0)
													{
													key=11;
													}
												break;
					}
				while(temp1 != 0xf0||temp2==0)		//等待按键被释放
					{
					temp1=P0;
					temp2=P2^0;
					temp1=temp1&0xf0;
					}
			}
		}
	
		
	//扫描第五行
	P0=0xef;	//第五行低电平，其余行高电平
	P20=1;
	temp1=P0;	//将当前端口状态值赋给临时变量temp
	temp2=P2^0;
	temp1=temp1&0xe0;
	if(temp1 != 0xe0||temp2==0)			//若第五行的列有被按下
		{
		delayms(10);
		temp1=P0;
		temp2=P2^0;	//重新读取一遍端口数据
		temp1=temp1&0xe0;
		if(temp1 != 0xe0||temp2==0)		//确实是否真的被按下
			{
				temp1=P0;
				temp2=P2^0;
				switch(temp1)			//验证是哪一列被按下并记录
					{
					case 0xcf:		key=15;
												break;
					case 0xaf:    key=0;
												break;
					case 0x6f:    key=14;
												break;
					case 0xef:    if(temp2==0)
													{
													key=10;
													}
												break;
					}
				while(temp1 != 0xe0||temp2==0)		//等待按键被释放
					{
					temp1=P0;
					temp2=P2^0;
					temp1=temp1&0xe0;
					}												
						}	
					}
			return key;
}
	
