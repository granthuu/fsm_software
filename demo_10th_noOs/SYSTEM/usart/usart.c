#include "sys.h"
#include "usart.h"	  
#include "led.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void sendChar(u8 ch)
{
	while((USART1->SR & 0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;       
}

/*使用microLib的方法*/
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
 

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 

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

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		rx_data = USART_ReceiveData(USART1);   //(USART1->DR);	//读取接收到的数据
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
 * \brief: 与FIFO相关操作的函数的定义
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
    rdr = re_ptr_r;  //这里e_ptr_r　指向　起始值　０ｘ８６
    
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
        k = re_ptr_w - re_ptr_r;  // k: 表示数据段的长度
    else	
        k = RX_BUF_SIZE - (re_ptr_r - re_ptr_w);

    if(k < i+5) // 这里比较数据段的长度是不是相等。
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
        k = k^(*rdr);  // k: 算出CRC的值，并保存在k中
        if(rdr == &receive_buffer[RX_BUF_SIZE-1]) rdr = &receive_buffer[0];
        else rdr++;
    }	
    l = 0;
    
    // if(*rdr == k)  // 判断CRC是否正确
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
    
    // re_ptr_r 为读指针，只有从起始位置0x86处开始读取。如果找到了起始值0x86，则从这里开始判断
    // 如果判断为非有效的命令帧，则从0x86的下一个数据，继续读取判断。
    if(l == 0)  // l = 0； 表示没有找到有效的数据帧，则向下移动一个数据
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
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART1);                   //(USART1->DR);	//读取接收到的数据
        
        printf("%c", Res); 
    } 
        
        
#else        
        
        
	u8 Res;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据

        if((USART_RX_STA&0x8000)==0)//接收未完成
        {
            if(USART_RX_STA&0x4000)//接收到了0x0d
            {
                if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
                else USART_RX_STA|=0x8000;	//接收完成了 
            }
            else //还没收到0X0D
            {	
                if(Res==0x0d)USART_RX_STA|=0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                    USART_RX_STA++;
                    
                    if(USART_RX_STA>(USART_REC_LEN-1))
                        USART_RX_STA=0;//接收数据错误,重新开始接收	  
                }		 
            }
        }   		 
    } 
     
     
     
     
#endif
     
} 

#endif
