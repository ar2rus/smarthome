/*
 * bht1750.h
 *
 * Created: 09.01.2017 16:10:32
 *  Author: gargon
 */ 


#ifndef BHT1750_H_
#define BHT1750_H_


#define BH1750_I2CADDR 0x23

// No active state
#define BH1750_POWER_DOWN 0x00

// Wating for measurment command
#define BH1750_POWER_ON 0x01

// Reset data register value - not accepted in POWER_DOWN mode
#define BH1750_RESET 0x07

// Start measurement at 1lx resolution. Measurement time is approx 120ms.
#define BH1750_CONTINUOUS_HIGH_RES_MODE  0x10

// Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
#define BH1750_CONTINUOUS_HIGH_RES_MODE_2  0x11

// Start measurement at 4lx resolution. Measurement time is approx 16ms.
#define BH1750_CONTINUOUS_LOW_RES_MODE  0x13

// Start measurement at 1lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_HIGH_RES_MODE  0x20

// Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_HIGH_RES_MODE_2  0x21

// Start measurement at 1lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_LOW_RES_MODE  0x23

uint8_t bht1750_start(uint8_t mode);
//� ���������� �������� ����������
uint8_t bht1750_start_mtreg(uint8_t mode, uint8_t mtreg);
uint8_t bht1750_readValues(char* values);



#endif /* BHT1750_H_ */