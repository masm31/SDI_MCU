#include "main.h"
#include <stdio.h>
#include <stdbool.h>

#define SAMPLE_COUNT 1000
#define CHANNEL_COUNT 6

float Read_TemperatureWater(uint16_t *ADCValue);
float Read_TemperatureAMB(uint16_t ADCValue);
float Read_Presure(uint16_t *ADCValue );

bool check_tolerance_presure(float new_pressure);
bool check_tolerance_temprature(float new_pressure);