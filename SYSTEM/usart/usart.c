#include <stdio.h>
#include "usart.h"
#include "delay.h"

#define UART1_PORT      GPIOA
#define UART1_RXPIN     GPIO_PIN_10
#define UART1_TXPIN     GPIO_PIN_9

//加入以下代码,支持printf函数
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

UART_HandleTypeDef UART1_Handler, UART2_Handler;    // UART 句柄

#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义 fputc 函数
int fputc(int ch, FILE *f)
{
    while((USART2->SR & USART_FLAG_TXE) == 0);//循环发送,直到发送完毕
    USART2->DR = (uint8_t) ch;
    return ch;
}

void _ttywrch(int ch) {
}

__attribute__((weak)) int _write(int file, char *ptr, int len) {
    int i;
    for (i = 0; i < len; i++) {
        // 示例：通过你的串口发送一个字符
        // USART_SendData(USART1, ptr[i]);
        // while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        while((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕
        USART1->DR = (uint8_t)ptr[i];
    }
    return len;
}

//初始化IO 串口1
//bound:波特率
void uart1_init(uint32_t baud)
{
    //UART 初始化设置
    UART1_Handler.Instance = USART1;                        // USART1
    UART1_Handler.Init.BaudRate = baud;                    // 波特率
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;     // 字长为 8 位数据格式
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;          // 一个停止位
    UART1_Handler.Init.Parity = UART_PARITY_NONE;           // 无奇偶校验位
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;     // 无硬件流控
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;              // 收发模式
    HAL_UART_Init(&UART1_Handler);                          // HAL_UART_Init() 会使能 UART1

    __HAL_UART_DISABLE_IT(&UART1_Handler, UART_IT_TXE);
    __HAL_UART_CLEAR_FLAG(&UART1_Handler, UART_FLAG_RXNE);
    __HAL_UART_CLEAR_FLAG(&UART1_Handler, UART_FLAG_TXE);
    __HAL_UART_CLEAR_FLAG(&UART1_Handler, UART_FLAG_TC);
//    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);                // 抢占优先级 3，子优先级 3
//    HAL_NVIC_EnableIRQ(USART1_IRQn);                        // 使能 USART1 中断通道
}

void uart1_deinit()
{
    if (HAL_UART_DeInit(&UART1_Handler) != HAL_OK ) {
    }
}

//初始化IO 串口2
//bound:波特率
void uart2_init(uint32_t baud)
{
    //UART 初始化设置
    UART2_Handler.Instance = USART2;                        // USART2
    UART2_Handler.Init.BaudRate = baud;                    // 波特率
    UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B;     // 字长为 8 位数据格式
    UART2_Handler.Init.StopBits = UART_STOPBITS_1;          // 一个停止位
    UART2_Handler.Init.Parity = UART_PARITY_NONE;           // 无奇偶校验位
    UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;     // 无硬件流控
    UART2_Handler.Init.Mode = UART_MODE_TX_RX;              // 收发模式
    HAL_UART_Init(&UART2_Handler);                          // HAL_UART_Init() 会使能 UART1

    __HAL_UART_DISABLE_IT(&UART2_Handler, UART_IT_TXE);
    __HAL_UART_CLEAR_FLAG(&UART2_Handler, UART_FLAG_RXNE);
    __HAL_UART_CLEAR_FLAG(&UART2_Handler, UART_FLAG_TXE);
    __HAL_UART_CLEAR_FLAG(&UART2_Handler, UART_FLAG_TC);
//    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);                // 抢占优先级 3，子优先级 3
//    HAL_NVIC_EnableIRQ(USART1_IRQn);                        // 使能 USART2 中断通道
}

void uart2_deinit()
{
    if (HAL_UART_DeInit(&UART2_Handler) != HAL_OK ) {
    }
}

void uartPutChar(UART_HandleTypeDef* huart, uint8_t c)
{
    while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE) == RESET);
    huart->Instance->DR = c;
}

int uartGetChar(UART_HandleTypeDef* huart)
{
    int c = -1;
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) == RESET) {
        return -1;
    }
    c = (uint32_t)huart->Instance->DR;
    return c;
}

// USART1 中断服务程序
void USART1_IRQHandler(void)                    
{ 
    uint32_t timeout=0;
    uint32_t maxDelay=0x1FFFF;

    /* UART in mode Receiver -------------------------------------------------*/
    if ((__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&UART1_Handler, UART_IT_RXNE) != RESET))
    {
//        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
    }

    /* UART in mode Transifer -------------------------------------------------*/
    if ((__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_TXE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&UART1_Handler, UART_IT_TXE) != RESET))
    {
//        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_TX_DONE);
    }

    // errors
    if (__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_ORE) != RESET)
    {
        __HAL_UART_CLEAR_OREFLAG(&UART1_Handler);
    }
    if (__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_NE) != RESET)
    {
        __HAL_UART_CLEAR_NEFLAG(&UART1_Handler);
    }
    if (__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_FE) != RESET)
    {
        __HAL_UART_CLEAR_FEFLAG(&UART1_Handler);
    }
    if (__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_PE) != RESET)
    {
        __HAL_UART_CLEAR_PEFLAG(&UART1_Handler);
    }
    if (__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_LBD) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&UART1_Handler, UART_FLAG_LBD);
    }
    if (__HAL_UART_GET_FLAG(&UART1_Handler, UART_FLAG_CTS) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&UART1_Handler, UART_FLAG_CTS);
    }
}
