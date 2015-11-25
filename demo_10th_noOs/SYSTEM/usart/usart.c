#include "sys.h"
#include "usart.h"	  
#include "led.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void sendChar(u8 ch)
{
	while((USART1->SR & 0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;       
}

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}




#if 0


#define RX_BUF_SIZE 50
u8 receive_buffer[RX_BUF_SIZE] ={0};
u8 *re_ptr_r = NULL;
u8 *re_ptr_w = NULL;
u8 receive_ok = 0;

#define VALID_DATA_LEN 20
u8 uart_package[VALID_DATA_LEN] ={0};
void rx_process_check(void);


void bufPara_initial(void)
{
    re_ptr_r = &receive_buffer[0];
    re_ptr_w = &receive_buffer[0]; 
    receive_ok =0;    
}


char rx_flag =0;
char rx_data;

void rec_data(char data)
{
    u8 Res = data;
    /* 1. get data from serial port, and write it to rx_buffer */
    *re_ptr_w = Res;
    if(re_ptr_w == &receive_buffer[RX_BUF_SIZE -1])
        re_ptr_w = &receive_buffer[0];
    else
        re_ptr_w++;   
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		rx_data = USART_ReceiveData(USART1);   //(USART1->DR);	//��ȡ���յ�������
        rx_flag = 1; 
       
        /* 1. get data from serial port, and write it to rx_buffer */
        *re_ptr_w = rx_data;
        if(re_ptr_w == &receive_buffer[RX_BUF_SIZE -1])
            re_ptr_w = &receive_buffer[0];
        else
            re_ptr_w++;
        
        /* 2. parse the message in buffer, it will set valid command flag when 
              detect there are some valid cmd in buffer
        */
        //rx_process_check(); 		 
     } 
     //USART1->SR &= ~USART_FLAG_RXNE;	          // clear interrupt
} 

#define KEYBUFFSIZE     10
char keyBuff[KEYBUFFSIZE];
char key_indexW = 0;
char key_indexR = 0;
char key_count = 0;

#define ENA_INT() __enable_irq()
#define DIS_INT() __disable_irq()
//#define ENA_INT() 
//#define DIS_INT() 

/**
 * \brief: ��FIFO��ز����ĺ����Ķ���
 */
 
void key_paraInit(void)
{
    char i = 0;
    
    for(i =0; i< KEYBUFFSIZE; i++)
        keyBuff[i] = 0;
    
    key_indexW = 0;
    key_indexR = 0;
    key_count = 0;
}
 
void key_inBuff(char keyValue)
{
    if(key_count >= KEYBUFFSIZE)
        return;
    
    DIS_INT();
    
    key_count ++;
    
    keyBuff[key_indexW] = keyValue;
    
    if(++ key_indexW == KEYBUFFSIZE)
        key_indexW = 0;
    
    ENA_INT();
}

char key_readBuff(void)
{
    char key = 0;

    if(key_count == 0)
        return 0;
    
    DIS_INT(); 
    
    key_count --;
    
    key = keyBuff[key_indexR];
    if( ++key_indexR == KEYBUFFSIZE)
        key_indexR = 0;
    
    ENA_INT(); 
    return key;
}
/**
 * @brief Callback that is called if data has been received by trx.
 *
 * @param rx_frame_array Pointer to data array containing received frame
 */
