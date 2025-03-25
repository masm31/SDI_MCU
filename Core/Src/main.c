/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "ili9341.h"
// #include "fonts.h"
#include "bfctype.h"

#include "key.h"
#include "temp.h"
#include "EEPROM.h"
#include "MyGPIO.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

extern FONTHEADER_BFC fontArial8_R;
extern FONTHEADER_BFC fontArial10_B;
extern FONTHEADER_BFC fontArial14_B;
extern FONTHEADER_BFC fontArial20_B;
extern FONTHEADER_BFC fontArial36_B;
extern FONTHEADER_BFC fontArial48_B;
extern const PICHEADER Logo_Art100;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c2;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

#define NUM_VALUES 1000 // Number of values to track
#define CHANAL_ADC 6	// Number of values to track
#define REJECT 0
#define PASS 1
#define MAXIMUM_SAVE 10

uint8_t firstTimeLoad = 0;
uint16_t ADC1_Buffer[NUM_VALUES][CHANAL_ADC];
float TempAmb = 1.0, TempWater = 1.0, Presure = 1.0, AveTempWater, AvePresure;
char LCDString[50];
uint8_t ByPassTime = 5, Pos = 0, RefreshDisplay = 0;
uint16_t ByPassTimer = 5;
GPIO_PinState PinStateDrain = 0;
GPIO_PinState PinStateWater = 0;
uint16_t longPress = 0;
volatile uint32_t press_start_time = 0, press_end_time = 0, totalPressTime = 0;
volatile uint8_t button_pressed = 0;
extern uint32_t timePress;
uint32_t press_duration = 0;
uint8_t speedMotor = 0;
float PreAve = 0;
/*this variable for Level*/ ///////////////
uint8_t _LowLevelPass = REJECT, _HighLevelPass = REJECT, _100mLLevelPass = REJECT;
///////////////////////////////////////////////////////
/*this variable use for recording data*/
float RecordedPresure[30] = {0};	// Circular buffer
float RecordedTemprature[30] = {0}; // Circular buffer
int indexTMP = 0;					// Index for circular buffer
int indexPRE = 0;					// Index for circular buffer
int countTMP = 0;					// Track number of values stored
int countPRE = 0;					// Track number of values stored
///////////////////////////////////////////////////////////
/*this variable use to pressere Stage*/
float DelatPressure, PressureSave, intervalPressure, intervalTemprature, primPresure, primTemprature;

uint16_t counter = 0;
extern uint32_t adcPresureAvg;

/////////////////////////////////////
/*variable for Timing Stage*/
uint16_t TimeCount = 0, TimeCounter = 0;
uint16_t Time_Minute = 0, Time_Second;
volatile uint16_t Time_mSecond = 0;
float PressureAux = 0, TempAux = 0;
int CounterRefresh = 0;
uint32_t startSysTick = 0, endSysTick = 0, milliSecond = 0;
extern uint32_t timeMS, timeMS_Total;
uint32_t timeLevel_000_primary = 0, timeLevel_100_primary = 0, timeLevel_500_primary = 0, timeCertain = 2000000;

uint32_t time_5Minute = 0, time_10Minute = 0, _15Minute = 0;
uint32_t timeLevel_000_5 = 0, timeLevel_100_5 = 0, timeLevel_500_5 = 0;
uint32_t timeLevel_000_10 = 0, timeLevel_100_10 = 0, timeLevel_500_10 = 0;
uint32_t timeLevel_000_15 = 0, timeLevel_100_15 = 0, timeLevel_500_15 = 0;
uint32_t Ti = 0, Tf_5 = 0, Tf_10 = 0, Tf_15 = 0;
uint32_t Ti_100mL = 0;
uint32_t Tf_5_100mL = 0, Tf_10_100mL = 0, Tf_15_100mL = 0;
float P_30_5 = 0.0, P_30_10 = 0.0, P_30_15 = 0.0;

/***default settings**********************************************************************************/
uint32_t timeRejecton000Primary = 0, timeRejecton100Primary = 120000, timeRejecton500Primary = 300000;
uint32_t timeRejectonTf_5_100mL = 300000, timeRejectonTf_10_100mL = 300000, timeRejectonTf_15_100mL = 300000;
uint32_t timeRejectonTf_5_500mL = 300000, timeRejectonTf_10_500mL = 300000, timeRejectonTf_15_500mL = 300000;

uint16_t passwordSaved = 12;
float DeltaDesierTemprature = 5;
float PressureSetPoint = 30.0, DesigerDeltaPressure = 5;
float PresurePrimary = 0.0;
uint8_t Max_P30=75;

//////////////////////////////////////////////////////////
/*for  result*/
float P_30_5_100mL = 0.0, P_30_10_100mL = 0.0, P_30_15_100mL = 0.0;
float SDI_5 = 0.0, SDI_10 = 0.0, SDI_15 = 0.0;
float SDI_5_100mL = 0.0, SDI_10_100mL = 0.0, SDI_15_100mL = 1.1;
/**************for Save Data SDI**************************************************/
uint8_t numberOfsampleSave = 1;
float values_SDI[6];
uint8_t saveSDI[32];
uint8_t whichSample = 1;
/**************for menu*************************************************************/
uint16_t pressMenuKey = 0;
uint8_t pressMenuKeyStage = 0;
volatile uint32_t pressStartTime = 0;
volatile uint8_t keyState = 0; // 0: Not pressed, 1: Pressed
uint32_t pressDurationMenu = 0;
uint32_t pressDurationExit = 0;
uint8_t PosT100m = 0, PosT500m = 0, PosTf_5 = 0, PosTf_10 = 0, PosTf_15 = 0;
uint8_t pointerSelection = 0;
uint8_t exitPress = 0;
uint8_t posPresurePrimary = 0;

uint8_t cursorPressure = 0, cursorTiming = 0, cursorTemprature = 0;
uint8_t PosMenuRoot = 0, PosMenuAddress = 0;
/* system menu and passpord*/
#define PAGE_SAVE_SETTING 12
#define NUMBER_DATA_SAVE_SETTING 60 // 15parameter *4(size)
uint8_t saveMenuDataSetting[NUMBER_DATA_SAVE_SETTING];
uint16_t passwordLoad = 0, currentPassword = 0;
/*------RTC-------------*/
RTC_DateTypeDef sDate;
RTC_TimeTypeDef sTime;
RTC_TimeTypeDef sTimeRead;
RTC_DateTypeDef sDateRead;




enum LEVEL_SWITCH
{
	LEVEL_NULL,
	LEVEL_LOW,
	LEVEL_MEDUIM,
	LEVEL_HIGH

};
enum LEVEL_SWITCH levelSwitch = LEVEL_NULL;

enum STAGE_TIME
{
	TIME_PRIMAY,
	TIME_5,
	TIME_10,
	TIME_15
};
enum STAGE_TIME stageTime = TIME_PRIMAY;

enum LEVEL_WATER
{
	WATER_NULL,
	WATER_EMPETY,
	WATER_MEDUIM,
	WATER_FULL,

};
enum LEVEL_WATER levelWater = WATER_EMPETY;
enum MENU
{
	BYPASS_TAB,
	PRESSURE_TAB,
	TIMING_TAB,
	RESULT_TAB,
	HISTORY_TAB,
	EXIT

};
enum MENU menu = BYPASS_TAB;

enum SETTING_MENU
{
	SETTING_PRESURE,
	SETTING_TEMPRATURE,
	SETTING_TIMING,
	SETTING_PASSWORD,
	SETTING_RESET_FACTORY,
	SETTING_EXIT

};
enum SETTING_MENU sttingMenu = SETTING_PRESURE;

