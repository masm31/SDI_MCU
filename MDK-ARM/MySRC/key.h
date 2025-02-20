
#include "main.h"

#define START_Pin GPIO_PIN_4
#define START_GPIO_Port GPIOC
#define START_EXTI_IRQn EXTI4_IRQn
#define STOP_Pin GPIO_PIN_12
#define STOP_GPIO_Port GPIOB
#define STOP_EXTI_IRQn EXTI15_10_IRQn
#define DOWN_Pin GPIO_PIN_13
#define DOWN_GPIO_Port GPIOB
#define DOWN_EXTI_IRQn EXTI15_10_IRQn
#define UP_Pin GPIO_PIN_5
#define UP_GPIO_Port GPIOC
#define UP_EXTI_IRQn EXTI5_IRQn