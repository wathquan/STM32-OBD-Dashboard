//
// Created by anzer on 30/08/2026.
//

#ifndef DASHBOARD_V4_USART_H
#define DASHBOARD_V4_USART_H

#include "main.h"
#include "stm32h7xx_hal.h"

#define TX_BUFFER_SIZE 256

extern UART_HandleTypeDef husart3;

extern uint8_t tx_buff[TX_BUFFER_SIZE];
extern volatile uint16_t ix_r;
extern volatile uint16_t ix_w;

int _write(int file, char *ptr, int len);
HAL_StatusTypeDef USART3_Init(void);

#endif //DASHBOARD_V4_USART_H
