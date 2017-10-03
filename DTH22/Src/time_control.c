/*
 * time_control.c
 *
 *  Created on: 1 вер. 2017 р.
 *      Author: Jenya
 */
#include "stm32f1xx_hal.h"

uint32_t getUs(void) {
	uint32_t usTicks = HAL_RCC_GetSysClockFreq() / 1000000;
	register uint32_t ms, cycle_cnt;
	do {
		ms = HAL_GetTick();
		cycle_cnt = SysTick->VAL;
	} while (ms != HAL_GetTick());
	return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

void delayUs(uint16_t micros) {
	uint32_t start = getUs();
	while (getUs() - start < (uint32_t) micros) {
		asm("nop");
	}
}

