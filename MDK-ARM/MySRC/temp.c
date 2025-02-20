
#include "stm32f1xx_hal.h"
#include "temp.h"

#include "math.h"


#define VCC         3.1     // Supply voltage
#define R_FIXED     10000   // Fixed resistor value in ohms
#define A           0.001129148 // Example Steinhart-Hart coefficients
#define B           0.000234125
#define C           0.0000000876741

#define offsetPresure           800
#define offsetCurrent           4
#define R_4_20MA     150   // Fixed resistor value in ohms



float Read_Presure(uint16_t ADCValue) {
    float voltage, current, presure;
    // Convert ADC value to voltage
    voltage = (ADCValue / 4095.0) * VCC;

    // Calculate Current
    current = 1000*(voltage /R_4_20MA);
    
		if (current < 4.0) current = 4.0;  // Clamp lower bound
    if (current > 20.0) current = 20.0; // Clamp upper bound
		presure=(current - 4.0) * (850); // (current - 4mA) * (10bar / 16mA)
		return presure;
   
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

float Read_TemperatureWater(uint16_t ADCValue) {
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


