
#include "main.h"

 void BUZZER_ON();
 void BUZZER_OFF();
 uint16_t BUZZER(GPIO_PinState PinState);
 uint16_t PUMP(GPIO_PinState PinState);
 uint16_t PUMP_SPPED(uint8_t Speed);
 uint16_t DrainValve(GPIO_PinState PinState);
 uint16_t WaterValve(GPIO_PinState PinState);
 
 
 
#define LEVEL000_Pin GPIO_PIN_3
#define LEVEL000_GPIO_Port GPIOA
#define LEVEL000_EXTI_IRQn EXTI3_IRQn

#define LEVEL100_Pin GPIO_PIN_1
#define LEVEL100_GPIO_Port GPIOA
#define LEVEL100_EXTI_IRQn EXTI1_IRQn

#define LEVEL500_Pin GPIO_PIN_0
#define LEVEL500_GPIO_Port GPIOA
#define LEVEL500_EXTI_IRQn EXTI0_IRQn