enum ERROR_CODE
{
	NO_ERROR,
	EXCEEDE_PRESURE,
	EXCEEDE_TEMPRATURE,
	NO_WATER
};
enum ERROR_CODE erroeCode = NO_ERROR;
enum INTERRUPT_KEY
{
	KEY_NULL,
	KEY_STOP,
	KEY_START,
	KEY_UP,
	KEY_DOWN,
	KEY_STOP_LONG,
	KEY_MENU
};
enum INTERRUPT_KEY key = KEY_NULL;
enum STAGE_PROCESS
{
	ByPASS,
	START_PRESURE,
	START_TIMING
};
enum STAGE_PROCESS stateProcess = ByPASS;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C2_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void ResetProcess(void);
void TapDisplay(uint8_t Tab);
void PresuureSetting(void);
void menuSetting(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void LCD_init()
{
	// HAL_GPIO_WritePin(ILI9341_BACKLIGHT_Port, ILI9341_BACKLIGHT_Pin, GPIO_PIN_SET); // LCD backlight
	BUZZER(GPIO_PIN_RESET); // buzzer off
	BUZZER(GPIO_PIN_SET);	// buzzer on
	LCDInit();
	BUZZER(GPIO_PIN_RESET); // buzzer off
	ClearScreen(WHITE);
	DrawPic(200, 20, &Logo_Art100);
	DrawStringA(20, 110, "PEGCo.", &fontArial20_B, NAVY, WHITE);
	DrawStringA(20, 145, ".Silt density index (SDI).", &fontArial14_B, NAVY, WHITE);
	DrawStringA(20, 170, ".Reference: D4189-07 (2014).", &fontArial14_B, NAVY, WHITE);
	// DrawStringA(20, 195, ".(2014) standard.", &fontArial14_B, NAVY, WHITE);
	DrawStringA(20, 195, ".Version : 1.2.0", &fontArial14_B, NAVY, WHITE);
	HAL_Delay(5000);
	ClearScreen(WHITE);
	TapDisplay(BYPASS_TAB);
	DrawFillRect(104, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Press START key to", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 40, "start process...", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 90, "Press DOWN key to", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 115, "history", &fontArial14_B, NAVY, WHITE);
}

void Clear_Buffer(uint8_t *data, int size)
{
	for (int i = 0; i < size; i++)
	{
		data[i] = 0;
	}
}

int DrawTaskbar(char *Titr, uint8_t Count, uint8_t MaxCount)
{
	int pos = 0;
	char Buff[10];
	DrawFillRect(0, 2, 320, 18, NAVY);
	DrawStringA(5, 3, Titr, &fontArial14_B, WHITE, NAVY);
	sprintf(Buff, "(%.2d/%.2d)", Count, MaxCount);
	pos = DrawStringA(250, 3, Buff, &fontArial14_B, WHITE, NAVY);
	return pos;
}
void DrainTankWater(int Second)
{
	menu = HISTORY_TAB;
	WaterValve(GPIO_PIN_SET);
	DrainValve(GPIO_PIN_SET);
	DrawStringA(0, 3 * 10, "                      ", &fontArial14_B, ILI9341_BLACK, ILI9341_BLACK);
	DrawStringA(0, 3 * 10, "WATER DRAINING", &fontArial14_B, ILI9341_MAGENTA, ILI9341_BLACK);
	printf("Second .... %d\r\n", Second);
	DrawStringA(0, 0, "                     ", &fontArial14_B, ILI9341_MAGENTA, ILI9341_BLACK);
	while (Second)
	{
		Second--;
		// HAL_Delay(1);// create One second
		for (uint32_t i = 0; i < 50000000; i++)
		{
			__NOP();
		}
		if (menu == HISTORY_TAB)
		{
			Second = 0;
		}
		char buffer[100];
		sprintf(buffer, "remain=%d", (int)Second);
		DrawStringA(0, 0, buffer, &fontArial14_B, ILI9341_BLACK, ILI9341_MAGENTA);
		Clear_Buffer((uint8_t *)buffer, 100);
		printf("Second .... %d\r\n", Second);
	}
	WaterValve(GPIO_PIN_RESET);
	DrainValve(GPIO_PIN_RESET);
	DrawStringA(0, 3 * 10, "                         ", &fontArial14_B, ILI9341_BLACK, ILI9341_BLACK);
	DrawStringA(0, 3 * 10, "NO WATER", &fontArial14_B, ILI9341_MAGENTA, ILI9341_BLACK);
}

void menuSetting(void)
{
	ClearScreen(WHITE);
	DrawFillRect(1, 1, 320, 20, NAVY);
	/*----------------------read from EEPROM----------------------------------------------------*/

	Clear_Buffer(saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	EEPROM_Read(PAGE_SAVE_SETTING, 0, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	uint32_t recoveredValues[NUMBER_DATA_SAVE_SETTING / 4];

	memcpy(recoveredValues, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	timeRejecton100Primary = recoveredValues[0];
	timeRejecton500Primary = recoveredValues[1];
	timeRejectonTf_5_100mL = recoveredValues[2];
	timeRejectonTf_10_100mL = recoveredValues[3];
	timeRejectonTf_15_100mL = recoveredValues[4];
	timeRejectonTf_5_500mL = recoveredValues[5];
	timeRejectonTf_10_500mL = recoveredValues[6];
	timeRejectonTf_15_500mL = recoveredValues[7];
	PresurePrimary = (float)recoveredValues[8] / 10.0;
	PressureSetPoint = (float)recoveredValues[9] / 10.0;
	DesigerDeltaPressure = (float)recoveredValues[10] / 10.0;
	DeltaDesierTemprature = (float)recoveredValues[11] / 10.0;

	passwordSaved = (uint16_t)recoveredValues[14];

PASSWORD:
	while (1)
	{
		/*-------------------------------------check password-------------------------------------------------------------------*/
		currentPassword = 0;
		DrawFillRect(1, 20, 320, 240, WHITE);
		Pos = DrawStringA(5, 30, "Enter password", &fontArial14_B, BLACK, WHITE);
		sprintf(LCDString, "%.4d", currentPassword);
		Pos = DrawStringA(100, 90, LCDString, &fontArial48_B, WHITE, BLACK);
		while (1)
		{
			if (key == KEY_STOP)
			{
				key = KEY_NULL;
				exitPress = 1;
				goto Exitmenu;
			}
			if (key == KEY_START)
			{
				key = KEY_NULL;
				break;
			}
			if ((key == KEY_UP) && (currentPassword < 1000))
			{
				key = KEY_NULL;
				RefreshDisplay = 1;
				currentPassword++;
			}
			if ((key == KEY_DOWN) && (currentPassword >= 1))
			{
				key = KEY_NULL;
				RefreshDisplay = 1;
				currentPassword--;
			}
			if (RefreshDisplay == 1)
			{
				RefreshDisplay = 0;
				sprintf(LCDString, "%.4d", currentPassword);
				Pos = DrawStringA(100, 90, LCDString, &fontArial48_B, WHITE, BLACK);
			}
		}
		if (currentPassword == passwordSaved)
		{
			DrawFillRect(20, 150, 240, 20, WHITE);
			Pos = DrawStringA(20, 150, "Password accepted", &fontArial14_B, DARKGREEN, WHITE);
			HAL_Delay(1000);
			goto RootMenu;
		}
		else
		{
			DrawFillRect(20, 150, 240, 20, WHITE);
			Pos = DrawStringA(20, 150, "Password Wrong", &fontArial14_B, RED, WHITE);
		}

		HAL_Delay(2000);
		goto PASSWORD;
	Exitmenu:
		if (exitPress)
		{
			/*-------------------------------------------------------write data to EEPROM--------------------**/
			uint32_t values[NUMBER_DATA_SAVE_SETTING / 4];
			values[0] = timeRejecton100Primary;
			values[1] = timeRejecton500Primary;
			values[2] = timeRejectonTf_5_100mL;
			values[3] = timeRejectonTf_10_100mL;
			values[4] = timeRejectonTf_15_100mL;
			values[5] = timeRejectonTf_5_500mL;
			values[6] = timeRejectonTf_10_500mL;
			values[7] = timeRejectonTf_15_500mL;
			values[8] = PresurePrimary * 10;
			values[9] = PressureSetPoint * 10;
			values[10] = DesigerDeltaPressure * 10;
			values[11] = DeltaDesierTemprature * 10;

			values[14] = passwordSaved;
			exitPress = 0;
			memcpy(saveMenuDataSetting, values, NUMBER_DATA_SAVE_SETTING / 4 * sizeof(uint32_t));
			EEPROM_Write(PAGE_SAVE_SETTING, 0, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);

			/*--------------------------------------------------------------------------------------------------*/
			ClearScreen(WHITE);
			exitPress = 0;
			break;
		}

		DrawFastHLine(1, 22, 320, NAVY);

		/*------------------------------------------choice Root menu*-------------------------------------------*/
		DrawTaskbar("Menu", 1, 5);
		key = KEY_NULL;
		sttingMenu = SETTING_PRESURE;
		uint8_t cursorRoot = 0;
	RootMenu:
		DrawFillRect(3, 20, 317, 220, WHITE);
		DrawTaskbar("Menu", 1, 5);
		cursorRoot = 0;
		sttingMenu = SETTING_PRESURE;

		// PosMenuRoot = DrawStringA(5, 30, ">", &fontArial14_B, RED, WHITE);
		Pos = DrawStringA(5, 30, "1.Pressure", &fontArial14_B, WHITE, BLACK);
		Pos = DrawStringA(5, 60, "2.Temprature", &fontArial14_B, BLACK, WHITE);
		Pos = DrawStringA(5, 90, "3.Timing", &fontArial14_B, BLACK, WHITE);
		Pos = DrawStringA(5, 120, "4.Password", &fontArial14_B, BLACK, WHITE);
		Pos = DrawStringA(5, 150, "5.Reset_factory", &fontArial14_B, BLACK, WHITE);

		uint8_t exitPoS;
		/*---------------------------------------------------- Root Menu------------------------------------------------------------*/
		while (1)
		{

			if ((key == KEY_DOWN) && (cursorRoot < 4))
			{
				key = KEY_NULL;
				RefreshDisplay = 1;
				cursorRoot++;
			}
			if ((key == KEY_UP) && (cursorRoot > 0))
			{
				key = KEY_NULL;
				RefreshDisplay = 1;
				cursorRoot--;
			}
			if (RefreshDisplay == 1)
			{
				RefreshDisplay = 0;
				DrawTaskbar("Menu", cursorRoot + 1, 5);
				if (cursorRoot == 0)
				{
					sttingMenu = SETTING_PRESURE;
					// DrawFillRect(3, 20, 300, 220, WHITE);
					//  PosMenuRoot = DrawStringA(5, 30, ">", &fontArial14_B, RED, WHITE);
					Pos = DrawStringA(5, 30, "1.Pressure", &fontArial14_B, WHITE, BLACK);
					Pos = DrawStringA(5, 60, "2.Temprature", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 90, "3.Timing", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 120, "4.Password", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 150, "5.Reset_factory", &fontArial14_B, BLACK, WHITE);
				}
				if (cursorRoot == 1)
				{
					sttingMenu = SETTING_TEMPRATURE;
					// DrawFillRect(3, 20, 300, 220, WHITE);
					Pos = DrawStringA(5, 30, "1.Pressure", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 60, "2.Temprature", &fontArial14_B, WHITE, BLACK);
					Pos = DrawStringA(5, 90, "3.Timing", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 120, "4.Password", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 150, "5.Reset_factory", &fontArial14_B, BLACK, WHITE);
				}
				if (cursorRoot == 2)
				{
					sttingMenu = SETTING_TIMING;
					// DrawFillRect(3, 20, 300, 220, WHITE);
					Pos = DrawStringA(5, 30, "1.Pressure", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 60, "2.Temprature", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 90, "3.Timing", &fontArial14_B, WHITE, BLACK);
					Pos = DrawStringA(5, 120, "4.Password", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 150, "5.Reset_factory", &fontArial14_B, BLACK, WHITE);
				}
				if (cursorRoot == 3)
				{
					sttingMenu = SETTING_PASSWORD;
					// DrawFillRect(3, 20, 300, 220, WHITE);
					Pos = DrawStringA(5, 30, "1.Pressure", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 60, "2.Temprature", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 90, "3.Timing", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 120, "4.Password", &fontArial14_B, WHITE, BLACK);
					Pos = DrawStringA(5, 150, "5.Reset_factory", &fontArial14_B, BLACK, WHITE);
				}
				if (cursorRoot == 4)
				{
					sttingMenu = SETTING_RESET_FACTORY;
					// DrawFillRect(3, 20, 300, 220, WHITE);
					Pos = DrawStringA(5, 30, "1.Pressure", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 60, "2.Temprature", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 90, "3.Timing", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 120, "4.Password", &fontArial14_B, BLACK, WHITE);
					Pos = DrawStringA(5, 150, "5.Reset_factory", &fontArial14_B, WHITE, BLACK);
				}
			}

			if (key == KEY_START)
			{
				break;
			}
			if (key == KEY_STOP)
			{
				exitPress = 1;
				goto Exitmenu;
			}
		}
		key = KEY_NULL;

		/*--------------------------------------------------execute sub menu---------------------------------------------------*/

		while (1)
		{
			if (key == KEY_STOP)
			{
				key = KEY_NULL;
				break;
			}

			switch (sttingMenu)
			{
			case SETTING_PRESURE: /*------------------------------presure----------------------------------------*/
			PressureMenu:
				cursorPressure = 0;
				DrawFillRect(1, 20, 320, 240, WHITE);
				DrawTaskbar("Pressure", cursorPressure + 1, 3);
				Pos = DrawStringA(5, 30, "1.Primary Pressure", &fontArial14_B, WHITE, BLACK);
				Pos = DrawStringA(5, 60, "2.SetPoint Pressure", &fontArial14_B, BLACK, WHITE);
				Pos = DrawStringA(5, 90, "3.Delta Pressure", &fontArial14_B, BLACK, WHITE);
				while (1)
				{
					if (key == KEY_STOP)
					{
						key = KEY_NULL;
						goto RootMenu;
					}
					if (key == KEY_START)
					{
						key = KEY_NULL;
						break;
					}
					if ((key == KEY_DOWN) && (cursorPressure < 2))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						cursorPressure++;
					}
					if ((key == KEY_UP) && (cursorPressure > 0))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						cursorPressure--;
					}
					if (RefreshDisplay == 1)
					{
						RefreshDisplay = 0;
						DrawTaskbar("Pressure", cursorPressure + 1, 3);
						if (cursorPressure == 0)
						{
							// DrawFillRect(3, 20, 300, 220, WHITE);
							Pos = DrawStringA(5, 30, "1.Primary Pressure", &fontArial14_B, WHITE, BLACK);
							Pos = DrawStringA(5, 60, "2.SetPoint Temprature", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 90, "3.Delta Pressure", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorPressure == 1)
						{
							// DrawFillRect(3, 20, 300, 220, WHITE);
							Pos = DrawStringA(5, 30, "1.Primary Pressure", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 60, "2.SetPoint Temprature", &fontArial14_B, WHITE, BLACK);
							Pos = DrawStringA(5, 90, "3.Delta Pressure", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorPressure == 2)
						{
							// DrawFillRect(3, 20, 300, 220, WHITE);
							Pos = DrawStringA(5, 30, "1.Primary Pressure", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 60, "2.SetPoint Temprature", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 90, "3.Delta Pressure", &fontArial14_B, WHITE, BLACK);
						}
					}
				}
				key = KEY_NULL;
				while (1)
				{
					DrawFillRect(3, 20, 320, 220, WHITE);
					DrawTaskbar("Pressure", 1, 1);

					/************************************************************primary pressure*************************************/
					if (cursorPressure == 0)
					{
						sprintf(LCDString, "%2.1f", PresurePrimary);
						DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
						DrawStringA(2, 23, "If the incoming water pressure exceeds this ", &fontArial10_B, NAVY, WHITE);
						DrawStringA(2, 36, "amount, it means there is no water in the pipe.", &fontArial10_B, NAVY, WHITE);
						DrawStringA(235, 180, "Min:0.0", &fontArial14_B, BLACK, WHITE);
						DrawStringA(235, 205, "Max:40.0", &fontArial14_B, BLACK, WHITE);
						while (1)
						{
							if (key == KEY_STOP)
							{
								key = KEY_NULL;
								goto PressureMenu;
							}

							if ((key == KEY_UP) && (PresurePrimary < 40.0))
							{
								key = KEY_NULL;
								RefreshDisplay = 1;
								PresurePrimary = PresurePrimary + 0.1;
							}
							if ((key == KEY_DOWN) && (PresurePrimary >= 0.1))
							{
								key = KEY_NULL;
								RefreshDisplay = 1;
								PresurePrimary = PresurePrimary - 0.1;
							}
							if (RefreshDisplay == 1)
							{

								RefreshDisplay = 0;
								sprintf(LCDString, "%2.1f", PresurePrimary);
								DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							}
						}
					}
					/************************************************************Setpoint pressure*************************************/
					if (cursorPressure == 1)
					{
						DrawFillRect(3, 20, 300, 220, WHITE);
						sprintf(LCDString, "%2.1f", PressureSetPoint);
						DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
						DrawStringA(235, 180, "Min:0.0", &fontArial14_B, BLACK, WHITE);
						DrawStringA(235, 205, "Max:40.0", &fontArial14_B, BLACK, WHITE);
						DrawStringA(2, 23, "Adjust Setpoint pressure for Calculate SDI", &fontArial10_B, NAVY, WHITE);
						while (1)
						{
							if (key == KEY_STOP)
							{
								key = KEY_NULL;
								goto PressureMenu;
							}

							if ((key == KEY_UP) && (PressureSetPoint < 40.0))
							{
								key = KEY_NULL;
								RefreshDisplay = 1;
								PressureSetPoint = PressureSetPoint + 0.1;
							}
							if ((key == KEY_DOWN) && (PressureSetPoint >= 0.1))
							{
								key = KEY_NULL;
								RefreshDisplay = 1;
								PressureSetPoint = PressureSetPoint - 0.1;
							}
							if (RefreshDisplay == 1)
							{

								RefreshDisplay = 0;
								sprintf(LCDString, "%2.1f", PressureSetPoint);
								DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							}
						}
					}
					/************************************************************Delta pressure*************************************/
					if (cursorPressure == 2)
					{
						DrawFillRect(3, 20, 300, 220, WHITE);
						sprintf(LCDString, "%2.1f", DesigerDeltaPressure);
						DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
						DrawStringA(2, 23, "Adjust Delta pressure", &fontArial10_B, NAVY, WHITE);
						DrawStringA(235, 180, "Min:0.0", &fontArial14_B, BLACK, WHITE);
						DrawStringA(235, 205, "Max:40.0", &fontArial14_B, BLACK, WHITE);
						while (1)
						{
							if (key == KEY_STOP)
							{
								key = KEY_NULL;
								goto PressureMenu;
							}

							if ((key == KEY_UP) && (DesigerDeltaPressure < 30.0))
							{
								key = KEY_NULL;
								RefreshDisplay = 1;
								DesigerDeltaPressure = DesigerDeltaPressure + 0.1;
							}
							if ((key == KEY_DOWN) && (DesigerDeltaPressure >= 0.1))
							{
								key = KEY_NULL;
								RefreshDisplay = 1;
								DesigerDeltaPressure = DesigerDeltaPressure - 0.1;
							}
							if (RefreshDisplay == 1)
							{

								RefreshDisplay = 0;
								sprintf(LCDString, "%2.1f", DesigerDeltaPressure);
								DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							}
						}
					}
				}

				break;
			case SETTING_TEMPRATURE: /*-----------------------------------Temprature----------------------------------------*/
			TempratureMenu:
				cursorTemprature = 0;
				DrawFillRect(1, 20, 320, 240, WHITE);
				DrawTaskbar("Temprature", cursorTiming + 1, 1);
				Pos = DrawStringA(5, 30, "1.Delta temprature", &fontArial14_B, WHITE, BLACK);
				while (1)
				{

					if (key == KEY_STOP)
					{
						key = KEY_NULL;
						goto RootMenu;
					}
					if (key == KEY_START)
					{
						key = KEY_NULL;
						break;
					}
					if ((key == KEY_DOWN) && (cursorTemprature < 1))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						cursorTemprature++;
					}
					if ((key == KEY_UP) && (cursorTemprature > 0))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						cursorTemprature--;
					}
					if (RefreshDisplay == 1)
					{
						RefreshDisplay = 0;
						DrawTaskbar("Temprature", cursorTemprature + 1, 1);
						if (cursorTemprature == 0)
						{
							DrawFillRect(2, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "1.Delta temprature", &fontArial14_B, WHITE, BLACK);
						}
					}
				}
				if (cursorTemprature == 0)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%2.1f", DeltaDesierTemprature);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					DrawStringA(235, 180, "Min:0.0", &fontArial14_B, BLACK, WHITE);
					DrawStringA(235, 205, "Max:40.0", &fontArial14_B, BLACK, WHITE);
					DrawStringA(2, 23, "set maximum Delta temprature ", &fontArial10_B, NAVY, WHITE);
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TempratureMenu;
						}

						if ((key == KEY_UP) && (DeltaDesierTemprature < 40.0))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							DeltaDesierTemprature = DeltaDesierTemprature + 0.1;
						}
						if ((key == KEY_DOWN) && (DeltaDesierTemprature > 0.1))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							DeltaDesierTemprature = DeltaDesierTemprature - 0.1;
						}
						if (RefreshDisplay == 1)
						{

							RefreshDisplay = 0;
							sprintf(LCDString, "%2.1f", DeltaDesierTemprature);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
						}
					}
				}
				break;
			case SETTING_TIMING: /*------------------------------Timing----------------------------------------*/

			TimingMenu:
				cursorTiming = 0;
				DrawFillRect(1, 20, 320, 240, WHITE);
				DrawTaskbar("Timming", cursorTiming + 1, 8);
				Pos = DrawStringA(5, 30, "1.100mL primary", &fontArial14_B, WHITE, BLACK);
				Pos = DrawStringA(5, 60, "2.500mL primary", &fontArial14_B, BLACK, WHITE);
				Pos = DrawStringA(5, 90, "3.Tf_100mL_5min", &fontArial14_B, BLACK, WHITE);
				Pos = DrawStringA(5, 120, "4.Tf_500mL_5min", &fontArial14_B, BLACK, WHITE);
				Pos = DrawStringA(5, 150, "5.Tf_100mL_10min", &fontArial14_B, BLACK, WHITE);
				Pos = DrawStringA(5, 180, "6.Tf_500mL_10min", &fontArial14_B, BLACK, WHITE);
				Pos = DrawStringA(5, 210, "7.Tf_100mL_15min", &fontArial14_B, BLACK, WHITE);
				Pos = DrawStringA(5, 240, "8.Tf_500mL_15min", &fontArial14_B, BLACK, WHITE);

				while (1)
				{
					if (key == KEY_STOP)
					{
						key = KEY_NULL;
						goto RootMenu;
					}
					if (key == KEY_START)
					{
						key = KEY_NULL;
						break;
					}
					if ((key == KEY_DOWN) && (cursorTiming < 7))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						cursorTiming++;
					}
					if ((key == KEY_UP) && (cursorTiming > 0))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						cursorTiming--;
					}
					if (RefreshDisplay == 1)
					{
						RefreshDisplay = 0;
						DrawTaskbar("Timming", cursorTiming + 1, 8);
						if (cursorTiming == 0)
						{
							DrawFillRect(2, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "1.100mL primary", &fontArial14_B, WHITE, BLACK);
							DrawFillRect(Pos, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 60, "2.500mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 90, "3.Tf_100mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 120, "4.Tf_500mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 120, 320, 14, WHITE);
							Pos = DrawStringA(5, 150, "5.Tf_100mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 180, "6.Tf_500mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 210, "7.Tf_100mL_15min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 240, "8.Tf_500mL_15min", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorTiming == 1)
						{
							DrawFillRect(2, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "1.100mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 60, "2.500mL primary", &fontArial14_B, WHITE, BLACK);
							DrawFillRect(Pos, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 90, "3.Tf_100mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 120, "4.Tf_500mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 120, 320, 14, WHITE);
							Pos = DrawStringA(5, 150, "5.Tf_100mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 180, "6.Tf_500mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 210, "7.Tf_100mL_15min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 240, "8.Tf_500mL_15min", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorTiming == 2)
						{
							DrawFillRect(2, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "1.100mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 60, "2.500mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 90, "3.Tf_100mL_5min", &fontArial14_B, WHITE, BLACK);
							DrawFillRect(Pos, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 120, "4.Tf_500mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 120, 320, 14, WHITE);
							Pos = DrawStringA(5, 150, "5.Tf_100mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 180, "6.Tf_500mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 210, "7.Tf_100mL_15min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 240, "8.Tf_500mL_15min", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorTiming == 3)
						{
							DrawFillRect(2, 120, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "1.100mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 60, "2.500mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 90, "3.Tf_100mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 120, "4.Tf_500mL_5min", &fontArial14_B, WHITE, BLACK);
							DrawFillRect(2, Pos, 320, 14, WHITE);
							Pos = DrawStringA(5, 150, "5.Tf_100mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 180, "6.Tf_500mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 210, "7.Tf_100mL_15min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 240, "8.Tf_500mL_15min", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorTiming == 4)
						{
							DrawFillRect(2, 150, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "1.100mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 60, "2.500mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 90, "3.Tf_100mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 120, "4.Tf_500mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 120, 320, 14, WHITE);
							Pos = DrawStringA(5, 150, "5.Tf_100mL_10min", &fontArial14_B, WHITE, BLACK);
							Pos = DrawStringA(5, 180, "6.Tf_500mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 210, "7.Tf_100mL_15min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 240, "8.Tf_500mL_15min", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorTiming == 5)
						{
							DrawFillRect(2, 180, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "1.100mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 60, "2.500mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 90, "3.Tf_100mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 120, "4.Tf_500mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, Pos, 320, 14, WHITE);
							Pos = DrawStringA(5, 150, "5.Tf_100mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 180, "6.Tf_500mL_10min", &fontArial14_B, WHITE, BLACK);
							Pos = DrawStringA(5, 210, "7.Tf_100mL_15min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 240, "8.Tf_500mL_15min", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorTiming == 6)
						{
							DrawFillRect(2, 210, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "1.100mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 60, "2.500mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 90, "3.Tf_100mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 120, "4.Tf_500mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(2, Pos, 320, 14, WHITE);
							Pos = DrawStringA(5, 150, "5.Tf_100mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 180, "6.Tf_500mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 210, "7.Tf_100mL_15min", &fontArial14_B, WHITE, BLACK);
							Pos = DrawStringA(5, 240, "8.Tf_500mL_15min", &fontArial14_B, BLACK, WHITE);
						}
						if (cursorTiming == 7)
						{
							DrawFillRect(2, 210, 320, 14, WHITE);
							Pos = DrawStringA(5, 30, "2.500mL primary", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 30, 320, 14, WHITE);
							Pos = DrawStringA(5, 60, "3.Tf_100mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 60, 320, 14, WHITE);
							Pos = DrawStringA(5, 90, "4.Tf_500mL_5min", &fontArial14_B, BLACK, WHITE);
							DrawFillRect(Pos, 90, 320, 14, WHITE);
							Pos = DrawStringA(5, 120, "5.Tf_100mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 150, "6.Tf_500mL_10min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 180, "7.Tf_100mL_15min", &fontArial14_B, BLACK, WHITE);
							Pos = DrawStringA(5, 210, "8.Tf_500mL_15min", &fontArial14_B, WHITE, BLACK);
						}
					}
				}
				key = KEY_NULL;
				/*-------------------------------------------------------------------------------------------------------------------
				execute which Item Selected
				---------------------------------------------------------------------------------------------------------------------*/
				/************************************************************Max time 100mL Primary*************************************/
				if (cursorTiming == 0)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%.2d:%.2d", (timeRejecton100Primary / 60000), (timeRejecton100Primary % 60000) / 1000);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					PosT100m = DrawStringA(10, 30, "Max time 100mL Primary:", &fontArial10_B, NAVY, WHITE);
					DrawStringA(220, 180, "Min:00:05", &fontArial14_B, BLACK, WHITE);
					DrawStringA(220, 205, "Max:05:00", &fontArial14_B, BLACK, WHITE);
					key = KEY_NULL;
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TimingMenu;
						}
						if ((key == KEY_UP) && (timeRejecton100Primary < 300000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejecton100Primary = timeRejecton100Primary + 1000;
						}
						if ((key == KEY_DOWN) && (timeRejecton100Primary > 5000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejecton100Primary = timeRejecton100Primary - 1000;
						}
						if (RefreshDisplay == 1)
						{
							sprintf(LCDString, "%.2d:%.2d", (timeRejecton100Primary / 60000), (timeRejecton100Primary % 60000) / 1000);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							RefreshDisplay = 0;
						}
					}
					key = KEY_NULL;
				}
				/************************************************************Max time 500mL Primary*************************************/
				if (cursorTiming == 1)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%.2d:%.2d", (timeRejecton500Primary / 60000), (timeRejecton500Primary % 60000) / 1000);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					PosT100m = DrawStringA(10, 30, "Max time 500mL Primary:", &fontArial10_B, NAVY, WHITE);
					DrawStringA(220, 180, "Min:00:05", &fontArial14_B, BLACK, WHITE);
					DrawStringA(220, 205, "Max:10:00", &fontArial14_B, BLACK, WHITE);
					key = KEY_NULL;
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TimingMenu;
						}
						if ((key == KEY_UP) && (timeRejecton500Primary < 300000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejecton500Primary = timeRejecton500Primary + 1000;
						}
						if ((key == KEY_DOWN) && (timeRejecton500Primary > 5000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejecton500Primary = timeRejecton500Primary - 1000;
						}
						if (RefreshDisplay == 1)
						{
							sprintf(LCDString, "%.2d:%.2d", (timeRejecton500Primary / 60000), (timeRejecton500Primary % 60000) / 1000);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							RefreshDisplay = 0;
						}
					}
					key = KEY_NULL;
				}
				/************************************************************Tf 100ml 5min************************************/
				if (cursorTiming == 2)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_5_100mL / 60000), (timeRejectonTf_5_100mL % 60000) / 1000);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					PosT100m = DrawStringA(10, 30, "Max time Tf when 100mL:", &fontArial10_B, NAVY, WHITE);
					DrawStringA(220, 180, "Min:00:05", &fontArial14_B, BLACK, WHITE);
					DrawStringA(220, 205, "Max:10:00", &fontArial14_B, BLACK, WHITE);
					key = KEY_NULL;
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TimingMenu;
						}
						if ((key == KEY_UP) && (timeRejectonTf_5_100mL < 600000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_5_100mL = timeRejectonTf_5_100mL + 1000;
						}
						if ((key == KEY_DOWN) && (timeRejectonTf_5_100mL > 5000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_5_100mL = timeRejectonTf_5_100mL - 1000;
						}
						if (RefreshDisplay == 1)
						{
							sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_5_100mL / 60000), (timeRejectonTf_5_100mL % 60000) / 1000);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							RefreshDisplay = 0;
						}
					}
					key = KEY_NULL;
				}
				/************************************************************Tf 500ml 5min*************************************/
				if (cursorTiming == 3)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_5_500mL / 60000), (timeRejectonTf_5_500mL % 60000) / 1000);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					PosT100m = DrawStringA(10, 30, "Max time 500mL for Tf:", &fontArial10_B, NAVY, WHITE);
					DrawStringA(220, 180, "Min:00:05", &fontArial14_B, BLACK, WHITE);
					DrawStringA(220, 205, "Max:10:00", &fontArial14_B, BLACK, WHITE);
					key = KEY_NULL;
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TimingMenu;
						}
						if ((key == KEY_UP) && (timeRejectonTf_5_500mL < 600000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_5_500mL = timeRejectonTf_5_500mL + 1000;
						}
						if ((key == KEY_DOWN) && (timeRejectonTf_5_500mL > 5000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_5_500mL = timeRejectonTf_5_500mL - 1000;
						}
						if (RefreshDisplay == 1)
						{
							sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_5_500mL / 60000), (timeRejectonTf_5_500mL % 60000) / 1000);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							RefreshDisplay = 0;
						}
					}
					key = KEY_NULL;
				}
				/************************************************************Tf 100ml 10min*************************************/
				if (cursorTiming == 4)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_10_100mL / 60000), (timeRejectonTf_10_100mL % 60000) / 1000);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					PosT100m = DrawStringA(10, 30, "Max Tf 100mL for 10min mesurement", &fontArial10_B, NAVY, WHITE);
					DrawStringA(220, 180, "Min:00:05", &fontArial14_B, BLACK, WHITE);
					DrawStringA(220, 205, "Max:15:00", &fontArial14_B, BLACK, WHITE);
					key = KEY_NULL;
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TimingMenu;
						}
						if ((key == KEY_UP) && (timeRejectonTf_10_100mL < 900000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_10_100mL = timeRejectonTf_10_100mL + 1000;
						}
						if ((key == KEY_DOWN) && (timeRejectonTf_10_100mL > 5000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_10_100mL = timeRejectonTf_10_100mL - 1000;
						}
						if (RefreshDisplay == 1)
						{
							sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_10_100mL / 60000), (timeRejectonTf_10_100mL % 60000) / 1000);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							RefreshDisplay = 0;
						}
					}
					key = KEY_NULL;
				}
				/************************************************************Tf 500ml 10min*************************************/
				if (cursorTiming == 5)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_10_500mL / 60000), (timeRejectonTf_10_500mL % 60000) / 1000);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					PosT100m = DrawStringA(10, 30, "Max Tf 500mL for 10min mesurement", &fontArial10_B, NAVY, WHITE);
					DrawStringA(220, 180, "Min:00:05", &fontArial14_B, BLACK, WHITE);
					DrawStringA(220, 205, "Max:15:00", &fontArial14_B, BLACK, WHITE);
					key = KEY_NULL;
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TimingMenu;
						}
						if ((key == KEY_UP) && (timeRejectonTf_10_500mL < 900000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_10_500mL = timeRejectonTf_10_500mL + 1000;
						}
						if ((key == KEY_DOWN) && (timeRejectonTf_10_500mL > 5000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_10_500mL = timeRejectonTf_10_500mL - 1000;
						}
						if (RefreshDisplay == 1)
						{
							sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_10_500mL / 60000), (timeRejectonTf_10_500mL % 60000) / 1000);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							RefreshDisplay = 0;
						}
					}
					key = KEY_NULL;
				}
				/************************************************************Tf 100ml 15min*************************************/
				if (cursorTiming == 6)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_15_100mL / 60000), (timeRejectonTf_15_100mL % 60000) / 1000);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					PosT100m = DrawStringA(10, 30, "Max Tf 100mL for 15min mesurement", &fontArial10_B, NAVY, WHITE);
					DrawStringA(220, 180, "Min:00:05", &fontArial14_B, BLACK, WHITE);
					DrawStringA(220, 205, "Max:25:00", &fontArial14_B, BLACK, WHITE);
					key = KEY_NULL;
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TimingMenu;
						}
						if ((key == KEY_UP) && (timeRejectonTf_15_100mL < 1500000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_15_100mL = timeRejectonTf_15_100mL + 1000;
						}
						if ((key == KEY_DOWN) && (timeRejectonTf_10_100mL > 5000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_15_100mL = timeRejectonTf_15_100mL - 1000;
						}
						if (RefreshDisplay == 1)
						{
							sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_15_100mL / 60000), (timeRejectonTf_15_100mL % 60000) / 1000);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							RefreshDisplay = 0;
						}
					}
					key = KEY_NULL;
				}
				/************************************************************Tf 500ml 15min*************************************/
				if (cursorTiming == 7)
				{
					DrawFillRect(3, 20, 300, 220, WHITE);
					sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_15_500mL / 60000), (timeRejectonTf_15_500mL % 60000) / 1000);
					DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
					PosT100m = DrawStringA(10, 30, "Max Tf 500mL for 15min mesurement", &fontArial10_B, NAVY, WHITE);
					DrawStringA(220, 180, "Min:00:05", &fontArial14_B, BLACK, WHITE);
					DrawStringA(220, 205, "Max:25:00", &fontArial14_B, BLACK, WHITE);
					key = KEY_NULL;
					while (1)
					{
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							goto TimingMenu;
						}
						if ((key == KEY_UP) && (timeRejectonTf_15_500mL < 1500000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_15_500mL = timeRejectonTf_15_500mL + 1000;
						}
						if ((key == KEY_DOWN) && (timeRejectonTf_15_500mL > 5000))
						{
							key = KEY_NULL;
							RefreshDisplay = 1;
							timeRejectonTf_15_500mL = timeRejectonTf_15_500mL - 1000;
						}
						if (RefreshDisplay == 1)
						{
							sprintf(LCDString, "%.2d:%.2d", (timeRejectonTf_15_500mL / 60000), (timeRejectonTf_15_500mL % 60000) / 1000);
							DrawStringA(5, 180, LCDString, &fontArial48_B, WHITE, BLACK);
							RefreshDisplay = 0;
						}
					}
				}
				key = KEY_NULL;

				break;
			case SETTING_PASSWORD: /*---------------------------------------------PASSWORD----------------------------*/
				currentPassword = 0;
				DrawFillRect(1, 20, 320, 240, WHITE);
				DrawTaskbar("Password", 0, 0);
				Pos = DrawStringA(5, 30, "Enter New password", &fontArial14_B, BLACK, WHITE);
				sprintf(LCDString, "%.4d", currentPassword);
				Pos = DrawStringA(100, 90, LCDString, &fontArial48_B, WHITE, BLACK);
				while (1)
				{
					if (key == KEY_STOP)
					{
						key = KEY_NULL;
						goto RootMenu;
					}
					if (key == KEY_START)
					{
						key = KEY_NULL;
						break;
					}
					if ((key == KEY_UP) && (currentPassword < 1000))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						currentPassword++;
					}
					if ((key == KEY_DOWN) && (currentPassword >= 1))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						currentPassword--;
					}
					if (RefreshDisplay == 1)
					{
						RefreshDisplay = 0;
						sprintf(LCDString, "%.4d", currentPassword);
						Pos = DrawStringA(100, 90, LCDString, &fontArial48_B, WHITE, BLACK);
					}
				}
				DrawFillRect(20, 150, 300, 20, WHITE);
				while (1)
				{

					Pos = DrawStringA(20, 150, "Do you want to save password?", &fontArial14_B, BLACK, WHITE);
					if (key == KEY_STOP)
					{
						key = KEY_NULL;
						break;
					}
					if (key == KEY_START)
					{
						key = KEY_NULL;
						DrawFillRect(20, 150, 300, 20, WHITE);
						Pos = DrawStringA(20, 150, "Password Changed!!!", &fontArial14_B, DARKGREEN, WHITE);
						passwordSaved = currentPassword;
						HAL_Delay(1000);
						goto PASSWORD;
					}
				}

				HAL_Delay(1000);
				goto RootMenu;
				break;
			case SETTING_RESET_FACTORY: /*----------------------------------------------factory setting-------------------------------------*/
				DrawFillRect(1, 20, 320, 240, WHITE);
				currentPassword = 0;
				DrawStringA(1, 30, "If you want factory setting enter", &fontArial14_B, NAVY, WHITE);
				DrawStringA(1, 60, "password then press START", &fontArial14_B, NAVY, WHITE);
				sprintf(LCDString, "%.4d", currentPassword);
				Pos = DrawStringA(100, 90, LCDString, &fontArial48_B, WHITE, BLACK);
				while (1)
				{
					if (key == KEY_STOP)
					{
						key = KEY_NULL;
						goto RootMenu;
					}
					if (key == KEY_START)
					{
						key = KEY_NULL;
						break;
					}
					if ((key == KEY_UP) && (currentPassword < 1000))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						currentPassword++;
					}
					if ((key == KEY_DOWN) && (currentPassword >= 1))
					{
						key = KEY_NULL;
						RefreshDisplay = 1;
						currentPassword--;
					}
					if (RefreshDisplay == 1)
					{
						RefreshDisplay = 0;
						sprintf(LCDString, "%.4d", currentPassword);
						Pos = DrawStringA(100, 90, LCDString, &fontArial48_B, WHITE, BLACK);
					}
				}
				DrawFillRect(20, 150, 300, 20, WHITE);
				if (currentPassword == passwordSaved)
				{
					while (1)
					{

						Pos = DrawStringA(20, 150, "Do you want to Reset factory?", &fontArial14_B, BLACK, WHITE);
						if (key == KEY_STOP)
						{
							key = KEY_NULL;
							break;
						}
						if (key == KEY_START)
						{
							key = KEY_NULL;
							DrawFillRect(20, 150, 300, 20, WHITE);
							Pos = DrawStringA(20, 150, "Please wait to rest System ....", &fontArial14_B, NAVY, WHITE);
							/*-----------------------------------------------
							default Setting
							-------------------------------------------------*/
							timeRejecton000Primary = 0;
							timeRejecton100Primary = 120000;
							timeRejecton500Primary = 300000;
							timeRejectonTf_5_100mL = 300000;
							timeRejectonTf_10_100mL = 300000;
							timeRejectonTf_15_100mL = 300000;
							timeRejectonTf_5_500mL = 300000;
							timeRejectonTf_10_500mL = 300000;
							timeRejectonTf_15_500mL = 300000;

							passwordSaved = 12;
							DeltaDesierTemprature = 5;
							PressureSetPoint = 30.0;
							DesigerDeltaPressure = 5;
							PresurePrimary = 0.0;
							/*-------------------------------------------------------write data to EEPROM--------------------**/
							uint32_t values[NUMBER_DATA_SAVE_SETTING / 4];
							values[0] = timeRejecton100Primary;
							values[1] = timeRejecton500Primary;
							values[2] = timeRejectonTf_5_100mL;
							values[3] = timeRejectonTf_10_100mL;
							values[4] = timeRejectonTf_15_100mL;
							values[5] = timeRejectonTf_5_500mL;
							values[6] = timeRejectonTf_10_500mL;
							values[7] = timeRejectonTf_15_500mL;
							values[8] = PresurePrimary * 10;
							values[9] = PressureSetPoint * 10;
							values[10] = DesigerDeltaPressure * 10;
							values[11] = DeltaDesierTemprature * 10;

							values[14] = passwordSaved;
							exitPress = 0;
							memcpy(saveMenuDataSetting, values, NUMBER_DATA_SAVE_SETTING / 4 * sizeof(uint32_t));
							EEPROM_Write(PAGE_SAVE_SETTING, 0, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);

							/*--------------------------------------------------------------------------------------------------*/
							HAL_Delay(2000);
							NVIC_SystemReset();
						}
					}
				}
				else
				{
					DrawFillRect(20, 150, 300, 20, WHITE);
					Pos = DrawStringA(20, 150, "incorrect password!!!", &fontArial14_B, RED, WHITE);
					HAL_Delay(1000);
					goto RootMenu;
				}

				break;
			default:
				break;
			}

			key = KEY_NULL;
		}
	}
	ClearScreen(WHITE);
	TapDisplay(BYPASS_TAB);
	DrawFillRect(104, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Press START key to ", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 40, "start process...", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 90, "Press DOWN key to", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 115, "history", &fontArial14_B, NAVY, WHITE);
}
/****************************************************************************************/
/////////////*This function pass water to empty pipe for bypassing Water*///////////////
/***************************************************************************************/
void ByPass(void)
{
	DrawFillRect(105, 10, 214, 232, WHITE);
	DrainValve(GPIO_PIN_SET);
	do
	{
		DrawStringA(105, 15, "Drain Tank....", &fontArial14_B, NAVY, WHITE);
		HAL_Delay(50);
	} while (HAL_GPIO_ReadPin(LEVEL000_GPIO_Port, LEVEL000_Pin) == 0);

	HAL_Delay(2000);
	key = KEY_NULL;
	DrainValve(GPIO_PIN_RESET);
Start_BAYPASS:
	DrawFillRect(105, 10, 214, 232, WHITE);
	_LowLevelPass = REJECT;
	_100mLLevelPass = REJECT;
	_HighLevelPass = REJECT;

	DrawStringA(105, 15, "Please select bypass", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 40, "Time then press ", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 65, "START", &fontArial14_B, NAVY, WHITE);
	/*****************************************************
		Setting time for drain Water
		******************************************************/
	key = KEY_NULL;
	// DrawFillRect(105, 10, 214, 232, WHITE);
	Pos = DrawStringA(105, 90, "Baypass Time: ", &fontArial14_B, NAVY, WHITE);
	sprintf(LCDString, "%d", ByPassTime);
	DrawStringA(Pos, 90, LCDString, &fontArial14_B, RED, WHITE);
	Pos = DrawStringA(Pos + 22, 90, "min", &fontArial14_B, NAVY, WHITE);
	do
	{
		if ((key == KEY_UP) && (ByPassTime < 30))
		{
			key = KEY_NULL;
			RefreshDisplay = 1;
			ByPassTime++;
		}
		if ((key == KEY_DOWN) && (ByPassTime > 2))
		{
			key = KEY_NULL;
			RefreshDisplay = 1;
			ByPassTime--;
		}
		if (RefreshDisplay == 1)
		{
			RefreshDisplay = 0;
			Pos = DrawStringA(105, 90, "Baypass Time: ", &fontArial14_B, NAVY, WHITE);
			DrawFillRect(Pos, 90, 22, 14, WHITE);
			sprintf(LCDString, "%d", ByPassTime);
			DrawStringA(Pos, 90, LCDString, &fontArial14_B, RED, WHITE);
			Pos = DrawStringA(Pos + 22, 90, "min", &fontArial14_B, NAVY, WHITE);
		}

		if (key == KEY_STOP)
		{
			key = NULL;
			goto Start_BAYPASS;
		}

	} while (key != KEY_START);

	DrawFillRect(105, 10, 214, 232, WHITE);
	PUMP_SPPED(99);
	WaterValve(GPIO_PIN_SET);
	DrainValve(GPIO_PIN_SET);
	ByPassTimer = ByPassTime * 600;
	/******************************************************************
	start drain water
	*******************************************************************/
	Clear_Buffer((uint8_t *)RecordedPresure, 120);	  // clear buffer to recorrd last 30 index  presuree
	Clear_Buffer((uint8_t *)RecordedTemprature, 120); // clear buffer to recorrd last 30 index  temprature
	indexTMP = 0;									  // Index for circular buffer
	indexPRE = 0;									  // Index for circular buffer
	countTMP = 0;									  // Track number of values stored
	countPRE = 0;									  // Track number of values stored

	while (ByPassTimer)
	{
		float PreSum = 0;
		PreAve = 0;
		uint8_t counter = 10;
		if (key == KEY_STOP)
		{
			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			BUZZER(GPIO_PIN_RESET);
			goto Start_BAYPASS;
		}
		while (counter)
		{
			counter--;
			ByPassTimer--;
			HAL_Delay(100);
			PreSum += Read_Presure((uint16_t *)ADC1_Buffer); // Accumulate sum
		}
		PreAve = PreSum / 10;
		printf("Second .... %d\r\n", ByPassTimer);
		sprintf(LCDString, " %.2d:%.2d ", (ByPassTimer / 600), (ByPassTimer / 10) % 60);
		Pos = DrawStringA(140, 80, LCDString, &fontArial36_B, NAVY, WHITE);
		TempWater = Read_TemperatureWater((uint16_t *)ADC1_Buffer); // read water temprature
																	// Presure=
		printf("TempWater = %.2f\r\n", TempWater);
		printf("Presure = %.2f\r\n", PreAve);
		if (check_tolerance_presure(PreAve))
		{
			printf("Tolerance exceeded 3. Breaking loop!\n");
			DrawStringA(105, 140, "Press STOP to exit", &fontArial10_B, NAVY, WHITE);
			erroeCode = EXCEEDE_PRESURE;
			break;
		}
		if (check_tolerance_temprature(TempWater))
		{
			printf("Tolerance exceeded 3. Breaking loop!\n");
			DrawStringA(105, 140, "Press STOP to exit", &fontArial10_B, NAVY, WHITE);
			erroeCode = EXCEEDE_TEMPRATURE;
			break;
		}
		/*Check water*/
		// if (adcPresureAvg < 700){
		if (PreAve < PresurePrimary)
		{

			BUZZER(GPIO_PIN_SET);
			DrawStringA(105, 120, "No Water in pipe line", &fontArial10_B, RED, WHITE);
		}
		else
		{
			BUZZER(GPIO_PIN_RESET);
			DrawStringA(105, 120, "                             ", &fontArial14_B, RED, WHITE);
		}
		counter = 10;
		/*for check level we nedd 1 minutes*/
		if (ByPassTimer == 600)
		{
			break;
		}
	}
	/********CHECK ERROR****************************************************/
	if (erroeCode == EXCEEDE_PRESURE)
	{
		DrawStringA(105, 190, "Check filter", &fontArial10_B, RED, WHITE);
		do
		{
			BUZZER(GPIO_PIN_SET);
			DrawStringA(105, 120, "Error water presure !!!", &fontArial10_B, RED, WHITE);
			HAL_Delay(500);
			BUZZER(GPIO_PIN_RESET); // buzzer off
			DrawStringA(105, 120, "                                       ", &fontArial14_B, NAVY, WHITE);
			HAL_Delay(500);
		} while (key != KEY_STOP);
		erroeCode = NO_ERROR;
		BUZZER(GPIO_PIN_RESET); // buzzer off
		DrawStringA(105, 15, "                           ", &fontArial20_B, NAVY, WHITE);
		DrawStringA(140, 80, "                  ", &fontArial36_B, NAVY, WHITE);
		DrawStringA(105, 140, "                          ", &fontArial14_B, NAVY, WHITE);
		DrawStringA(105, 120, "                                       ", &fontArial14_B, NAVY, WHITE);
		// PUMP(GPIO_PIN_RESET);
		PUMP_SPPED(0);
		WaterValve(GPIO_PIN_RESET);
		HAL_Delay(2000);
		DrainValve(GPIO_PIN_RESET);
		goto Start_BAYPASS;
	}
	else if (erroeCode == EXCEEDE_TEMPRATURE)
	{

		do
		{
			BUZZER(GPIO_PIN_SET);
			DrawStringA(105, 120, "Error  water Temprature !!!", &fontArial10_B, RED, WHITE);
			HAL_Delay(500);
			BUZZER(GPIO_PIN_RESET); // buzzer off
			DrawStringA(105, 120, "                                             ", &fontArial14_B, NAVY, WHITE);
			HAL_Delay(500);
		} while (key != KEY_STOP);
		key = KEY_NULL;
		erroeCode = NO_ERROR;
		BUZZER(GPIO_PIN_RESET); // buzzer off
		DrawStringA(105, 15, "                           ", &fontArial20_B, NAVY, WHITE);
		DrawStringA(140, 80, "                  ", &fontArial36_B, NAVY, WHITE);
		DrawStringA(105, 140, "                          ", &fontArial14_B, NAVY, WHITE);
		DrawStringA(105, 120, "                                          ", &fontArial14_B, NAVY, WHITE);
		// PUMP(GPIO_PIN_RESET);
		PUMP_SPPED(0);
		WaterValve(GPIO_PIN_RESET);
		HAL_Delay(2000);
		DrainValve(GPIO_PIN_RESET);
		goto Start_BAYPASS;
	}
	else
	{
	}

	/******** END CHECK ERROR****************************************************/
	////////////////////////////*All things good*//////////////////////////////////
	key = KEY_NULL;
	DrainValve(GPIO_PIN_RESET); // close //
	counter = 0;
	////////////////////////////////////Check level///////////////////////////////////////////////////////////////////////////////////
	Pos = DrawStringA(105, 150, "Checking Level Switch ...", &fontArial14_B, NAVY, WHITE);
	do
	{
		if (counter >= 500) ////500
		{
			counter = 0;
			DrawFillRect(130, 80, 150, 47, WHITE);
			sprintf(LCDString, " %.2d:%.2d ", (ByPassTimer / 600), (ByPassTimer / 10) % 60);
			Pos = DrawStringA(140, 80, LCDString, &fontArial36_B, NAVY, WHITE);
			if (key == KEY_STOP)
			{
				key = KEY_NULL;
				RefreshDisplay = 1;
				goto Start_BAYPASS;
			}
			if (ByPassTimer == 0)
			{
				break;
			}

			ByPassTimer = ByPassTimer - 10;
		}
		if (levelSwitch == LEVEL_LOW)
		{
			// levelSwitch=LEVEL_NULL;
			_LowLevelPass = PASS;
		}
		if (levelSwitch == LEVEL_MEDUIM)
		{
			// levelSwitch=LEVEL_NULL;
			_100mLLevelPass = PASS;
		}
		if (levelSwitch == LEVEL_HIGH)
		{
			// levelSwitch=LEVEL_NULL;
			_HighLevelPass = PASS;
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			break;
		}
		HAL_Delay(1);
		counter++;
		/*Check water*/
		/*if (adcPresureAvg < 700)
		{
			BUZZER(GPIO_PIN_SET);
			DrawStringA(105, 120, "No Water in pipe line", &fontArial10_B, RED, WHITE);
		}
		else
		{
			BUZZER(GPIO_PIN_RESET);
			DrawStringA(105, 120, "                             ", &fontArial14_B, RED, WHITE);
		}*/
	} while (key != KEY_STOP);
	if (_LowLevelPass != PASS || _100mLLevelPass != PASS || _HighLevelPass != PASS && ByPassTimer == 0)
	{
		levelSwitch = LEVEL_NULL;
		Pos = DrawStringA(105, 150, "                                   ", &fontArial20_B, NAVY, WHITE);
		Pos = DrawStringA(105, 160, "Time Over ,Housing filter Error ...", &fontArial10_B, RED, WHITE);
	}
	DrainValve(GPIO_PIN_SET);
	// PUMP(GPIO_PIN_RESET);
	PUMP_SPPED(0);
	WaterValve(GPIO_PIN_RESET);
	Pos = DrawStringA(105, 150, "                                                ", &fontArial20_B, NAVY, WHITE);

	if (_LowLevelPass == PASS)
	{
		levelSwitch = LEVEL_NULL;
		_LowLevelPass = REJECT;
		Pos = DrawStringA(105, 180, "Level Switch Zero OK ...", &fontArial10_B, DARKGREEN, WHITE);
	}
	else if (_LowLevelPass == REJECT)
	{
		levelSwitch = LEVEL_NULL;
		Pos = DrawStringA(105, 180, "Level Switch Zero fault ...", &fontArial10_B, RED, WHITE);
	}
	if (_100mLLevelPass == PASS)
	{
		levelSwitch = LEVEL_NULL;
		_100mLLevelPass = REJECT;
		Pos = DrawStringA(105, 192, "Level Switch 100mL OK ...", &fontArial10_B, DARKGREEN, WHITE);
	}
	else if (_100mLLevelPass == REJECT)
	{
		levelSwitch = LEVEL_NULL;
		Pos = DrawStringA(105, 192, "Level Switch 100mL fault ...", &fontArial10_B, RED, WHITE);
	}
	if (_HighLevelPass == PASS)
	{
		levelSwitch = LEVEL_NULL;
		_HighLevelPass = REJECT;
		Pos = DrawStringA(105, 204, "Level Switch 500mL OK ...", &fontArial10_B, DARKGREEN, WHITE);
	}
	else if (_HighLevelPass == REJECT)
	{
		levelSwitch = LEVEL_NULL;
		Pos = DrawStringA(105, 204, "Level Switch 500mL fault ...", &fontArial10_B, RED, WHITE);
	}

	do
	{
		DrawStringA(105, 15, "Drain Tank....", &fontArial14_B, NAVY, WHITE);
		HAL_Delay(50);
	} while (HAL_GPIO_ReadPin(LEVEL000_GPIO_Port, LEVEL000_Pin) == 0);
	if (key == KEY_STOP)
	{
		key = KEY_NULL;
		DrawFillRect(105, 10, 214, 232, WHITE);
		// PUMP(GPIO_PIN_RESET);
		PUMP_SPPED(0);
		WaterValve(GPIO_PIN_RESET);
		DrainValve(GPIO_PIN_SET);
		goto Start_BAYPASS;
	}
	HAL_Delay(2000);
	key = KEY_NULL;
	DrainValve(GPIO_PIN_RESET);
	HAL_Delay(1000);
	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "                           ", &fontArial20_B, NAVY, WHITE);
	DrawStringA(140, 80, "                  ", &fontArial36_B, NAVY, WHITE);
	DrawStringA(105, 140, "                          ", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 120, "                                          ", &fontArial14_B, NAVY, WHITE);
	menu = PRESSURE_TAB;
	TapDisplay(PRESSURE_TAB);
	key = KEY_NULL;
	PresuureSetting();
}

