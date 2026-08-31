//
// Created by anzer on 30/08/2026.
//

#ifndef DASHBOARD_V4_OBD2_H
#define DASHBOARD_V4_OBD2_H

#include <math.h>
#include "main.h"
#include "fdcan.h"

static const uint8_t OBD2_DefaultDisplayMode_PIDs[] = {
	0xA4,  // Transmission Actual Gear
	0x0D,  // Vehicle Speed (km/h)
	0x2F,  // Fuel Tank Level (%)
	0x67,  // Engine Coolant Temperature (°C)
	0x0C,  // Engine Speed (rpm)
	0x70,  // Boost Pressure Control (kPa)
	0x5E   // Engine Fuel Rate (L/h)
};

static const uint8_t OBD2_AdvancedDisplayMode_PIDs[] = {
	// vsi default PIDs
	0x68   // Intake Air Temperature Sensor
};

#define OBD2_DDM_PID_COUNT sizeof(OBD2_DefaultDisplayMode_PIDs) / sizeof(uint8_t)
#define OBD2_ADM_PID_COUNT sizeof(OBD2_AdvancedDisplayMode_PIDs) / sizeof(uint8_t)

#define COOLANT_TEMPERATURE_UNDEFINED (-41)
#define TRANSMISSION_GEAR_UNDEFINED 'U'

typedef struct
{
	uint8_t actual_gear;
	uint8_t speed;                // (km/h)
	uint8_t fuel_level;           // (%)
	int16_t coolant_temperature;  // (°C)
	uint32_t engine_rpm;
	float boost_pressure;         // (kPa)
	float fuel_rate;              // (L/h)
} ECU_Data_t;

typedef enum
{
	DEFAULT,
	ADVANCED,
	OTHER
} OBD2_DisplayMode;

extern ECU_Data_t* pECU_Data;

static uint8_t TxData[8];
static uint8_t RxData[8];

HAL_StatusTypeDef OBD2_SendQuery(uint8_t pid_ix);
uint16_t OBD2_Poll_ECU_Data(void);
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
HAL_StatusTypeDef OBD2_Init(void);

#endif //DASHBOARD_V4_OBD2_H
