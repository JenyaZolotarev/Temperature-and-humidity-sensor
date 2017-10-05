/*
 * dht22.h
 *
 *  Created on: 5 זמגע. 2017 נ.
 *      Author: Jenya
 */

#ifndef DHT22_H_
#define DHT22_H_
#include "stm32f1xx_hal.h"


GPIO_TypeDef * GPIO_Section;
uint16_t GPIO_Pin_Sensor;

typedef enum {
  DHT22_OK = 0,
  DHT22_INIT_NO_80_0_STROBE = 1,
  DHT22_INIT_NO_80_1_STROBE = 2,
  DHT22_DATA_NO_50_0_STROBE = 3,
  DHT22_DATAEND_NO_50_0_STROBE = 4,
  DHT22_DATA_CHECKSUM_ERROR = 5

} DHT22_Result;

typedef enum {
  PIN_STATE_REACHED = 1,
  PIN_STATE_NOT_REACHED = 0
} PIN_STATE_IS_REACHED;

void DHT22_Init(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin);
void DHT22_SwitchPortToOD();
void DHT22_SwithPortToIN();
DHT22_Result DHT22_Read(float *temp, float *hum);
void delayUs(uint16_t micros);
int waitPinState(GPIO_PinState level, uint timeout, uint32_t * elapsed);



#endif /* DHT22_H_ */