void TapDisplay(uint8_t Tab)
{
	DrawFillRect(1, 4, 99, 45, NAVY);
	DrawStringA(5, 18, "Bypass", &fontArial14_B, WHITE, NAVY);

	DrawFillRect(1, 51, 99, 45, NAVY);
	DrawStringA(5, 64, "Pressure", &fontArial14_B, WHITE, NAVY);

	DrawFillRect(1, 98, 99, 45, NAVY);
	DrawStringA(5, 111, "Timing", &fontArial14_B, WHITE, NAVY);

	DrawFillRect(1, 145, 99, 45, NAVY);
	DrawStringA(5, 158, "Result", &fontArial14_B, WHITE, NAVY);

	DrawFillRect(1, 192, 99, 45, NAVY);
	DrawStringA(5, 205, "History", &fontArial14_B, WHITE, NAVY);
	if (Tab == BYPASS_TAB)
	{

		DrawRectangle(1, 4, 101, 45, NAVY);
		DrawRectangle(101, 4, 218, 232, NAVY);
		DrawFillRect(2, 5, 102, 43, WHITE);
		DrawStringA(5, 18, "Bypass", &fontArial14_B, NAVY, WHITE);
	}
	else if (Tab == PRESSURE_TAB)
	{

		DrawRectangle(1, 51, 101, 45, NAVY);
		DrawRectangle(101, 4, 218, 232, NAVY);
		DrawFillRect(2, 52, 102, 43, WHITE);
		DrawStringA(5, 64, "Pressure", &fontArial14_B, NAVY, WHITE);
	}
	else if (Tab == TIMING_TAB)
	{
		DrawRectangle(1, 98, 101, 45, NAVY);
		DrawRectangle(101, 4, 218, 232, NAVY);
		DrawFillRect(2, 99, 102, 43, WHITE);
		DrawStringA(5, 111, "Timing", &fontArial14_B, NAVY, WHITE);
	}
	else if (Tab == RESULT_TAB)
	{
		DrawRectangle(1, 145, 101, 45, NAVY);
		DrawRectangle(101, 4, 218, 232, NAVY);
		DrawFillRect(2, 146, 102, 43, WHITE);
		DrawStringA(5, 158, "Result", &fontArial14_B, NAVY, WHITE);
	}
	else if (Tab == HISTORY_TAB)
	{
		DrawRectangle(1, 192, 101, 45, NAVY);
		DrawRectangle(101, 4, 218, 232, NAVY);
		DrawFillRect(2, 193, 102, 43, WHITE);
		DrawStringA(5, 205, "History", &fontArial14_B, NAVY, WHITE);
	}
}
void PresuureSetting(void)
{

Start_PRESSURE:
	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Please insert filter", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 40, "then press START:", &fontArial14_B, NAVY, WHITE);
	/******************************************************************
	start Presure
	*******************************************************************/
	Clear_Buffer((uint8_t *)RecordedPresure, 120);	  // clear buffer to recorrd last 30 index  presuree
	Clear_Buffer((uint8_t *)RecordedTemprature, 120); // clear buffer to recorrd last 30 index  temprature
	indexTMP = 0;									  // Index for circular buffer
	indexPRE = 0;									  // Index for circular buffer
	countTMP = 0;									  // Track number of values stored
	countPRE = 0;									  // Track number of values stored
	do
	{
		if (key == KEY_STOP)
		{
			key = KEY_NULL;
			RefreshDisplay = 1;
			goto Start_PRESSURE;
		}
		HAL_Delay(10);
	} while (key != KEY_START);
	key = KEY_NULL;
	DrawFillRect(105, 10, 214, 232, WHITE);
	Pos = DrawStringA(105, 15, "Adj Pressure to", &fontArial14_B, NAVY, WHITE);
	sprintf(LCDString, "%2.2f PSI", PressureSetPoint);
	DrawStringA(105, 40, LCDString, &fontArial14_B, NAVY, WHITE);
	key = KEY_NULL;
	// PUMP(GPIO_PIN_SET);
	PUMP_SPPED(99);
	WaterValve(GPIO_PIN_SET);
	DrainValve(GPIO_PIN_SET);
	float PreSum = 0, PreAve = 0;
	counter = 0;

	while (1)
	{

		Presure = Read_Presure((uint16_t *)ADC1_Buffer);
		TempWater = Read_TemperatureWater((uint16_t *)ADC1_Buffer); // read water temprature
		DelatPressure = fabs(PressureSave - Read_Presure((uint16_t *)ADC1_Buffer));
		if (DelatPressure >= 0.1)
		{
			PressureSave = Read_Presure((uint16_t *)ADC1_Buffer);
			DrawFillRect(150, 80, 150, 47, WHITE);
			sprintf(LCDString, " %.1f ", Read_Presure((uint16_t *)ADC1_Buffer));
			Pos = DrawStringA(130, 80, LCDString, &fontArial36_B, NAVY, WHITE);
			DrawStringA(Pos + 5, 100, "PSI", &fontArial14_B, NAVY, WHITE);
		}
		printf("Counter : %d \r\n", counter);
		if ((DelatPressure < 0.1) && (Read_Presure((uint16_t *)ADC1_Buffer) > PressureSetPoint - 1.0) && (Read_Presure((uint16_t *)ADC1_Buffer) < PressureSetPoint + 1.0))
		{
			PressureSave = Read_Presure((uint16_t *)ADC1_Buffer);
			DrawFillRect(105, 150, 210, 50, WHITE);
			DrawStringA(105, 150, "Pressur stable", &fontArial14_B, DARKGREEN, WHITE);
			counter++;
			if (counter > 20)
			{
				DrawStringA(105, 175, "Press START to next.", &fontArial14_B, DARKGREEN, WHITE);
				break;
			}
		}
		if ((DelatPressure < 0.1) && ((Read_Presure((uint16_t *)ADC1_Buffer) < PressureSetPoint - 1.0) || (Read_Presure((uint16_t *)ADC1_Buffer) > PressureSetPoint + 1.0)))
		{
			PressureSave = Read_Presure((uint16_t *)ADC1_Buffer);
			DrawFillRect(105, 150, 210, 50, WHITE);
			DrawStringA(105, 150, " Pressur not standard ", &fontArial14_B, WHITE, RED);
			//	DrawStringA(105, 175, "Press Enter to next.", &fontArial14_B, NAVY, WHITE);
			counter = 0;
			counter++;
			if (counter > 20)
			{
				DrawStringA(105, 175, "Press START to next.", &fontArial14_B, DARKGREEN, WHITE);
				break;
			}
		}
		if (key == KEY_STOP)
		{
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			HAL_Delay(2000);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_RESET);
			key = KEY_NULL;
			RefreshDisplay = 1;
			goto Start_PRESSURE;
		}
		HAL_Delay(1000);
	}
	key = KEY_NULL;
	// PUMP(GPIO_PIN_RESET);
	PUMP_SPPED(0);
	HAL_Delay(2000);
	WaterValve(GPIO_PIN_RESET);
	DrainValve(GPIO_PIN_RESET);
	key = KEY_NULL;
	menu = TIMING_TAB;
	TapDisplay(TIMING_TAB);
	////////////////////////////////////////check and execute errors message///////////////////////////////////////////////
	if (erroeCode == EXCEEDE_PRESURE)
	{
		do
		{
			BUZZER(GPIO_PIN_SET);
			DrawStringA(105, 120, "Error input water presure !!!", &fontArial10_B, RED, WHITE);
			HAL_Delay(500);
			BUZZER(GPIO_PIN_RESET); // buzzer off
			DrawStringA(105, 120, "                                       ", &fontArial14_B, NAVY, WHITE);
			HAL_Delay(500);
		} while (key != KEY_STOP);
		erroeCode = NO_ERROR;
		BUZZER(GPIO_PIN_RESET); // buzzer off
		DrawStringA(105, 15, "                           ", &fontArial20_B, NAVY, WHITE);
		DrawStringA(140, 80, "                  ", &fontArial36_B, NAVY, WHITE);
		DrawStringA(105, 140, "                          ", &fontArial14_B, NAVY, WHITE);
		DrawStringA(105, 120, "                                       ", &fontArial14_B, NAVY, WHITE);
		goto Start_PRESSURE;
	}

	if (erroeCode == EXCEEDE_TEMPRATURE)
	{
		do
		{
			BUZZER(GPIO_PIN_SET);
			DrawStringA(105, 120, "Error input water Temprature !!!", &fontArial10_B, RED, WHITE);
			HAL_Delay(500);
			BUZZER(GPIO_PIN_RESET); // buzzer off
			DrawStringA(105, 120, "                                             ", &fontArial14_B, NAVY, WHITE);
			HAL_Delay(500);
		} while (key != KEY_STOP);
		key = KEY_NULL;
		erroeCode = NO_ERROR;
		BUZZER(GPIO_PIN_RESET); // buzzer off
		DrawStringA(105, 15, "                           ", &fontArial20_B, NAVY, WHITE);
		DrawStringA(140, 80, "                  ", &fontArial36_B, NAVY, WHITE);
		DrawStringA(105, 140, "                          ", &fontArial14_B, NAVY, WHITE);
		DrawStringA(105, 120, "                                          ", &fontArial14_B, NAVY, WHITE);
		goto Start_PRESSURE;
	}
	//////////////////////////////////////////////End Check errors////////////////////////////////////////
}

