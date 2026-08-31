//
// Created by anzer on 30/08/2026.
//

#ifndef DASHBOARD_V4_FDCAN_H
#define DASHBOARD_V4_FDCAN_H

#include "main.h"
#include "stm32h7xx_hal_fdcan.h"

extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_FilterTypeDef ffdcan1;

extern FDCAN_TxHeaderTypeDef TxHeader;
extern FDCAN_RxHeaderTypeDef RxHeader;

HAL_StatusTypeDef FDCAN1_Init(void);

#endif //DASHBOARD_V4_FDCAN_H
