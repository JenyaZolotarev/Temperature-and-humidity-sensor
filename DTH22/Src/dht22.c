/*
 * dht22.c
 *
 *  Created on: 5 זמגע. 2017 נ.
 *      Author: Jenya
 */

#include "dht22.h"
extern TIM_HandleTypeDef htim1;


/**
 * Setting up the necessary variables and waiting 1 second (let sensor to start)
 */
void DHT22_Init(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin){
  GPIO_Section = GPIOx;
  GPIO_Pin_Sensor = GPIO_Pin;

  DHT22_SwitchPortToOD();
  HAL_GPIO_WritePin(GPIO_Section, GPIO_Pin_Sensor, GPIO_PIN_SET);
  //initial delay for sensor start
  HAL_Delay(1000);

}

/**
 * Switch port to output mode
 */
void DHT22_SwitchPortToOD(){
  HAL_GPIO_DeInit(GPIO_Section, GPIO_Pin_Sensor);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_Pin_Sensor;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_Section, &GPIO_InitStruct);

}

/**
 * Switch port to input mode
 */
void DHT22_SwithPortToIN(){
  HAL_GPIO_DeInit(GPIO_Section, GPIO_Pin_Sensor);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_Pin_Sensor;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO_Section, &GPIO_InitStruct);
}

/**
 * Reading data from sensor
 */
DHT22_Result DHT22_Read(float *temp, float *hum){
  uint32_t pinStateUs;
  uint8_t data[5];

  DHT22_SwitchPortToOD();
  HAL_GPIO_WritePin(GPIO_Section, GPIO_Pin_Sensor, GPIO_PIN_RESET);
  HAL_Delay(18);
  HAL_GPIO_WritePin(GPIO_Section, GPIO_Pin_Sensor, GPIO_PIN_SET);
  DHT22_SwithPortToIN();

  // first 80us strobe into 0
  if (!waitPinState(GPIO_PIN_RESET,100,&pinStateUs)){
    return DHT22_INIT_NO_80_0_STROBE;
  }

  // second 80us strobe into 1
  if (!waitPinState(GPIO_PIN_SET,100,&pinStateUs)){
    return DHT22_INIT_NO_80_1_STROBE;
  }

  // wait until second 80us strobe disappear
  waitPinState(GPIO_PIN_RESET,90,&pinStateUs);

  // start receiving data
  for(int j = 0; j < 5; ++j) {
    for(int i = 0; i < 8; ++i) {

      // before each bit should be 50us of 0 strobe
      if (!waitPinState(GPIO_PIN_RESET,70,&pinStateUs)){
        return DHT22_DATA_NO_50_0_STROBE;
      }
      // wait until 50us strobe before data bit will end
      waitPinState(GPIO_PIN_SET,70,&pinStateUs);

      //read data bit
      waitPinState(GPIO_PIN_RESET,75,&pinStateUs);
      //shift 0 or previous bit in current byte
      data[j] = data[j] << 1;

      //if > 50us it's 1
      if(pinStateUs > 50){
        data[j] = data[j]+1;
      }

    }
  }

  // ending 50us strobe into 0
  if (!waitPinState(GPIO_PIN_RESET,60,&pinStateUs)){
    return DHT22_DATAEND_NO_50_0_STROBE;
  }
  //checksum validation
  if (data[4] != data[0]+data[1]+data[2]+data[3]){
    return DHT22_DATA_CHECKSUM_ERROR;
  }

  *hum = (float)((data[0] << 8)+data[1])/10.f;
  *temp = (float)((data[2] << 8)+data[3])/10.f;


  return DHT22_OK;
}
/**
 * Using the TIM2 timer waiting(delay) specified microseconds
 */
void delayUs(uint16_t micros) {
	  __HAL_TIM_SetCounter(&htim1, 0); //reset counter after input capture interrupt occurs
	  //__HAL_TIM_GetCounter(&htim1);  //read TIM2 counter value
	  while (__HAL_TIM_GetCounter(&htim1)<micros);
}
/**
 * Wait until pin state will become specified level.
 * If timed out returns PIN_STATE_NOT_REACHED status
 * The parameter "elapsed" will be filled with microseconds elapsed before pin state reached target level
 */
int waitPinState(GPIO_PinState level, uint timeout, uint32_t * elapsed) {
	__HAL_TIM_SetCounter(&htim1, 0); //reset counter after input capture interrupt occurs
  while(HAL_GPIO_ReadPin(GPIO_Section,GPIO_Pin_Sensor)!=level){
    //if timed out then return false
    if (__HAL_TIM_GetCounter(&htim1) > timeout){
      return PIN_STATE_NOT_REACHED;
    }
  }
  * elapsed = __HAL_TIM_GetCounter(&htim1);
  return PIN_STATE_REACHED;
}