/*------------------------------------------------------------------------------------
for which saved parameter data we want to show
--------------------------------------------------------------------------------*/
void ShowHistory(int NumberOfSampl)
{

	Clear_Buffer(saveSDI, 32);
	EEPROM_Read(NumberOfSampl, 0, saveSDI, 32);
	float recoveredValues[8];
	memcpy(recoveredValues, saveSDI, 32);
	SDI_5 = recoveredValues[0];
	SDI_10 = recoveredValues[1];
	SDI_15 = recoveredValues[2];
	SDI_5_100mL = recoveredValues[3];
	SDI_10_100mL = recoveredValues[4];
	SDI_15_100mL = recoveredValues[5];

	intervalPressure = recoveredValues[6];
	intervalTemprature = recoveredValues[7];

	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Load from Memory", &fontArial14_B, GREEN, WHITE);
	sprintf(LCDString, "Sample Number : %d", NumberOfSampl);
	DrawStringA(105, 40, LCDString, &fontArial14_B, NAVY, WHITE);

	Pos = DrawStringA(105, 70, "Del_P:", &fontArial14_B, BLACK, WHITE);
	sprintf(LCDString, "%1.1f", intervalPressure);
	Pos = DrawStringA(Pos, 70, LCDString, &fontArial14_B, NAVY, WHITE);

	DrawStringA(Pos + 20, 70, "Del_T:", &fontArial14_B, BLACK, WHITE);
	sprintf(LCDString, "%1.1f", intervalTemprature);
	Pos = DrawStringA(Pos + 84, 70, LCDString, &fontArial14_B, NAVY, WHITE);

	DrawStringA(176, 103, "500mL   100mL", &fontArial14_B, BLACK, WHITE);
	DrawFastHLine(105, 123, 215, NAVY);

	DrawStringA(176, 103, "500mL   100mL", &fontArial14_B, BLACK, WHITE);
	DrawFastHLine(105, 123, 215, NAVY);

	sprintf(LCDString, "SDI-5      %2.2f        %2.2f", SDI_5, SDI_5_100mL);
	DrawStringA(105, 130, LCDString, &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 130, LCDString, &fontArial14_B, NAVY, WHITE);
	DrawFastHLine(105, 153, 215, NAVY);
	sprintf(LCDString, "SDI-10    %2.2f        %2.2f", SDI_10, SDI_10_100mL);
	DrawStringA(105, 160, LCDString, &fontArial14_B, DARKGREEN, WHITE);
	DrawFastHLine(105, 183, 215, NAVY);
	sprintf(LCDString, "SDI-15    %2.2f        %2.2f", SDI_15, SDI_15_100mL);
	DrawStringA(105, 190, LCDString, &fontArial14_B, RED, WHITE);
	DrawFastHLine(105, 213, 215, NAVY);

	DrawFastVLine(104, 123, 90, NAVY);
	DrawFastVLine(170, 123, 90, NAVY);
	DrawFastVLine(250, 123, 90, NAVY);
}

