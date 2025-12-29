#ifndef _USART_H
#define _USART_H

#include <stdio.h>
#include "sys.h"

extern UART_HandleTypeDef UART1_Handler, UART2_Handler;

void uart1_init(uint32_t baud);
void uart1_deinit(void);
void uart2_init(uint32_t baud);
void uart2_deinit(void);
void uartPutChar(UART_HandleTypeDef *huart, uint8_t c);
int uartGetChar(UART_HandleTypeDef *huart);

#endif
