/*
 * ili9341.h
 *
 *  Created on: Nov 28, 2019
 *      Author: andrew
 */

#ifndef __ILI9341_H
#define __ILI9341_H


#include "stm32f1xx_hal.h"

#include "main.h"


#include "bfctype.h" 
#include <math.h>
#define USE_HORIZONTAL 0 

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F 
 
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define DARKGREEN        0x0320 
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
#define NAVY             0x0210 //0x004080

// Color definitions
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x001F
#define	ILI9341_RED     0xF800
#define	ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF
#define ILI9341_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))


#define ILI9341_RES_Pin       GPIO_PIN_4
#define ILI9341_RES_GPIO_Port GPIOB
#define ILI9341_CS_Pin        GPIO_PIN_4
#define ILI9341_CS_GPIO_Port  GPIOA
#define ILI9341_DC_Pin        GPIO_PIN_12
#define ILI9341_DC_GPIO_Port  GPIOC

#define ILI9341_BACKLIGHT_Pin        GPIO_PIN_14
#define ILI9341_BACKLIGHT_Port  GPIOB


#define LCD_RST_Clr()  HAL_GPIO_WritePin(ILI9341_RES_GPIO_Port,ILI9341_RES_Pin, GPIO_PIN_RESET)//RES
#define LCD_RST_Set()  HAL_GPIO_WritePin(ILI9341_RES_GPIO_Port,ILI9341_RES_Pin, GPIO_PIN_SET)

#define LCD_DC_Clr()   HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port,ILI9341_DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_Set()   HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port,ILI9341_DC_Pin, GPIO_PIN_SET)

#define LCD_CS_Clr()   HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port,ILI9341_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_Set()   HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port,ILI9341_CS_Pin, GPIO_PIN_SET)

//#define LCD_BLK_Clr()   HAL_GPIO_WritePin(TFT_BACKLIGHT_GPIO_Port,TFT_BACKLIGHT_Pin, GPIO_PIN_RESET)
//#define LCD_BLK_Set()   HAL_GPIO_WritePin(TFT_BACKLIGHT_GPIO_Port,TFT_BACKLIGHT_Pin, GPIO_PIN_SET)

//#define LCD_BLK(cp)   __HAL_TIM_SetCompare (&htim2,TIM_CHANNEL_1, (uint16_t) ((float)4999*(float)((float)cp/(float)100)));
typedef enum
{
  // orientation is based on position of board pins when looking at the screen
  isoNONE                     = -1,
  isoDown,   isoPortrait      = isoDown,  // = 0
  isoRight,  isoLandscape     = isoRight, // = 1
  isoUp,     isoPortraitFlip  = isoUp,    // = 2
  isoLeft,   isoLandscapeFlip = isoLeft,  // = 3
  isoCOUNT                                // = 4
}
LCD_orientation_t;

void LCDInit(void);

void LCD_spi_write_command( uint8_t Command);
void LCD_spi_write_data( uint8_t DataArray[], uint8_t DataSize);
void LCD_spi_write_command_data( uint8_t Command, uint16_t DataSize, uint8_t DataArray[]);
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

uint8_t LCD_screen_rotation(uint8_t orientation);
uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
void draw_pixel( uint16_t color, uint16_t x, uint16_t y);
void DrawFillRect(int16_t X, int16_t Y, int16_t W, int16_t H, uint16_t Color);
void ClearScreen(uint16_t Color);
void DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
int  DrawRectangle(uint16_t X, uint16_t Y, uint16_t W, uint16_t H,uint16_t Color);
void Draw_Circle(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color);//詺指露篓位謨禄颅一赂枚圆
void DrawRoundRectangle(uint16_t X1, uint16_t Y1, uint16_t W, uint16_t H,uint16_t Redius, uint16_t Color);
void DrawFillRoundRectangle(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);

void DrawCircleHelper( int32_t x0, int32_t y0, int32_t rr, uint8_t cornername, uint32_t color);
void FillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint32_t color);

int DrawFastHLine(uint16_t X,uint16_t Y,uint16_t W,uint16_t Color);
int DrawFastVLine(uint16_t X,uint16_t Y,uint16_t H,uint16_t Color);
 
void LCD_Writ_Bus(uint8_t dat);//模拟SPI时序
void LCD_WR_DATA8(uint8_t dat);//写入一个字节
void LCD_WR_DATA(uint16_t dat);//写入两个字节
void LCD_WR_REG(uint8_t dat);//写入一个指令


int GetFontHeight(const FONTHEADER_BFC *pFont);
int DrawChar( int x0, int y0, const FONTHEADER_BFC *pFont, unsigned short ch,uint16_t FgColor, uint16_t BgColor);
int DrawStringA( int x0, int y0, const char *str, const FONTHEADER_BFC *pFont,uint16_t FgColor, uint16_t BgColor);
int DrawPic( int x0, int y0,  const PICHEADER *pPic);
void LCD_DrawFastHLine(uint16_t X,uint16_t Y,uint16_t W,uint16_t Color);


int DrawString( int x0, int y0, const char *str, const FONTHEADER_BFC *pFont,uint16_t Color);

#endif /* __ILI9341_H */
