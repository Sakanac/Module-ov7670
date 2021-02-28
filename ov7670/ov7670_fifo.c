
/********************AL422+OV7670外接引脚************************/
// 1、SCL_SCCB       摄像头输出配置时钟引脚           
// 2、SDA_SCCB       摄像头输出配置数据引脚
// 3、VSYNC          摄像头的帧信号引脚
// 4、WEN(WR)        AL422 FIFO的输入使能引脚
// 5、RRST           AL422 FIFO的输出复位引脚，可以将地址指针值为0
// 6、OE/CS(CS)      AL422 FIFO的输出使能引脚 ，OE为低电平时，允许数据输出 ，高电平时，数据输出高阻态     
// 7、RCLK/WR(RD)    AL422 FIFO的读使能引脚 
// 8、XCLK           摄像头的时钟信号
// 9、D0~D7          AL422 FIFO的8位数据入口，直接连接摄像头的8位数据输出
/*****************************************************************/

/********************AL422+OV7670外接引脚******************************************/
//1\SIC                P2_0
//2\SID                P2_1
//3\VSYNC              P2_2
//4\WEN                P2_3
//5\RRST               P2_4 
//6\CS                 P2_5
//7\WR/(RD)            P2_6
/////////////////////////////////////////////////////////////////////////////////////////          
//9\D0~D7              P6_0~P6_7
/**********************************************************************************/


/*************************************AL422+OV7670使用方法*************************/
//1\SCCB      摄像头输出配置
//2\XCLK      为摄像头提供时钟
//3\VSYN      场中断的处理，判断是否需要保存和读取图像
//4\FIFO      对其进行操作，读取图像信息和保存图像信息
/**********************************************************************************/
#include <STC12C5A60S2.h>
#include "ov7670_fifo.h" 
#include "ov7670_config.h"
#include "sys.h"


void startSCCB(void)//SCCB的起始信号
{
    SCCB_SID = 1;     //数据线高电平
    delay_us(50);

    SCCB_SIC = 1;     //在时钟线高的时候数据线由高至低
    delay_us(50);
 
    SCCB_SID = 0;
    delay_us(50);

    SCCB_SIC = 0;     //数据线恢复低电平，单操作函数必要
    delay_us(50);
}


void stopSCCB(void) //SCCB的停止信号
{
    SCCB_SID = 0;
    delay_us(50);
 
    SCCB_SIC = 1;	
    delay_us(50);
  

    SCCB_SID = 1;	
    delay_us(50);
   
}

void noAck(void)//用于连续读取中的最后一个结束周期
{
	
	SCCB_SID = 1;	
	delay_us(50);
	
	SCCB_SIC = 1;	
	delay_us(50);
	
	SCCB_SIC = 0;	
	delay_us(50);
	
	SCCB_SID = 0;	
	delay_us(50);

}

unsigned char SCCBwriteByte(unsigned char m_data)//写入一个字节的数据到SCCB，发送成功返回1，发送失败返回0
{
  unsigned char j,tem;
  
  for(j=0;j<8;j++) //循环8次发送数据
  {
    if((m_data<<j)&0x80)
    {
       SCCB_SID = 1;	
    }
    else
    {
       SCCB_SID = 0;	
    }
    delay_us(50);
    SCCB_SIC = 1;	
    delay_us(50);
    SCCB_SIC = 0;	
    delay_us(20);
  }
  delay_us(50);
  SCCB_SID_IN;//设置SDA为输入
  delay_us(10);
  SCCB_SIC = 1;	
  delay_us(80);
  
  if(SCCB_SID_STATE){tem=0;}   //SDA=1发送失败，返回0}
  else {tem=1;}   //SDA=0发送成功，返回1
  SCCB_SIC = 0;	
  delay_us(50);	
  SCCB_SID_OUT;//设置SDA为输出
  
  return (tem);  
}


unsigned char SCCBreadByte(void)//读取一个字节数据并且返回读取到的数据
{
	unsigned char read,j;
	read=0x00;
	
	SCCB_SID_IN;//设置SDA为输入
	delay_us(50);
	for(j=8;j>0;j--) //循环8次接收数据
	{		     
		//delay_us(100);
		SCCB_SIC = 1;
		delay_us(50);
		read=read<<1;
		if(SCCB_SID_STATE) 
		{
			read=read+1;
		}
		SCCB_SIC = 0;
		delay_us(50);
	}	
    SCCB_SID_OUT;//设置SDA为输出
	return(read);
}

unsigned char wr_Sensor_Reg(unsigned char regID, unsigned char regDat)//写OV7660寄存器，成功返回1，失败返回0
{
  startSCCB();//发送SCCB 总线开始传输命令
  if(0==SCCBwriteByte(0x42))//写地址
  {
    stopSCCB();//发送SCCB 总线停止传输命令
    return(0);//错误返回
  }
  delay_us(20);
  if(0==SCCBwriteByte(regID))//寄存器ID
  {
    stopSCCB();//发送SCCB 总线停止传输命令
    return(0);//错误返回
  }
  delay_us(20);
  if(0==SCCBwriteByte(regDat))//写数据到积存器
  {
    stopSCCB();//发送SCCB 总线停止传输命令
    return(0);//错误返回
  }
  stopSCCB();//发送SCCB 总线停止传输命令
  
  return(1);//成功返回
}

unsigned char rd_Sensor_Reg(unsigned char regID, unsigned char *regDat)//读OV7660寄存器，成功返回1，失败返回0
{
	//通过写操作设置寄存器地址
	startSCCB();
	if(0==SCCBwriteByte(0x42))//写地址
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
	delay_us(20);
  	if(0==SCCBwriteByte(regID))//积存器ID
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
	stopSCCB();//发送SCCB 总线停止传输命令
	
	delay_us(20);
	
	//设置寄存器地址后，才是读
	startSCCB();
	if(0==SCCBwriteByte(0x43))//读地址
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
	delay_us(20);
  	*regDat=SCCBreadByte();//返回读到的值
  	noAck();//发送NACK命令
  	stopSCCB();//发送SCCB 总线停止传输命令
  	return(1);//成功返回
}

unsigned char Sensor_init(void)//7670初始化，成功返回1，失败返回0
{
  unsigned char temp;
  unsigned int i=0;
  
  SCCB_SID_OUT;  //AL422+OV7670 控制引脚初始化
  camera_DataIn; //摄像头8位数据端口初始化
  
  temp=0x80;
  
  if(0==wr_Sensor_Reg(0x12, temp)) //Reset SCCB
  {
    return 0 ;//错误返回
  }
  
  delay_us(20000);
  
  if(0==rd_Sensor_Reg(0x0b, &temp))//读ID
  {
    return 0 ;//错误返回
  }
  
  if(temp==0x73)//OV7670
  {
    for(i=0;i<OV7670_REG_NUM;i++)
    {
      if( 0==wr_Sensor_Reg(OV7670_reg[i][0],OV7670_reg[i][1]))
      {
        return 0;//错误返回
      }
    }
  }
  
  else if(temp==0x21)//OV7725
  {
    for(i=0;i<OV7725_REG_NUM;i++)
    {
      if( 0==wr_Sensor_Reg(OV7725_reg[i][0],OV7725_reg[i][1]))
      {
        return 0;//错误返回
      }
    }
  }
  return temp; //ok
} 