/*----------------------------------------------------------------------------------------
for Show Data Save
---------------------------------------------------------------------------------------------------------*/
void History_Stage()
{
	ClearScreen(WHITE);
	DrawFillRect(105, 10, 214, 232, WHITE);
	TapDisplay(HISTORY_TAB);
	/*****************************************************
		Setting time for drain Water
		******************************************************/
	DrawStringA(105, 15, "Press START ", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 40, "and select Key Up Down...", &fontArial14_B, NAVY, WHITE);

HISTORY_STAGE:
	if (key == KEY_START)
	{
		key = KEY_NULL;
		DrawFillRect(105, 10, 214, 232, WHITE);
		whichSample = numberOfsampleSave;
		ShowHistory(whichSample);
		while (1)
		{
			if ((key == KEY_UP) && (whichSample < 10))
			{
				key = KEY_NULL;
				RefreshDisplay = 1;
				whichSample++;
			}
			if ((key == KEY_DOWN) && (whichSample > 1))
			{
				key = KEY_NULL;
				RefreshDisplay = 1;
				whichSample--;
			}
			if (RefreshDisplay == 1)
			{
				RefreshDisplay = 0;
				ShowHistory(whichSample);
			}

			if (key == KEY_STOP)
			{
				break;
			}
		}
		ClearScreen(WHITE);
		TapDisplay(BYPASS_TAB);
		DrawFillRect(104, 10, 214, 232, WHITE);
		DrawStringA(105, 15, "Press START key to", &fontArial14_B, NAVY, WHITE);
		DrawStringA(105, 40, "start process...", &fontArial14_B, NAVY, WHITE);
		DrawStringA(105, 90, "Press DOWN key to", &fontArial14_B, NAVY, WHITE);
		DrawStringA(105, 115, "history", &fontArial14_B, NAVY, WHITE);
		menu = BYPASS_TAB;
	}
	else
	{
		goto HISTORY_STAGE;
	}
	key = NULL;
}

