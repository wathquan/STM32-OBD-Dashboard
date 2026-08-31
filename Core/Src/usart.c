//
// Created by anzer on 30/08/2026.
//

#include "usart.h"

UART_HandleTypeDef husart3 = {0};

uint8_t tx_buff[TX_BUFFER_SIZE];
volatile uint16_t ix_r = 0;
volatile uint16_t ix_w = 0;

int _write(int file, char *ptr, int len)
{
	(void)file;
	int data_ix;

	for(data_ix = 0; data_ix < len; data_ix++)
	{
		while((ix_w + 1) % TX_BUFFER_SIZE == ix_r)
		{
			// if full, wait for the transmit buffer to clear
		}
		taskENTER_CRITICAL();
		tx_buff[ix_w] = *ptr++;
		ix_w = (ix_w + 1) % TX_BUFFER_SIZE;
		taskEXIT_CRITICAL();
	}

	USART3->CR1 |= USART_CR1_TXEIE;

	return len;
}

HAL_StatusTypeDef USART3_Init(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_USART3_CLK_ENABLE();

	GPIO_InitTypeDef init_struct = {0};
	init_struct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	init_struct.Mode = GPIO_MODE_AF_PP;
	init_struct.Pull = GPIO_NOPULL;
	init_struct.Speed = GPIO_SPEED_FREQ_LOW;
	init_struct.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOB, &init_struct);

	husart3.Instance = USART3;
	husart3.Init.BaudRate = 115200;
	husart3.Init.WordLength = UART_WORDLENGTH_8B;
	husart3.Init.StopBits = UART_STOPBITS_1;
	husart3.Init.Parity = UART_PARITY_NONE;
	husart3.Init.Mode = UART_MODE_TX;
	husart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if (HAL_UART_Init(&husart3) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_NVIC_SetPriority(USART3_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);

	return HAL_OK;
}