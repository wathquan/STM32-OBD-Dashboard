//
// Created by anzer on 30/08/2026.
//

#include "obd2.h"

ECU_Data_t *pECU_Data;
OBD2_DisplayMode* p_display_mode;

HAL_StatusTypeDef OBD2_SendQuery(uint8_t pid_ix)
{
	switch(*p_display_mode)
	{
	case DEFAULT:
		TxData[2] = OBD2_DefaultDisplayMode_PIDs[pid_ix];
		break;

	case ADVANCED:
		TxData[2] = OBD2_AdvancedDisplayMode_PIDs[pid_ix];
		break;

	default:
		return HAL_ERROR;
	}

	if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint16_t OBD2_Poll_ECU_Data(void)
{
	uint8_t pid_count = 0U;
	uint16_t update_bits = 0U;

	switch(*p_display_mode)
	{
	  case DEFAULT:
			pid_count = OBD2_DDM_PID_COUNT;
			break;

		case ADVANCED:
			pid_count = OBD2_ADM_PID_COUNT;
			break;

		default:
			break;
	}

	for(uint8_t ix = 0; ix < pid_count; ix++)
	{
		if(OBD2_SendQuery(ix) != HAL_OK)
		{
			update_bits |= (1 << ix);
		}
	}

	return update_bits;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0U)
	{
		if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
		{
			if (RxData[1] == 0x41) // Service mode 01 response
			{
				/*
				 * RxData[0]: length
				 * RxData[1]: should be 0x41 (response to service 1 request)
				 * RxData[2]: OBD2 PID
				 * RxData[3-7]: data
				 */
				switch(RxData[2])
				{
				case 0xA4:
					if(RxData[3] & 0x02)
					{
						pECU_Data->actual_gear = (uint8_t) ((256U * RxData[5] + RxData[6]) / 1000);
					}
					break;

				case 0x0C:
					pECU_Data->engine_rpm = (uint32_t) ((256U * RxData[3] + RxData[4]) / 4);
					break;

				case 0x0D:
					pECU_Data->speed = (uint8_t) RxData[3];
					break;

				case 0x70:
					pECU_Data->boost_pressure = ((float) 256 * RxData[6] + RxData[7]) / 0.03125f;
					break;

				case 0x67:
					if(RxData[3] & 0x03) // Sensor 1 & 2 supported
					{
						pECU_Data->coolant_temperature = (int16_t) round((double) (RxData[4] + RxData[5] - 80) / 2.0);
					}
					else if(RxData[3] & 0x01) // Sensor 1 supported
					{
						pECU_Data->coolant_temperature = (int16_t) (RxData[4] - 40);
					}
					else if(RxData[3] & 0x02) // Sensor 2 supported
					{
						pECU_Data->coolant_temperature = (int16_t) (RxData[5] - 40);
					}
					else // No sensor supported
					{
						pECU_Data->coolant_temperature = COOLANT_TEMPERATURE_UNDEFINED;
					}
					break;

				case 0x2F:
					pECU_Data->fuel_level = (uint8_t) (100 * RxData[3] / 255);
					break;

				case 0x5E:
					pECU_Data->fuel_rate = (float) ((256 * RxData[3] + RxData[4]) / 20.0f);
					break;

				default:
					break;
				}
			}
		}
	}
}

HAL_StatusTypeDef OBD2_Init()
{
	/* Query init */
	TxHeader.Identifier = 0x7df;
	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	TxData[0] = 0x2U;
	TxData[1] = 0x01U;  // Service mode 01: show current data
	TxData[2] = 0x00;   // Metric PID
	TxData[3] = 0xcc;
	TxData[4] = 0xcc;
	TxData[5] = 0xcc;
	TxData[6] = 0xcc;
	TxData[7] = 0xcc;

	/* ECU data structure init */
	pECU_Data->actual_gear = TRANSMISSION_GEAR_UNDEFINED;
	pECU_Data->speed = 0U;
	pECU_Data->fuel_level = 0U;
	pECU_Data->coolant_temperature = COOLANT_TEMPERATURE_UNDEFINED;
	pECU_Data->engine_rpm = 0U;
	pECU_Data->boost_pressure = 0;
	pECU_Data->fuel_rate = 0;

	/* Display mode init */
	*p_display_mode = DEFAULT;

	return HAL_OK;
}