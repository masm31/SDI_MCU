

#include "stm32f1xx_hal.h"
#include "MyGPIO.h"
#include <stdio.h>
#include "main.h"


extern TIM_HandleTypeDef htim2;

#define BUZZER_Pin GPIO_PIN_2
#define BUZZER_Port GPIOA

#define PUMP_Pin GPIO_PIN_15
#define PUMP_Port GPIOA

#define DrainVAlve_Pin GPIO_PIN_15
#define DrainVAlve_Port GPIOB

#define WaterVAlve_Pin GPIO_PIN_13
#define WaterVAlve_Port GPIOC







//GPIO_PinState PinState
 void BUZZER_ON() {
    HAL_GPIO_WritePin(BUZZER_Port, BUZZER_Pin, GPIO_PIN_RESET);
}

void BUZZER_OFF() {
    HAL_GPIO_WritePin(BUZZER_Port, BUZZER_Pin, GPIO_PIN_SET);
}
uint16_t BUZZER(GPIO_PinState PinState){
	HAL_GPIO_WritePin(BUZZER_Port, BUZZER_Pin, PinState);
	return PinState;
}

uint16_t PUMP(GPIO_PinState PinState){
	HAL_GPIO_WritePin(PUMP_Port, PUMP_Pin, PinState);
	printf("PUMP=%d\r\n",PinState);
	return PinState;
}

uint16_t PUMP_SPPED(uint8_t Speed){
	if (Speed>0){
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	}else if (Speed==0){
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	}
	TIM2->CCR1=Speed;//duty cucle  0=Min  maxSpeed =99(if ARR=99)
	return Speed;
}

uint16_t DrainValve(GPIO_PinState PinState){
	HAL_GPIO_WritePin(DrainVAlve_Port, DrainVAlve_Pin, PinState);
	printf("DrainVAlve=%d\r\n",PinState);
	return PinState;
}
uint16_t WaterValve(GPIO_PinState PinState){
	HAL_GPIO_WritePin(WaterVAlve_Port, WaterVAlve_Pin, PinState);
	printf("WaterValve=%d\r\n",PinState);
	return PinState;
}