/*--------------------------------------------------------------------------------------------------------------
calculate parameter for SDI
-------------------------------------------------------------------------------------------------------------------------------*/
void ResultStage(void)
{
	ClearScreen(WHITE);
	TapDisplay(RESULT_TAB);
	DrawFillRect(105, 10, 214, 232, WHITE);

	Pos = DrawStringA(105, 70, "Del_P:", &fontArial14_B, BLACK, WHITE);
	sprintf(LCDString, "%1.1f", intervalPressure);
	Pos = DrawStringA(Pos, 70, LCDString, &fontArial14_B, NAVY, WHITE);

	DrawStringA(Pos + 20, 70, "Del_T:", &fontArial14_B, BLACK, WHITE);
	sprintf(LCDString, "%1.1f", intervalTemprature);
	Pos = DrawStringA(Pos + 84, 70, LCDString, &fontArial14_B, NAVY, WHITE);

	DrawStringA(176, 103, "500mL   100mL", &fontArial14_B, BLACK, WHITE);
	DrawFastHLine(105, 123, 215, NAVY);

	sprintf(LCDString, "SDI-5      %2.2f        %2.2f", SDI_5, SDI_5_100mL);
	DrawStringA(105, 130, LCDString, &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 130, LCDString, &fontArial14_B, NAVY, WHITE);
	DrawFastHLine(105, 153, 215, NAVY);
	sprintf(LCDString, "SDI-10    %2.2f        %2.2f", SDI_10, SDI_10_100mL);
	DrawStringA(105, 160, LCDString, &fontArial14_B, DARKGREEN, WHITE);
	DrawFastHLine(105, 183, 215, NAVY);
	sprintf(LCDString, "SDI-15    %2.2f        %2.2f", SDI_15, SDI_15_100mL);
	DrawStringA(105, 190, LCDString, &fontArial14_B, RED, WHITE);
	DrawFastHLine(105, 213, 215, NAVY);

	DrawFastVLine(104, 123, 90, NAVY);
	DrawFastVLine(170, 123, 90, NAVY);
	DrawFastVLine(250, 123, 90, NAVY);

	DrawStringA(105, 15, "For saving data", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 40, "Press START :", &fontArial14_B, NAVY, WHITE);
	do
	{
		__NOP();

	} while (key != KEY_START);
	key = KEY_NULL;
	float values[8] = {SDI_5, SDI_10, SDI_15, SDI_5_100mL, SDI_10_100mL, SDI_15_100mL, intervalPressure, intervalTemprature};
	numberOfsampleSave = 0;
	numberOfsampleSave = EEPROM_Read_NUM(0, 0);
	numberOfsampleSave++;
	if (numberOfsampleSave > 10)
	{
		numberOfsampleSave = 11;
		numberOfsampleSave--;
		EEPROM_Write_NUM(0, 0, numberOfsampleSave);
		for (int i = 1; i < numberOfsampleSave; i++)
		{ // for delete last Data page0 for Number; we must save from page 1
			Clear_Buffer(saveSDI, 32);
			EEPROM_Read(i + 1, 0, saveSDI, 32);
			EEPROM_Write(i, 0, saveSDI, 32);
		}
		memcpy(saveSDI, values, 8 * sizeof(float));
		EEPROM_Write(numberOfsampleSave, 0, saveSDI, 32);
	}
	else
	{
		EEPROM_Write_NUM(0, 0, numberOfsampleSave);
		memcpy(saveSDI, values, 8 * sizeof(float));
		EEPROM_Write(numberOfsampleSave, 0, saveSDI, 32);
	}

	Clear_Buffer(saveSDI, 8 * sizeof(float));
	EEPROM_Read(numberOfsampleSave, 0, saveSDI, 8 * sizeof(float));
	float recoveredValues[8];
	memcpy(recoveredValues, saveSDI, 8 * sizeof(float));

	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Saved SDI data", &fontArial14_B, GREEN, WHITE);
	sprintf(LCDString, "Sample Number : %d", numberOfsampleSave);
	DrawStringA(105, 40, LCDString, &fontArial14_B, NAVY, WHITE);
	HAL_Delay(1000);
	DrawStringA(105, 65, "Press Start for next :", &fontArial14_B, NAVY, WHITE);
	do
	{
		__NOP();
		if (key == KEY_STOP)
		{
			break;
		}

	} while (key != KEY_START);
	key = KEY_NULL;
	menu = HISTORY_TAB;
	History_Stage();
}
/*--------------------------------------------------------------------------------------------------------------
measure Time for Drain Water in 500ml 100ml 5Min 10Min 15Min
----------------------------------------------------------------------------------------------------------------*/
void TimingStage(void)
{

	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Press START to set ", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);

Start_TIMING:

	/*-------------------------------------------------------------------------------------
		//New filter and start Timer
	-----------------------------------------------------------------------------------------*/
	ClearScreen(WHITE);
	TapDisplay(TIMING_TAB);
	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Please insert new filter", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 40, "then press START:", &fontArial14_B, NAVY, WHITE);
	do
	{
		__NOP();
	} while (key != KEY_START);
	/*Drain Water*/
	DrainValve(GPIO_PIN_SET);
	DrawFillRect(105, 10, 214, 232, WHITE);
	do
	{
		DrawStringA(105, 15, "Drain Tank....", &fontArial14_B, NAVY, WHITE);
		HAL_Delay(50);
	} while (HAL_GPIO_ReadPin(LEVEL000_GPIO_Port, LEVEL000_Pin) == 0);

	HAL_Delay(2000);
	CounterRefresh = 1;
	key = KEY_NULL;
	timeMS = 0;
	timeMS_Total = 0;
	Time_Second = 0;
	Time_Minute = 0;
	CounterRefresh = 0;
	timeLevel_000_primary = 0;
	timeLevel_100_primary = 0;
	timeLevel_500_primary = 0;
	timeLevel_000_5 = 0;
	timeLevel_100_5 = 0;
	timeLevel_500_5 = 0;
	timeLevel_000_10 = 0;
	timeLevel_100_10 = 0;
	timeLevel_500_10 = 0;
	timeLevel_000_15 = 0;
	timeLevel_100_15 = 0;
	timeLevel_500_15 = 0;
	Tf_15_100mL=0;
	Tf_10_100mL=0;
	Tf_5_100mL=0;
	Tf_15=0;
	Tf_5=0;
	Tf_10=0;
	Ti=0;
	
	_LowLevelPass = REJECT;
	_100mLLevelPass = REJECT;
	_HighLevelPass = REJECT;
	levelSwitch = LEVEL_NULL;
	levelWater = WATER_EMPETY;
	DrainValve(GPIO_PIN_RESET);
	TimeCounter = 0;
	TimeCount = 0;
	// PUMP(GPIO_PIN_SET);
	PUMP_SPPED(99);
	WaterValve(GPIO_PIN_SET);
	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Timing....", &fontArial14_B, NAVY, WHITE);

	/*-------------------------------------------------------------------------------------
				//Process Timing
	-----------------------------------------------------------------------------------------*/

Measure_TIMING:

	while (1)
	{

		if (key == KEY_STOP)
		{
			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Timing....", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}
		/*-------------------------------------------------------------------------------------
			//Check time setting for 100ML and 500ML
		-----------------------------------------------------------------------------------------*/
		if (timeMS_Total > timeRejecton100Primary && _100mLLevelPass == REJECT)
		{
			key = KEY_NULL;
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			do
			{
				BUZZER(GPIO_PIN_SET);
				DrawFillRect(105, 110, 214, 70, WHITE);
				DrawStringA(105, 120, "The turbidity water is very high!!!", &fontArial10_B, RED, WHITE);
				DrawStringA(105, 145, "Time 100ML primary ", &fontArial14_B, NAVY, WHITE);
				DrawStringA(105, 165, "greater than what you SET", &fontArial10_B, NAVY, WHITE);
				HAL_Delay(500);
				BUZZER(GPIO_PIN_RESET); // buzzer off
				DrawStringA(105, 120, "                                           ", &fontArial14_B, NAVY, WHITE);
				HAL_Delay(500);
			} while (key != KEY_STOP);

			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Press START to set ", &fontArial14_B, NAVY, WHITE);
			DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}
		if (timeMS_Total > timeRejecton500Primary && _HighLevelPass == REJECT)
		{
			key = KEY_NULL;
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			do
			{
				BUZZER(GPIO_PIN_SET);
				DrawFillRect(105, 110, 214, 70, WHITE);
				DrawStringA(105, 120, "The turbidity water is very high!!!", &fontArial10_B, RED, WHITE);
				DrawStringA(105, 145, "Time 500ML primary ", &fontArial14_B, NAVY, WHITE);
				DrawStringA(105, 165, "greater than what you SET", &fontArial10_B, NAVY, WHITE);
				HAL_Delay(500);
				BUZZER(GPIO_PIN_RESET); // buzzer off
				DrawStringA(105, 120, "                                           ", &fontArial14_B, NAVY, WHITE);
				HAL_Delay(500);
			} while (key != KEY_STOP);

			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Press START to set ", &fontArial14_B, NAVY, WHITE);
			DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}
		/*-------------------------------------------------------------------------------------
		//Time  tf_5 tf_10 tf_15  for Rejection
	-----------------------------------------------------------------------------------------*/
		if (Tf_5 > timeRejectonTf_5_500mL)
		{
			key = KEY_NULL;
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			do
			{
				BUZZER(GPIO_PIN_SET);
				DrawFillRect(105, 110, 214, 70, WHITE);
				DrawStringA(105, 120, "The turbidity water is very high!!!", &fontArial10_B, RED, WHITE);
				DrawStringA(105, 145, "Tf_5 500mL", &fontArial14_B, NAVY, WHITE);
				DrawStringA(105, 165, "greater than what you SET", &fontArial10_B, NAVY, WHITE);
				HAL_Delay(500);
				BUZZER(GPIO_PIN_RESET); // buzzer off
				DrawStringA(105, 120, "                                           ", &fontArial14_B, NAVY, WHITE);
				HAL_Delay(500);
			} while (key != KEY_STOP);

			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Please go to MENU ", &fontArial14_B, NAVY, WHITE);
			DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}
		if (Tf_10 > timeRejectonTf_10_500mL)
		{
			key = KEY_NULL;
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			do
			{
				BUZZER(GPIO_PIN_SET);
				DrawFillRect(105, 110, 214, 70, WHITE);
				DrawStringA(105, 120, "The turbidity water is very high!!!", &fontArial10_B, RED, WHITE);
				DrawStringA(105, 145, "tf_10 500mL ", &fontArial14_B, NAVY, WHITE);
				DrawStringA(105, 165, "greater than what you SET", &fontArial10_B, NAVY, WHITE);
				HAL_Delay(500);
				BUZZER(GPIO_PIN_RESET); // buzzer off
				DrawStringA(105, 120, "                                           ", &fontArial14_B, NAVY, WHITE);
				HAL_Delay(500);
			} while (key != KEY_STOP);

			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Please go to MENU ", &fontArial14_B, NAVY, WHITE);
			DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}

		if (Tf_15 > timeRejectonTf_15_500mL)
		{
			key = KEY_NULL;
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			do
			{
				BUZZER(GPIO_PIN_SET);
				DrawFillRect(105, 110, 214, 70, WHITE);
				DrawStringA(105, 120, "The turbidity water is very high!!!", &fontArial10_B, RED, WHITE);
				DrawStringA(105, 145, "Tf_15 500mL", &fontArial14_B, NAVY, WHITE);
				DrawStringA(105, 165, "greater than what you SET", &fontArial10_B, NAVY, WHITE);
				HAL_Delay(500);
				BUZZER(GPIO_PIN_RESET); // buzzer off
				DrawStringA(105, 120, "                                           ", &fontArial14_B, NAVY, WHITE);
				HAL_Delay(500);
			} while (key != KEY_STOP);

			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Please go to MENU ", &fontArial14_B, NAVY, WHITE);
			DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}

		/*-------------------------------------------------------------------------------------
		//Time  tf_5 tf_10 tf_15  for 100mL Rejection
	-----------------------------------------------------------------------------------------*/
		if (Tf_5_100mL > timeRejectonTf_5_100mL)
		{
			key = KEY_NULL;
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			do
			{
				BUZZER(GPIO_PIN_SET);
				DrawFillRect(105, 110, 214, 70, WHITE);
				DrawStringA(105, 120, "The turbidity water is very high!!!", &fontArial10_B, RED, WHITE);
				DrawStringA(105, 145, "Tf_5_100mL", &fontArial14_B, NAVY, WHITE);
				DrawStringA(105, 165, "greater than what you SET", &fontArial10_B, NAVY, WHITE);
				HAL_Delay(500);
				BUZZER(GPIO_PIN_RESET); // buzzer off
				DrawStringA(105, 120, "                                           ", &fontArial14_B, NAVY, WHITE);
				HAL_Delay(500);
			} while (key != KEY_STOP);

			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Please go to MENU ", &fontArial14_B, NAVY, WHITE);
			DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}
		if (Tf_10_100mL > timeRejectonTf_10_100mL)
		{
			key = KEY_NULL;
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			do
			{
				BUZZER(GPIO_PIN_SET);
				DrawFillRect(105, 110, 214, 70, WHITE);
				DrawStringA(105, 120, "The turbidity water is very high!!!", &fontArial10_B, RED, WHITE);
				DrawStringA(105, 145, "Tf_10_100mL ", &fontArial14_B, NAVY, WHITE);
				DrawStringA(105, 165, "greater than what you SET", &fontArial10_B, NAVY, WHITE);
				HAL_Delay(500);
				BUZZER(GPIO_PIN_RESET); // buzzer off
				DrawStringA(105, 120, "                                           ", &fontArial14_B, NAVY, WHITE);
				HAL_Delay(500);
			} while (key != KEY_STOP);

			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Please go to MENU ", &fontArial14_B, NAVY, WHITE);
			DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}

		if (Tf_15_100mL > timeRejectonTf_15_100mL)
		{
			key = KEY_NULL;
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			do
			{
				BUZZER(GPIO_PIN_SET);
				DrawFillRect(105, 110, 214, 70, WHITE);
				DrawStringA(105, 120, "The turbidity water is very high!!!", &fontArial10_B, RED, WHITE);
				DrawStringA(105, 145, "Tf_15_100mL", &fontArial14_B, NAVY, WHITE);
				DrawStringA(105, 165, "greater than what you SET", &fontArial10_B, NAVY, WHITE);
				HAL_Delay(500);
				BUZZER(GPIO_PIN_RESET); // buzzer off
				DrawStringA(105, 120, "                                           ", &fontArial14_B, NAVY, WHITE);
				HAL_Delay(500);
			} while (key != KEY_STOP);

			key = KEY_NULL;
			DrawFillRect(105, 10, 214, 232, WHITE);
			// PUMP(GPIO_PIN_RESET);
			PUMP_SPPED(0);
			WaterValve(GPIO_PIN_RESET);
			DrainValve(GPIO_PIN_SET);
			DrawFillRect(105, 10, 214, 232, WHITE);
			DrawStringA(105, 15, "Please go to MENU ", &fontArial14_B, NAVY, WHITE);
			DrawStringA(105, 40, "Time for rejection...", &fontArial14_B, NAVY, WHITE);
			goto Start_TIMING;
		}
		/*-------------------------------------------------------------------------------------------------------------*/
		startSysTick = HAL_GetTick();

		Time_mSecond = timeMS;
		if (Time_mSecond > 999)
		{
			Time_Second++;
			timeMS = 0;
		}
		if (Time_Second > 59)
		{
			Time_Minute++;
			Time_Second = 0;
		}

		/*-------------------------------------------------------------------------------------
			//choice Time stage
		-----------------------------------------------------------------------------------------*/
		if (timeMS_Total < 10000)
		{
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);

			levelWater = WATER_EMPETY;
			levelSwitch = LEVEL_NULL;
			stageTime = TIME_PRIMAY;
			DrainValve(GPIO_PIN_RESET); /*close Drain Valve*/
		}
		if (timeMS_Total > 300000 && timeMS_Total < 302000)
		{
			DrawFillRect(105, 180, 214, 70, WHITE);
			DrawFillRect(105, 120, 214, 50, WHITE);
			levelWater = WATER_EMPETY;
			levelSwitch = LEVEL_NULL;
			stageTime = TIME_5;
			DrainValve(GPIO_PIN_RESET); /*close Drain Valve*/
		}
		if (timeMS_Total > 600000 && timeMS_Total < 602000)
		{
			Pos = DrawStringA(105, 180, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                      ", &fontArial14_B, DARKGREEN, WHITE);
			levelWater = WATER_EMPETY;
			levelSwitch = LEVEL_NULL;
			stageTime = TIME_10;
			DrainValve(GPIO_PIN_RESET); /*close Drain Valve*/
		}
		if (timeMS_Total > 900000 && timeMS_Total < 902000)
		{
			Pos = DrawStringA(105, 180, "                        ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 192, "                        ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 204, "                        ", &fontArial14_B, DARKGREEN, WHITE);
			Pos = DrawStringA(105, 216, "                        ", &fontArial14_B, DARKGREEN, WHITE);
			levelWater = WATER_EMPETY;
			levelSwitch = LEVEL_NULL;
			stageTime = TIME_15;
			DrainValve(GPIO_PIN_RESET); /*close Drain Valve*/
		}
		/*---------------------------------------------------------------------------
		Show Temprature and Presure
		-----------------------------------------------------------------------*/
		CounterRefresh++;
		sprintf(LCDString, "%.2d:%.2d:%d", (timeMS_Total / 60000), (timeMS_Total % 60000) / 1000, (timeMS_Total % 1000) / 100);
		Pos = DrawStringA(105, 60, LCDString, &fontArial20_B, NAVY, WHITE);
		startSysTick = HAL_GetTick();
		if (CounterRefresh > 100)
		{ /// for refresh Display ;//wait for 1 second
			CounterRefresh = 0;
			PressureAux = Read_Presure((uint16_t *)ADC1_Buffer);
			if (PressureAux > 99.9)
				PressureAux = 99.9;
			sprintf(LCDString, "P:%2.1f ", PressureAux);
			Pos = DrawStringA(230, 15, LCDString, &fontArial14_B, NAVY, WHITE);

			DrawStringA(230 + Pos + 30, 20, "PSI", &fontArial10_B, NAVY, WHITE);
			TempAux = Read_TemperatureWater((uint16_t *)ADC1_Buffer);
			if (TempAux > 99.9)
				TempAux = 99.9;
			sprintf(LCDString, "T:%2.1f", TempAux);
			Pos = DrawStringA(230, 40, LCDString, &fontArial14_B, NAVY, WHITE);
			DrawStringA(230 + Pos + 30, 45, "c", &fontArial10_B, NAVY, WHITE);
		}
		endSysTick = HAL_GetTick();
		milliSecond = endSysTick - startSysTick;
		/*---------------------------------------------------------------------------
		record primary and last Temprature and Presure
		-----------------------------------------------------------------------*/
		if (timeMS_Total < 10000)
		{
			primPresure = PressureAux;
			primTemprature = TempAux;
		}

		/*---------------------------------------------------------------------------
		execute according Time STAGE
		-----------------------------------------------------------------------*/
		/*---------------------Measure Time primery---------------------------------------------------- */
		if (stageTime == TIME_PRIMAY)
		{
			Pos = DrawStringA(105, 140, "Stage Primary: ", &fontArial14_B, BLACK, WHITE);
			if (levelSwitch == LEVEL_LOW && levelWater == WATER_EMPETY)
			{
				levelWater = WATER_MEDUIM;
				levelSwitch = LEVEL_NULL;
				_LowLevelPass = PASS;
				timeLevel_000_primary = timeMS_Total;
				sprintf(LCDString, "T    0:  %.2d:%.2d:%.3d", (timeLevel_000_primary / 60000), (timeLevel_000_primary % 60000) / 1000, timeLevel_000_primary % 1000);
				// Pos = DrawStringA(105, 180, LCDString, &fontArial10_B, DARKGREEN, WHITE);
			}
			if (levelSwitch == LEVEL_MEDUIM && levelWater == WATER_MEDUIM)
			{
				levelWater = WATER_FULL;
				levelSwitch = LEVEL_NULL;
				_100mLLevelPass = PASS;
				timeLevel_100_primary = timeMS_Total;
				Ti_100mL = timeLevel_100_primary - timeLevel_000_primary;
				sprintf(LCDString, "T100:  %.2d:%.2d:%.3d", (timeLevel_100_primary / 60000), (timeLevel_100_primary % 60000) / 1000, timeLevel_100_primary % 1000);
				// Pos = DrawStringA(105, 192, LCDString, &fontArial10_B, DARKGREEN, WHITE);
			}
			if (levelSwitch == LEVEL_HIGH && levelWater == WATER_FULL)
			{
				levelWater = WATER_FULL;
				levelSwitch = LEVEL_NULL;
				_HighLevelPass = PASS;
				timeLevel_500_primary = timeMS_Total;
				timeCertain = timeMS_Total;
				sprintf(LCDString, "T500:  %.2d:%.2d:%.3d", (timeLevel_500_primary / 60000), (timeLevel_500_primary % 60000) / 1000, timeLevel_500_primary % 1000);
				// Pos = DrawStringA(105, 204, LCDString, &fontArial10_B, DARKGREEN, WHITE);
				DrainValve(GPIO_PIN_SET); /*Open Drain Valve*/
			}

			if ((int32_t)timeMS_Total - (int32_t)timeCertain > 10000)
			{ // for certain tank is empety*/
				levelWater = WATER_NULL;
				levelSwitch = LEVEL_NULL;
				//_LowLevelPass = REJECT;
				//_100mLLevelPass = REJECT;
				//_HighLevelPass = REJECT;
				timeCertain = 2000000;
				Ti = timeLevel_500_primary - timeLevel_000_primary;
				sprintf(LCDString, "Ti:  %.2d:%.2d:%.3d       ", (Ti / 60000), (Ti % 60000) / 1000, Ti % 1000);
				// Pos = DrawStringA(105, 216, LCDString, &fontArial10_B, DARKGREEN, WHITE);
				goto Measure_TIMING;
			}
		}
		/*----------------------------------execute Time 5 minute -------------------------------------------------------------------*/
		if (stageTime == TIME_5)
		{
			Pos = DrawStringA(105, 140, "Stage 5MIN: ", &fontArial14_B, BLACK, WHITE);
			if (levelSwitch == LEVEL_LOW && levelWater == WATER_EMPETY)
			{
				levelWater = WATER_MEDUIM;
				levelSwitch = LEVEL_NULL;
				_LowLevelPass = PASS;
				timeLevel_000_5 = timeMS_Total;
				sprintf(LCDString, "T    0:  %.2d:%.2d:%.3d ", (timeLevel_000_5 / 60000), (timeLevel_000_5 % 60000) / 1000, timeLevel_000_5 % 1000);
				// Pos = DrawStringA(105, 180, LCDString, &fontArial10_B, DARKGREEN, WHITE);
			}
			if (levelSwitch == LEVEL_MEDUIM && levelWater == WATER_MEDUIM)
			{
				levelWater = WATER_FULL;
				levelSwitch = LEVEL_NULL;
				_100mLLevelPass = PASS;
				timeLevel_100_5 = timeMS_Total;
				sprintf(LCDString, "T100:  %.2d:%.2d:%.3d  ", (timeLevel_100_5 / 60000), (timeLevel_100_5 % 60000) / 1000, timeLevel_100_5 % 1000);
				// Pos = DrawStringA(105, 192, LCDString, &fontArial10_B, DARKGREEN, WHITE);
			}
			if (levelSwitch == LEVEL_HIGH && levelWater == WATER_FULL)
			{
				levelWater = WATER_FULL;
				levelSwitch = LEVEL_NULL;
				_HighLevelPass = PASS;
				timeLevel_500_5 = timeMS_Total;
				timeCertain = timeMS_Total;
				sprintf(LCDString, "T500:  %.2d:%.2d:%.3d  ", (timeLevel_500_5 / 60000), (timeLevel_500_5 % 60000) / 1000, timeLevel_500_5 % 1000);
				// Pos = DrawStringA(105, 204, LCDString, &fontArial10_B, DARKGREEN, WHITE);
				DrainValve(GPIO_PIN_SET); /*Open Drain Valve*/
			}
			if ((int32_t)timeMS_Total - (int32_t)timeCertain > 10000)
			{ // for certain tank is empety*/
				levelWater = WATER_NULL;
				levelSwitch = LEVEL_NULL;
				//_LowLevelPass = REJECT;
				//_100mLLevelPass = REJECT;
				//_HighLevelPass = REJECT;
				timeCertain = 2000000;
				Tf_5 = timeLevel_500_5 - timeLevel_000_5;
				Tf_5_100mL = timeLevel_100_5 - timeLevel_000_5;
				sprintf(LCDString, "Tf5 :  %.2d:%.2d:%.3d  ", (Tf_5 / 60000), (Tf_5 % 60000) / 1000, Tf_5 % 1000);
				// Pos = DrawStringA(105, 216, LCDString, &fontArial10_B, DARKGREEN, WHITE);
				P_30_5 = (1 - ((float)Ti / (float)Tf_5)) * 100;
				sprintf(LCDString, "P_30_5:%2.2f", P_30_5);
				// Pos = DrawStringA(230, 180, LCDString, &fontArial10_B, NAVY, WHITE);
				goto Measure_TIMING;
			}
		}
		/*----------------------------------execute Time 10 minute -------------------------------------------------------------------*/
		if (stageTime == TIME_10)
		{
			Pos = DrawStringA(105, 140, "Stage 10MIN: ", &fontArial14_B, BLACK, WHITE);
			if (levelSwitch == LEVEL_LOW && levelWater == WATER_EMPETY)
			{
				levelWater = WATER_MEDUIM;
				levelSwitch = LEVEL_NULL;
				_LowLevelPass = PASS;
				timeLevel_000_10 = timeMS_Total;
				sprintf(LCDString, "T    0:  %.2d:%.2d:%.3d ", (timeLevel_000_10 / 60000), (timeLevel_000_10 % 60000) / 1000, timeLevel_000_10 % 1000);
				// Pos = DrawStringA(105, 180, LCDString, &fontArial10_B, DARKGREEN, WHITE);
			}
			if (levelSwitch == LEVEL_MEDUIM && levelWater == WATER_MEDUIM)
			{
				levelWater = WATER_FULL;
				levelSwitch = LEVEL_NULL;
				_100mLLevelPass = PASS;
				timeLevel_100_10 = timeMS_Total;
				sprintf(LCDString, "T100:  %.2d:%.2d:%.3d   ", (timeLevel_100_10 / 60000), (timeLevel_100_10 % 60000) / 1000, timeLevel_100_10 % 1000);
				// Pos = DrawStringA(105, 192, LCDString, &fontArial10_B, DARKGREEN, WHITE);
			}
			if (levelSwitch == LEVEL_HIGH && levelWater == WATER_FULL)
			{
				levelWater = WATER_FULL;
				levelSwitch = LEVEL_NULL;
				_HighLevelPass = PASS;
				timeLevel_500_10 = timeMS_Total;
				timeCertain = timeMS_Total;
				sprintf(LCDString, "T500:  %.2d:%.2d:%.3d  ", (timeLevel_500_10 / 60000), (timeLevel_500_10 % 60000) / 1000, timeLevel_500_10 % 1000);
				// Pos = DrawStringA(105, 204, LCDString, &fontArial10_B, DARKGREEN, WHITE);
				DrainValve(GPIO_PIN_SET); /*Open Drain Valve*/
			}
			if ((int32_t)timeMS_Total - (int32_t)timeCertain > 10000)
			{ // for certain tank is empety*/
				levelWater = WATER_NULL;
				levelSwitch = LEVEL_NULL;
				//_LowLevelPass = REJECT;
				//_100mLLevelPass = REJECT;
				//_HighLevelPass = REJECT;
				timeCertain = 2000000;
				Tf_10 = timeLevel_500_10 - timeLevel_000_10;
				Tf_10_100mL = timeLevel_100_10 - timeLevel_000_10;
				sprintf(LCDString, "Tf10 :  %.2d:%.2d:%.3d  ", (Tf_10 / 60000), (Tf_10 % 60000) / 1000, Tf_10 % 1000);
				// Pos = DrawStringA(105, 216, LCDString, &fontArial10_B, DARKGREEN, WHITE);
				P_30_10 = (1 - ((float)Ti / (float)Tf_10)) * 100;

				sprintf(LCDString, "P_30_10:%2.2f", P_30_10);
				// Pos = DrawStringA(230, 192, LCDString, &fontArial10_B, NAVY, WHITE);
				goto Measure_TIMING;
			}
		}

		/*----------------------------------execute Time 15 minute -------------------------------------------------------------------*/
		if (stageTime == TIME_15)
		{
			Pos = DrawStringA(105, 140, "Stage 15MIN: ", &fontArial14_B, BLACK, WHITE);
			if (levelSwitch == LEVEL_LOW && levelWater == WATER_EMPETY)
			{
				levelWater = WATER_MEDUIM;
				levelSwitch = LEVEL_NULL;
				_LowLevelPass = PASS;
				timeLevel_000_15 = timeMS_Total;
				sprintf(LCDString, "T    0:  %.2d:%.2d:%.3d  ", (timeLevel_000_15 / 60000), (timeLevel_000_15 % 60000) / 1000, timeLevel_000_15 % 1000);
				// Pos = DrawStringA(105, 180, LCDString, &fontArial10_B, DARKGREEN, WHITE);
			}
			if (levelSwitch == LEVEL_MEDUIM && levelWater == WATER_MEDUIM)
			{
				levelWater = WATER_FULL;
				levelSwitch = LEVEL_NULL;
				_100mLLevelPass = PASS;
				timeLevel_100_15 = timeMS_Total;
				sprintf(LCDString, "T100:  %.2d:%.2d:%.3d  ", (timeLevel_100_15 / 60000), (timeLevel_100_15 % 60000) / 1000, timeLevel_100_15 % 1000);
				// Pos = DrawStringA(105, 192, LCDString, &fontArial10_B, DARKGREEN, WHITE);
			}
			if (levelSwitch == LEVEL_HIGH && levelWater == WATER_FULL)
			{
				levelWater = WATER_FULL;
				levelSwitch = LEVEL_NULL;
				_HighLevelPass = PASS;
				timeLevel_500_15 = timeMS_Total;
				timeCertain = timeMS_Total;
				sprintf(LCDString, "T500:  %.2d:%.2d:%.3d  ", (timeLevel_500_15 / 60000), (timeLevel_500_15 % 60000) / 1000, timeLevel_500_15 % 1000);
				// Pos = DrawStringA(105, 204, LCDString, &fontArial10_B, DARKGREEN, WHITE);
				DrainValve(GPIO_PIN_SET); /*Open Drain Valve*/
			}
			if ((int32_t)timeMS_Total - (int32_t)timeCertain > 10000)
			{ // for certain tank is empety*/
				levelWater = WATER_NULL;
				levelSwitch = LEVEL_NULL;
				//_LowLevelPass = REJECT;
				//_100mLLevelPass = REJECT;
				//_HighLevelPass = REJECT;
				timeCertain = 2000000;
				Tf_15 = timeLevel_500_15 - timeLevel_000_15;
				Tf_15_100mL = timeLevel_100_15 - timeLevel_000_15;
				sprintf(LCDString, "Tf15 :  %.2d:%.2d:%.3d  ", (Tf_15 / 60000), (Tf_15 % 60000) / 1000, Tf_15 % 1000);
				// Pos = DrawStringA(105, 216, LCDString, &fontArial10_B, DARKGREEN, WHITE);
				P_30_15 = (1 - ((float)Ti / (float)Tf_15)) * 100;
				sprintf(LCDString, "P_30_15:%2.2f", P_30_15);
				// Pos = DrawStringA(230, 204, LCDString, &fontArial10_B, NAVY, WHITE);
				break;
			}
		}
	}

	// PUMP(GPIO_PIN_RESET);
	PUMP_SPPED(0);
	WaterValve(GPIO_PIN_RESET);
	DrainValve(GPIO_PIN_SET);
	do
	{
		DrawStringA(105, 15, "Drain Tank....", &fontArial14_B, NAVY, WHITE);
		HAL_Delay(50);
	} while (HAL_GPIO_ReadPin(LEVEL000_GPIO_Port, LEVEL000_Pin) == 0);
	HAL_Delay(2000);
	DrainValve(GPIO_PIN_RESET);

	/*-----------------------------------------------------------------------------------------------
according to 8.Calculation On PDF
--------------------------------------------------------------------------------------------------*/

	P_30_5_100mL = (1 - ((float)Ti_100mL / (float)Tf_5_100mL)) * 100;
	P_30_10_100mL = (1 - ((float)Ti_100mL / (float)Tf_10_100mL)) * 100;
	P_30_15_100mL = (1 - ((float)Ti_100mL / (float)Tf_15_100mL)) * 100;

	SDI_5_100mL = P_30_5_100mL / 5.0;
	SDI_10_100mL = P_30_10_100mL / 10.0;
	SDI_15_100mL = P_30_15_100mL / 15.0;

	SDI_5 = P_30_5 / 5.0;
	SDI_10 = P_30_10 / 10.0;
	SDI_15 = P_30_15 / 15.0;

	/*--------------------------------------------------------------------------
	Show P30% Errors
	--------------------------------------------------------------------------------*/
	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawFillRect(105, 10, 214, 60, WHITE);
	DrawStringA(105, 10, "Press START to Show", &fontArial14_B, NAVY, WHITE);
	DrawStringA(105, 28, "Result:", &fontArial14_B, NAVY, WHITE);
	if (P_30_5 > Max_P30)
	{
		sprintf(LCDString, "P30 exceed than 75,Not sutible for %dmin", (int)5);
		Pos = DrawStringA(105, 45, LCDString, &fontArial10_B, DARKGREEN, WHITE);
	}
	else if (P_30_10 > Max_P30)
	{
		sprintf(LCDString, "P30 exceed than 75,Not sutible for %dmin", (int)10);
		Pos = DrawStringA(105, 60, LCDString, &fontArial10_B, DARKGREEN, WHITE);
	}
	else if (P_30_15 > Max_P30)
	{
		sprintf(LCDString, "P30 exceed than 75,Not sutible for %dmin", (int)15);
		Pos = DrawStringA(105, 75, LCDString, &fontArial10_B, DARKGREEN, WHITE);
	}
	else
	{
		sprintf(LCDString, "P30 less %d",Max_P30);
		Pos = DrawStringA(105, 45, LCDString, &fontArial10_B, DARKGREEN, WHITE);
	}
	/*--------------------------------------------------------------------------
	Show SDI Errors
	--------------------------------------------------------------------------------*/

	if (SDI_5_100mL < 0)
	{
		sprintf(LCDString, "SDI_5_100mL negetive _Error");
		Pos = DrawStringA(105, 110, LCDString, &fontArial10_B, RED, WHITE);
	}
	if (SDI_10_100mL < 0)
	{
		sprintf(LCDString, "SDI_10_100mL negetive _Error");
		Pos = DrawStringA(105, 130, LCDString, &fontArial10_B, RED, WHITE);
	}
	if (SDI_15_100mL < 0)
	{
		sprintf(LCDString, "SDI_15_100mL negetive _Error");
		Pos = DrawStringA(105, 150, LCDString, &fontArial10_B, RED, WHITE);
	}
	if (SDI_5 < 0)
	{
		sprintf(LCDString, "SDI_5_500mL negetive _Error");
		Pos = DrawStringA(105, 170, LCDString, &fontArial10_B, RED, WHITE);
	}
	if (SDI_10 < 0)
	{
		sprintf(LCDString, "SDI_10_500mL negetive _Error");
		Pos = DrawStringA(105, 190, LCDString, &fontArial10_B, RED, WHITE);
	}
	if (SDI_15 < 0)
	{
		sprintf(LCDString, "SDI_15_500mL negetive _Error");
		Pos = DrawStringA(105, 210, LCDString, &fontArial10_B, RED, WHITE);
	}

	do
	{
		__NOP();
	} while (key != KEY_START);
	key = KEY_NULL;

	DrawFillRect(105, 10, 214, 232, WHITE);
	DrawStringA(105, 15, "Pay attention to the ", &fontArial14_B, NAVY, WHITE);
	sprintf(LCDString, "PRESSUR: %2.1f ", PressureAux);
	Pos = DrawStringA(105, 40, LCDString, &fontArial14_B, RED, WHITE);

	DrawStringA(105, 70, "Pay attention to the ", &fontArial14_B, NAVY, WHITE);
	sprintf(LCDString, "TEMPRATURE: %2.1f ", TempAux);
	Pos = DrawStringA(105, 95, LCDString, &fontArial14_B, RED, WHITE);

	intervalTemprature = TempAux - primTemprature;
	intervalPressure = PressureAux - primPresure;

	if (intervalPressure > DesigerDeltaPressure)
	{
		DrawStringA(105, 140, "The difference between the", &fontArial10_B, RED, WHITE);
		DrawStringA(105, 155, "initial and final pressure is high.", &fontArial10_B, RED, WHITE);
	}
	if (intervalTemprature > DeltaDesierTemprature)
	{
		DrawStringA(105, 180, "The difference between the", &fontArial10_B, RED, WHITE);
		DrawStringA(105, 195, "initial and final temprature high", &fontArial10_B, RED, WHITE);
	}

	do
	{
		__NOP();
	} while (key != KEY_START);
	key = KEY_NULL;

	menu = RESULT_TAB;
	TapDisplay(RESULT_TAB);
	ResultStage();
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	// if (firstTimeLoad<1){

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	MX_DMA_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_DMA_Init();
  MX_TIM7_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

	/*ForPWM backLight*/
	HAL_TIM_Base_Start(&htim1); // PWM backLight
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); // Enable complementary output
	TIM1->CCR2 = 10;							 // duty cucle  0=Max  minBrightness=99(if ARR=99)

	/*For PWM PUMP*/
	HAL_TIM_Base_Start(&htim2);
	// HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	// TIM2->CCR1=99;//duty cucle  99=Max  minSpeed =0(if ARR=99)

	BUZZER(GPIO_PIN_RESET); // buzzer off
	LCD_init();
	HAL_TIM_Base_Start(&htim3); // for capture ADC
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1_Buffer, CHANAL_ADC * NUM_VALUES);
	//menu = TIMING_TAB;TapDisplay(TIMING_TAB);
	// menu =RESULT_TAB;TapDisplay(RESULT_TAB);
	// ResultStage();
	/*---------------------------------RTC setting----------------------------------------*/
	// Set time
