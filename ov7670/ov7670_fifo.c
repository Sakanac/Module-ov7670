
/********************AL422+OV7670�������************************/
// 1��SCL_SCCB       ����ͷ�������ʱ������           
// 2��SDA_SCCB       ����ͷ���������������
// 3��VSYNC          ����ͷ��֡�ź�����
// 4��WEN(WR)        AL422 FIFO������ʹ������
// 5��RRST           AL422 FIFO�������λ���ţ����Խ���ַָ��ֵΪ0
// 6��OE/CS(CS)      AL422 FIFO�����ʹ������ ��OEΪ�͵�ƽʱ������������� ���ߵ�ƽʱ�������������̬     
// 7��RCLK/WR(RD)    AL422 FIFO�Ķ�ʹ������ 
// 8��XCLK           ����ͷ��ʱ���ź�
// 9��D0~D7          AL422 FIFO��8λ������ڣ�ֱ����������ͷ��8λ�������
/*****************************************************************/

/********************AL422+OV7670�������******************************************/
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


/*************************************AL422+OV7670ʹ�÷���*************************/
//1\SCCB      ����ͷ�������
//2\XCLK      Ϊ����ͷ�ṩʱ��
//3\VSYN      ���жϵĴ����ж��Ƿ���Ҫ����Ͷ�ȡͼ��
//4\FIFO      ������в�������ȡͼ����Ϣ�ͱ���ͼ����Ϣ
/**********************************************************************************/
#include <STC12C5A60S2.h>
#include "ov7670_fifo.h" 
#include "ov7670_config.h"
#include "sys.h"


void startSCCB(void)//SCCB����ʼ�ź�
{
    SCCB_SID = 1;     //�����߸ߵ�ƽ
    delay_us(50);

    SCCB_SIC = 1;     //��ʱ���߸ߵ�ʱ���������ɸ�����
    delay_us(50);
 
    SCCB_SID = 0;
    delay_us(50);

    SCCB_SIC = 0;     //�����߻ָ��͵�ƽ��������������Ҫ
    delay_us(50);
}


void stopSCCB(void) //SCCB��ֹͣ�ź�
{
    SCCB_SID = 0;
    delay_us(50);
 
    SCCB_SIC = 1;	
    delay_us(50);
  

    SCCB_SID = 1;	
    delay_us(50);
   
}

void noAck(void)//����������ȡ�е����һ����������
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

unsigned char SCCBwriteByte(unsigned char m_data)//д��һ���ֽڵ����ݵ�SCCB�����ͳɹ�����1������ʧ�ܷ���0
{
  unsigned char j,tem;
  
  for(j=0;j<8;j++) //ѭ��8�η�������
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
  SCCB_SID_IN;//����SDAΪ����
  delay_us(10);
  SCCB_SIC = 1;	
  delay_us(80);
  
  if(SCCB_SID_STATE){tem=0;}   //SDA=1����ʧ�ܣ�����0}
  else {tem=1;}   //SDA=0���ͳɹ�������1
  SCCB_SIC = 0;	
  delay_us(50);	
  SCCB_SID_OUT;//����SDAΪ���
  
  return (tem);  
}


unsigned char SCCBreadByte(void)//��ȡһ���ֽ����ݲ��ҷ��ض�ȡ��������
{
	unsigned char read,j;
	read=0x00;
	
	SCCB_SID_IN;//����SDAΪ����
	delay_us(50);
	for(j=8;j>0;j--) //ѭ��8�ν�������
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
    SCCB_SID_OUT;//����SDAΪ���
	return(read);
}

unsigned char wr_Sensor_Reg(unsigned char regID, unsigned char regDat)//дOV7660�Ĵ������ɹ�����1��ʧ�ܷ���0
{
  startSCCB();//����SCCB ���߿�ʼ��������
  if(0==SCCBwriteByte(0x42))//д��ַ
  {
    stopSCCB();//����SCCB ����ֹͣ��������
    return(0);//���󷵻�
  }
  delay_us(20);
  if(0==SCCBwriteByte(regID))//�Ĵ���ID
  {
    stopSCCB();//����SCCB ����ֹͣ��������
    return(0);//���󷵻�
  }
  delay_us(20);
  if(0==SCCBwriteByte(regDat))//д���ݵ�������
  {
    stopSCCB();//����SCCB ����ֹͣ��������
    return(0);//���󷵻�
  }
  stopSCCB();//����SCCB ����ֹͣ��������
  
  return(1);//�ɹ�����
}

unsigned char rd_Sensor_Reg(unsigned char regID, unsigned char *regDat)//��OV7660�Ĵ������ɹ�����1��ʧ�ܷ���0
{
	//ͨ��д�������üĴ�����ַ
	startSCCB();
	if(0==SCCBwriteByte(0x42))//д��ַ
	{
		stopSCCB();//����SCCB ����ֹͣ��������
		return(0);//���󷵻�
	}
	delay_us(20);
  	if(0==SCCBwriteByte(regID))//������ID
	{
		stopSCCB();//����SCCB ����ֹͣ��������
		return(0);//���󷵻�
	}
	stopSCCB();//����SCCB ����ֹͣ��������
	
	delay_us(20);
	
	//���üĴ�����ַ�󣬲��Ƕ�
	startSCCB();
	if(0==SCCBwriteByte(0x43))//����ַ
	{
		stopSCCB();//����SCCB ����ֹͣ��������
		return(0);//���󷵻�
	}
	delay_us(20);
  	*regDat=SCCBreadByte();//���ض�����ֵ
  	noAck();//����NACK����
  	stopSCCB();//����SCCB ����ֹͣ��������
  	return(1);//�ɹ�����
}

unsigned char Sensor_init(void)//7670��ʼ�����ɹ�����1��ʧ�ܷ���0
{
  unsigned char temp;
  unsigned int i=0;
  
  SCCB_SID_OUT;  //AL422+OV7670 �������ų�ʼ��
  camera_DataIn; //����ͷ8λ���ݶ˿ڳ�ʼ��
  
  temp=0x80;
  
  if(0==wr_Sensor_Reg(0x12, temp)) //Reset SCCB
  {
    return 0 ;//���󷵻�
  }
  
  delay_us(20000);
  
  if(0==rd_Sensor_Reg(0x0b, &temp))//��ID
  {
    return 0 ;//���󷵻�
  }
  
  if(temp==0x73)//OV7670
  {
    for(i=0;i<OV7670_REG_NUM;i++)
    {
      if( 0==wr_Sensor_Reg(OV7670_reg[i][0],OV7670_reg[i][1]))
      {
        return 0;//���󷵻�
      }
    }
  }
  
  else if(temp==0x21)//OV7725
  {
    for(i=0;i<OV7725_REG_NUM;i++)
    {
      if( 0==wr_Sensor_Reg(OV7725_reg[i][0],OV7725_reg[i][1]))
      {
        return 0;//���󷵻�
      }
    }
  }
  return temp; //ok
} 