/*
    data frame

    start bit    data len    data     crc    stop bit
       0x86                                  0x55 0xaa
    
*/
void rx_process_check(void)
{
    uint8_t i,j,k,l;
    uint8_t *rdr;
    
    DIS_INT(); 
    if(*re_ptr_r != 0x86)
    {
        if(re_ptr_r == &receive_buffer[RX_BUF_SIZE-1]) 
            re_ptr_r = &receive_buffer[0];
        else re_ptr_r++;
        
        if(re_ptr_r == re_ptr_w) 
        {
            ENA_INT(); 
            return;
        }
            
    }	
    rdr = re_ptr_r;  //����re_ptr_r��ָ����ʼֵ����������
    
    if(rdr == &receive_buffer[RX_BUF_SIZE-1]) 
        rdr = &receive_buffer[0];
    else rdr++;
    
    if(rdr == re_ptr_w) 
    {
        ENA_INT(); 
        return;
    }
        
    
    i = *rdr;

    if(re_ptr_w > re_ptr_r) 
        k = re_ptr_w - re_ptr_r;  // k: ��ʾ���ݶεĳ���
    else	
        k = RX_BUF_SIZE - (re_ptr_r - re_ptr_w);

    if(k < i+5) // ����Ƚ����ݶεĳ����ǲ�����ȡ�
    {
        ENA_INT(); 
        return;   
    }   

            
    if(rdr == &receive_buffer[RX_BUF_SIZE-1]) 
        rdr = &receive_buffer[0];
    else 
        rdr++;
    
    k = 0;
    for(j=0;j<i;j++)
    {
        k = k^(*rdr);  // k: ���CRC��ֵ����������k��
        if(rdr == &receive_buffer[RX_BUF_SIZE-1]) rdr = &receive_buffer[0];
        else rdr++;
    }	
    l = 0;
    
    // if(*rdr == k)  // �ж�CRC�Ƿ���ȷ
    if(*rdr == *rdr)  
    {	
        if(rdr == &receive_buffer[RX_BUF_SIZE-1]) 
            rdr = &receive_buffer[0];
        else 
            rdr++;
        
        if(*rdr == 0x55)
        {		
            if(rdr == &receive_buffer[RX_BUF_SIZE-1]) rdr = &receive_buffer[0];
            else rdr++;
            if(*rdr == 0xAA)
            {
                for(j=0;j<(i+5);j++)
                {
                    // uart_package message format
                    /*  0x86 data_length  data  crc  stopbit(0x55 0xaa) 
                     *   data_length = 5+ length(data);
                     */
                    uart_package[j] = *re_ptr_r; //receive and save a frame of valid msg.
                    if(re_ptr_r == &receive_buffer[RX_BUF_SIZE-1]) re_ptr_r = &receive_buffer[0];
                    else re_ptr_r++;
                }	
                
                if(uart_package[1] == 0x01)
                    key_inBuff(0x01);
                else if(uart_package[1] == 0x02)
                    key_inBuff(0x02);
                else if(uart_package[1] == 0x03)
                    key_inBuff(0x03);
                else
                    key_inBuff(0xaa); 
                
                
                receive_ok = 1;
                l = 1;
            }
        }
    }
    
    // re_ptr_r Ϊ��ָ�룬ֻ�д���ʼλ��0x86����ʼ��ȡ������ҵ�����ʼֵ0x86��������￪ʼ�ж�
    // ����ж�Ϊ����Ч������֡�����0x86����һ�����ݣ�������ȡ�жϡ�
    if(l == 0)  // l = 0�� ��ʾû���ҵ���Ч������֡���������ƶ�һ������
    {
	    if(re_ptr_r == &receive_buffer[RX_BUF_SIZE-1]) re_ptr_r = &receive_buffer[0];
	    else re_ptr_r++;
    }
    ENA_INT(); 
}

#else


char RedState = 0;
char GreenState = 0;
char BlackState = 0;
char DateState = 0;

void uart_sendString(char *str)
{
    while(*str != '\0')
    {
        sendChar(*str);
        str ++;
    }
}
void uart_sendChar(u8 ch)
{
    sendChar(ch);
}


//void USART1_IRQHandler(void)   
static void test(void)
{
#if 1
    
    u16 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res = USART_ReceiveData(USART1);                   //(USART1->DR);	//��ȡ���յ�������
        
        printf("%c", Res); 
    } 
        
        
#else        
        
        
	u8 Res;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������

        if((USART_RX_STA&0x8000)==0)//����δ���
        {
            if(USART_RX_STA&0x4000)//���յ���0x0d
            {
                if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
                else USART_RX_STA|=0x8000;	//��������� 
            }
            else //��û�յ�0X0D
            {	
                if(Res==0x0d)USART_RX_STA|=0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                    USART_RX_STA++;
                    
                    if(USART_RX_STA>(USART_REC_LEN-1))
                        USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
                }		 
            }
        }   		 
    } 
     
     
     
     
#endif
     
} 

#endif