//sTime.Hours = 12;
//sTime.Minutes = 30;
//sTime.Seconds = 45;
//HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

// Set date
//sDate.WeekDay = RTC_WEEKDAY_MONDAY;
//sDate.Month = RTC_MONTH_MARCH;
//sDate.Date = 12;
//sDate.Year = 25;
//HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	/*----------------------read from EEPROM----------------------------------------------------*/
	Clear_Buffer(saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	EEPROM_Read(PAGE_SAVE_SETTING, 0, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	uint32_t recoveredValues[NUMBER_DATA_SAVE_SETTING / 4];
	memcpy(recoveredValues, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	if (recoveredValues[0]==0 && recoveredValues[1]==0){//that means clear data EEPROM and we must to write it again
		/*-------------------------------------------------------write data to EEPROM--------------------**/
							uint32_t values[NUMBER_DATA_SAVE_SETTING / 4];
							values[0] = timeRejecton100Primary;
							values[1] = timeRejecton500Primary;
							values[2] = timeRejectonTf_5_100mL;
							values[3] = timeRejectonTf_10_100mL;
							values[4] = timeRejectonTf_15_100mL;
							values[5] = timeRejectonTf_5_500mL;
							values[6] = timeRejectonTf_10_500mL;
							values[7] = timeRejectonTf_15_500mL;
							values[8] = PresurePrimary * 10;
							values[9] = PressureSetPoint * 10;
							values[10] = DesigerDeltaPressure * 10;
							values[11] = DeltaDesierTemprature * 10;

							values[14] = passwordSaved;
							exitPress = 0;
							memcpy(saveMenuDataSetting, values, NUMBER_DATA_SAVE_SETTING / 4 * sizeof(uint32_t));
							EEPROM_Write(PAGE_SAVE_SETTING, 0, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);

							/*--------------------------------------------------------------------------------------------------*/
	}
	/*----------------------read from EEPROM----------------------------------------------------*/
	Clear_Buffer(saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	EEPROM_Read(PAGE_SAVE_SETTING, 0, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	memcpy(recoveredValues, saveMenuDataSetting, NUMBER_DATA_SAVE_SETTING);
	timeRejecton100Primary = recoveredValues[0];
	timeRejecton500Primary = recoveredValues[1];
	timeRejectonTf_5_100mL = recoveredValues[2];
	timeRejectonTf_10_100mL = recoveredValues[3];
	timeRejectonTf_15_100mL = recoveredValues[4];
	timeRejectonTf_5_500mL = recoveredValues[5];
	timeRejectonTf_10_500mL = recoveredValues[6];
	timeRejectonTf_15_500mL = recoveredValues[7];
	PresurePrimary = (float)recoveredValues[8] / 10.0;
	PressureSetPoint = (float)recoveredValues[9] / 10.0;
	DesigerDeltaPressure = (float)recoveredValues[10] / 10.0;
	DeltaDesierTemprature = (float)recoveredValues[11] / 10.0;
	passwordSaved = (uint16_t)recoveredValues[14];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//HAL_RTC_GetTime(&hrtc, &sTimeRead, RTC_FORMAT_BIN);
		//HAL_RTC_GetDate(&hrtc, &sDateRead, RTC_FORMAT_BIN);

		// PUMP_SPPED(speedMotor);
		if (menu == BYPASS_TAB && key == KEY_START)
		{
			__NOP();
			key = KEY_NULL;
			ByPass();
		}
		if (menu == PRESSURE_TAB && key == KEY_START)
		{
			__NOP();
			key = KEY_NULL;
			PresuureSetting();
		}
		if (menu == TIMING_TAB && key == KEY_START)
		{
			__NOP();
			key = KEY_NULL;
			TimingStage();
		}

		if (menu == BYPASS_TAB && key == KEY_DOWN)
		{
			__NOP();
			key = KEY_NULL;
			History_Stage();
		}
		if (menu == HISTORY_TAB && key == KEY_UP)
		{
			__NOP();
			key = KEY_NULL;
			TapDisplay(BYPASS_TAB);
			menu = BYPASS_TAB;
		}
		if (menu == BYPASS_TAB && key == KEY_MENU)
		{
			menuSetting();
		}
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 6;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x15;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_FRIDAY;
  DateToUpdate.Month = RTC_MONTH_MARCH;
  DateToUpdate.Date = 0x12;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 99;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 63;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 99;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 63;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 63;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 999;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 63;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, WaterValve_Pin|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Buzzer_Pin|TFT_CS_Pin|GPIO_PIN_8|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DrainVave_Pin|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : WaterValve_Pin */
  GPIO_InitStruct.Pin = WaterValve_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WaterValve_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Level_500_Pin Level_100_Pin Level_000_Pin */
  GPIO_InitStruct.Pin = Level_500_Pin|Level_100_Pin|Level_000_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Buzzer_Pin TFT_CS_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin|TFT_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Key4_Pin Key3_Pin */
  GPIO_InitStruct.Pin = Key4_Pin|Key3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Key1_Pin Key2_Pin */
  GPIO_InitStruct.Pin = Key1_Pin|Key2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DrainVave_Pin */
  GPIO_InitStruct.Pin = DrainVave_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DrainVave_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC6 PC7 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void ResetProcess(void)
{
	ClearScreen(WHITE);
	DrawFillRect(105, 10, 214, 232, WHITE);
	key = NULL;
	// PUMP(GPIO_PIN_RESET);
	PUMP_SPPED(0);
	WaterValve(GPIO_PIN_RESET);
	DrainValve(GPIO_PIN_RESET);
	ByPassTimer = 0;
	//	DrawStringA(105, 15, "Press START TEST", &fontArial14_B, NAVY, WHITE);
	//	DrawStringA(105, 40, "key to start process...", &fontArial14_B, NAVY, WHITE);
	//	DrawStringA(105, 90, "Press DOWN key to", &fontArial14_B, NAVY, WHITE);
	//	DrawStringA(105, 115, "history", &fontArial14_B, NAVY, WHITE);
	TapDisplay(BYPASS_TAB);
	stateProcess = ByPASS;
	erroeCode = NO_ERROR;
	menu = BYPASS_TAB;
	levelWater = WATER_EMPETY;
	levelSwitch = LEVEL_NULL;
	// firstTimeLoad=1;
	// main();
	NVIC_SystemReset();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == DOWN_Pin)
	{

		if (HAL_GPIO_ReadPin(DOWN_GPIO_Port, DOWN_Pin) == GPIO_PIN_RESET)
		{ // Button Pressed (Falling Edge)
			pressStartTime = HAL_GetTick();
			keyState = 1;
		}
		else
		{ // Button Released (Rising Edge)
			pressDurationMenu = HAL_GetTick() - pressStartTime;
			keyState = 0;

			if (pressDurationMenu >= 2000)
			{
				// Long Press Detected
				key = KEY_MENU;
				printf("MENU .... %d\r\n", GPIO_Pin);
				BUZZER(GPIO_PIN_SET); // buzzer on
			}
			else
			{
				// Short Press Detected
				key = KEY_DOWN;
				BUZZER(GPIO_PIN_SET); // buzzer on
				printf("DOWN .... %d\r\n", GPIO_Pin);
			}
			for (int i = 0; i < 100000; i++)
				;
			BUZZER(GPIO_PIN_RESET);
		}
	}
	else if (GPIO_Pin == UP_Pin)
	{
		BUZZER(GPIO_PIN_SET);
		key = KEY_UP;
		printf("UP .. %d\r\n", GPIO_Pin);
		for (int i = 0; i < 100000; i++)
			;
		BUZZER(GPIO_PIN_RESET); // buzzer off
	}

	else if (GPIO_Pin == START_Pin)
	{
		BUZZER(GPIO_PIN_SET);
		key = KEY_START;
		printf("START_Pin .... %d\r\n", GPIO_Pin);
		for (int i = 0; i < 100000; i++)
			;
		BUZZER(GPIO_PIN_RESET); // buzzer off
	}

	else if (GPIO_Pin == STOP_Pin)
	{
		if (HAL_GPIO_ReadPin(STOP_GPIO_Port, STOP_Pin) == GPIO_PIN_RESET)
		{ // Button Pressed (Falling Edge)
			pressStartTime = HAL_GetTick();
			keyState = 1;
		}
		else
		{ // Button Released (Rising Edge)
			pressDurationExit = HAL_GetTick() - pressStartTime;
			keyState = 0;
			if (pressDurationExit >= 2000)
			{
				// Long Press Detected
				key = KEY_STOP_LONG;
				printf("EXIT .... %d\r\n", GPIO_Pin);
				ResetProcess();
			}
			else
			{
				// Short Press Detected
				key = KEY_STOP;
				BUZZER(GPIO_PIN_SET); // buzzer on
				printf("STOP .... %d\r\n", GPIO_Pin);
			}
			for (int i = 0; i < 100000; i++)
				;
			BUZZER(GPIO_PIN_RESET);
		}
	}
	else if (GPIO_Pin == LEVEL000_Pin)
	{
		printf("LEVEL000 .. %d\r\n", GPIO_Pin);
		levelSwitch = LEVEL_LOW;
	}
	else if (GPIO_Pin == LEVEL100_Pin)
	{
		levelSwitch = LEVEL_MEDUIM;
		printf("LEVEL100 .. %d\r\n", GPIO_Pin);
	}
	else if (GPIO_Pin == LEVEL500_Pin)
	{
		levelSwitch = LEVEL_HIGH;
		printf("LEVEL500 .. %d\r\n", GPIO_Pin);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
