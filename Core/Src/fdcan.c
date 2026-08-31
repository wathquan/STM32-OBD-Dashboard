//
// Created by anzer on 30/08/2026.
//

#include "fdcan.h"

FDCAN_HandleTypeDef hfdcan1 = {0};
FDCAN_FilterTypeDef ffdcan1 = {0};
FDCAN_TxHeaderTypeDef TxHeader = {0};
FDCAN_RxHeaderTypeDef RxHeader = {0};

HAL_StatusTypeDef FDCAN1_Init(void)
{
	__HAL_RCC_FDCAN_CLK_ENABLE();
	__HAL_RCC_D2SRAM3_CLK_ENABLE();

	hfdcan1.Instance = FDCAN1;
	hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
	hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
	hfdcan1.Init.AutoRetransmission = DISABLE;
	hfdcan1.Init.TransmitPause = DISABLE;
	hfdcan1.Init.ProtocolException = DISABLE;
	hfdcan1.Init.NominalPrescaler = 1;
	hfdcan1.Init.NominalSyncJumpWidth = 10;
	hfdcan1.Init.NominalTimeSeg1 = 69;
	hfdcan1.Init.NominalTimeSeg2 = 10;
	hfdcan1.Init.DataPrescaler = 1;
	hfdcan1.Init.DataSyncJumpWidth = 1;
	hfdcan1.Init.DataTimeSeg1 = 1;
	hfdcan1.Init.DataTimeSeg2 = 1;
	hfdcan1.Init.MessageRAMOffset = 0;
	hfdcan1.Init.StdFiltersNbr = 0;
	hfdcan1.Init.ExtFiltersNbr = 0;
	hfdcan1.Init.RxFifo0ElmtsNbr = 4;
	hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
	hfdcan1.Init.RxFifo1ElmtsNbr = 0;
	hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
	hfdcan1.Init.RxBuffersNbr = 0;
	hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
	hfdcan1.Init.TxEventsNbr = 0;
	hfdcan1.Init.TxBuffersNbr = 0;
	hfdcan1.Init.TxFifoQueueElmtsNbr = 1;
	hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
	if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

	HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);

	ffdcan1.IdType = FDCAN_STANDARD_ID;
	ffdcan1.FilterIndex = 0;
	ffdcan1.FilterType = FDCAN_FILTER_MASK;
	ffdcan1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	ffdcan1.FilterID1 = 0x7E8;
	ffdcan1.FilterID2 = 0x7F8;
	ffdcan1.RxBufferIndex = 0;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &ffdcan1) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	HAL_FDCAN_Start(&hfdcan1);

	return HAL_OK;
}