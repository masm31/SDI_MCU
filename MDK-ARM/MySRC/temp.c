
#include "stm32f1xx_hal.h"
#include "temp.h"

#include "math.h"
#include <stdio.h>
#include <stdbool.h>


#define VCC         3.1     // Supply voltage
#define R_FIXED     10000   // Fixed resistor value in ohms
#define A           0.001129148 // Example Steinhart-Hart coefficients
#define B           0.000234125
#define C           0.0000000876741

#define offsetPresure           800
#define offsetCurrent           4
#define R_4_20MA     150   // Fixed resistor value in ohms

#define NUM_RECORDING 30  // Number of values to track
extern float RecordedPresure[30] ; // Circular buffer
extern float RecordedTemprature[30]; // Circular buffer
extern int indexTMP ; // Index for circular buffer
extern int indexPRE ; // Index for circular buffer 
extern int countTMP ; // Track number of values stored
extern int countPRE ; // Track number of values stored





uint32_t adcPresure[SAMPLE_COUNT];
uint32_t adcPresureSum = 0;
uint32_t adcPresureAvg = 0;

uint32_t adcTemprature[SAMPLE_COUNT];
uint32_t adcTempratureSum = 0;
uint32_t adcTempratureAvg = 0;



float Read_Presure(uint16_t *ADCValue) {
	int h=2;// chanal
	adcPresureSum = 0;
	adcPresureAvg = 0;
	for (int i = 0; i < SAMPLE_COUNT; i++) {
        adcPresure[i] =ADCValue[h];
        adcPresureSum += adcPresure[i];                    // Accumulate sum
				h=h+CHANNEL_COUNT;
   }
    adcPresureAvg = adcPresureSum / SAMPLE_COUNT;  // Compute average
    float voltage, current, presure;
		current = (float)((float)adcPresureAvg * (float)0.0054) -(float)0.0027;
		presure = current * 9.375 - 37.5;
		if(presure <= 0) presure =0;
		return presure;
//    // Convert ADC value to voltage
//    voltage = (ADCValue / 4095.0) * VCC;
//    // Calculate Current
//    current = 1000*(voltage /R_4_20MA);
//    
//		if (current < 4.0) current = 4.0;  // Clamp lower bound
//    if (current > 20.0) current = 20.0; // Clamp upper bound
//		presure=(current - 4.0) * (850); // (current - 4mA) * (10bar / 16mA)
//		return presure;
	
   
}


float Read_TemperatureAMB(uint16_t ADCValue) {
    float voltage, resistance, temperature;
		
    // Convert ADC value to voltage
    voltage = (ADCValue / 4095.0) * VCC;

    // Calculate NTC resistance
    resistance = R_FIXED * (voltage / (VCC - voltage));

    // Apply Steinhart-Hart equation
    float logR = log(resistance);
    float kelvin = 1.0 / (A + B * logR + C * logR * logR * logR);

    // Convert to Celsius
    temperature = kelvin - 273.15;
    return temperature;
}

float Read_TemperatureWater(uint16_t *ADCValue) {
    float voltage, resistance, temperature;
		int j=1;
	  adcTempratureSum = 0;
    adcTempratureAvg = 0;
		for (int i = 0; i < SAMPLE_COUNT; i++) {
        adcTemprature[i] = ADCValue[j];
        adcTempratureSum += adcTemprature[i];                    // Accumulate sum
			j=j+CHANNEL_COUNT;
   }
    adcTempratureAvg = adcTempratureSum / SAMPLE_COUNT;  // Compute average
    // Convert ADC value to voltage
    voltage = (adcTempratureAvg / 4095.0) * VCC;

    // Calculate NTC resistance
    resistance = R_FIXED * (voltage / (VCC - voltage));

    // Apply Steinhart-Hart equation
    float logR = log(resistance);
    float kelvin = 1.0 / (A + B * logR + C * logR * logR * logR);

    // Convert to Celsius
    temperature = kelvin - 273.15;
    return temperature;
}



// Function to check if tolerance exceeds the threshold
bool check_tolerance_temprature(float new_Value) {
	
  // Store new pressure value in the buffer
    RecordedTemprature[indexTMP] = new_Value;

    // Move index in a circular manner
    indexTMP = (indexTMP + 1) % NUM_RECORDING;

    // Ensure count does not exceed NUM_RECORDING
    

    // Find min and max in the last `count` values
    float min_val = RecordedTemprature[0];
    float max_val = RecordedTemprature[0];

    for (int i = 1; i < countTMP; i++) {  // Start from index 1 to avoid issues
        if (RecordedTemprature[i] < min_val) min_val = RecordedTemprature[i];
        if (RecordedTemprature[i] > max_val) max_val = RecordedTemprature[i];
    }
		if (countTMP < NUM_RECORDING) {
        countTMP++;
    }

    float tolerance = max_val - min_val;

    // Debugging output
    printf("New Temprature: %.2f | Min: %.2f | Max: %.2f | ToleranceTemprature: %.2f\n", 
           new_Value, min_val, max_val, tolerance);

    // Return true if tolerance exceeds 3, otherwise false
    return (tolerance > 5.0);
}

bool check_tolerance_presure(float new_pressure){
	// Store new pressure value in the buffer
    RecordedPresure[indexPRE] = new_pressure;

    // Move index in a circular manner
    indexPRE = (indexPRE + 1) % NUM_RECORDING;

    

    // Find min and max in the last `count` values
    float min = RecordedPresure[1];
    float max = RecordedPresure[1];

    for (int i = 1; i < countPRE; i++) {  // Start from index 1 to avoid issues
        if (RecordedPresure[i] < min) min = RecordedPresure[i];
        if (RecordedPresure[i] > max) max = RecordedPresure[i];
    }
		// Ensure count does not exceed NUM_RECORDING
    if (countPRE < NUM_RECORDING) {
        countPRE++;
    }

    float tolerance = max - min;

    // Debugging output
    printf("New presure(: %.2f | Min: %.2f | Max: %.2f | TolerancePresure: %.2f\n", 
           new_pressure, min, max, tolerance);

    // Return true if tolerance exceeds 3, otherwise false
    return (tolerance > 2.0);
}


