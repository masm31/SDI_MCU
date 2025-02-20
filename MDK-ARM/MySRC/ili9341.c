
#include <stdlib.h> // malloc()
#include <string.h> // memset()
#include <ctype.h>

#include "ili9341.h"
extern SPI_HandleTypeDef hspi1;


void LCDInit(void)
 {
	//LCD_BLK_Clr();
	LCD_RST_Clr();//¸´Î»
	HAL_Delay(10);
	LCD_RST_Set();
	HAL_Delay(10);
	 
	 // SOFTWARE RESET
  LCD_spi_write_command( 0x01);
  HAL_Delay(100);
 //LCD_spi_write_command( 0x11);
  // POWER CONTROL A
  LCD_spi_write_command_data(
      0xCB, 5, (uint8_t[]){ 0x39, 0x2C, 0x00, 0x34, 0x02 });
  // POWER CONTROL B
  LCD_spi_write_command_data(
      0xCF, 3, (uint8_t[]){ 0x00, 0xC1, 0x30 });

  // DRIVER TIMING CONTROL A
  LCD_spi_write_command_data(
      0xE8, 3, (uint8_t[]){ 0x85, 0x00, 0x78 });

  // DRIVER TIMING CONTROL B
  LCD_spi_write_command_data(
      0xEA, 2, (uint8_t[]){ 0x00, 0x00 });

  // POWER ON SEQUENCE CONTROL
  LCD_spi_write_command_data(
      0xED, 4, (uint8_t[]){ 0x64, 0x03, 0x12, 0x81 });

  // PUMP RATIO CONTROL
  LCD_spi_write_command_data(
      0xF7, 1, (uint8_t[]){ 0x20 });

  // POWER CONTROL,VRH[5:0]
  LCD_spi_write_command_data(
      0xC0, 1, (uint8_t[]){ 0x23 });

  // POWER CONTROL,SAP[2:0];BT[3:0]
  LCD_spi_write_command_data(
      0xC1, 1, (uint8_t[]){ 0x10 });

  // VCM CONTROL
  LCD_spi_write_command_data(
      0xC5, 2, (uint8_t[]){ 0x3E, 0x28 });

  // VCM CONTROL 2
  LCD_spi_write_command_data(
      0xC7, 1, (uint8_t[]){ 0x86 });

  // MEMORY ACCESS CONTROL
  LCD_spi_write_command_data(
      0x36, 1, (uint8_t[]){ 0x48 });

  // PIXEL FORMAT
  LCD_spi_write_command_data(
      0x3A, 1, (uint8_t[]){ 0x55 });

  // FRAME RATIO CONTROL, STANDARD RGB COLOR
  LCD_spi_write_command_data(
      0xB1, 2, (uint8_t[]){ 0x00, 0x18 });

  // DISPLAY FUNCTION CONTROL
  LCD_spi_write_command_data(
      0xB6, 3, (uint8_t[]){ 0x08, 0x82, 0x27 });

  // 3GAMMA FUNCTION DISABLE
  LCD_spi_write_command_data(
      0xF2, 1, (uint8_t[]){ 0x00 });

  // GAMMA CURVE SELECTED
  LCD_spi_write_command_data(
      0x26, 1, (uint8_t[]){ 0x01 });

  // POSITIVE GAMMA CORRECTION
  LCD_spi_write_command_data(
      0xE0, 15, (uint8_t[]){ 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                             0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 });

  // NEGATIVE GAMMA CORRECTION
  LCD_spi_write_command_data(
      0xE1, 15, (uint8_t[]){ 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                             0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F });

  // EXIT SLEEP
  LCD_spi_write_command(0x11);
  HAL_Delay(1);

  // TURN ON DISPLAY
  LCD_spi_write_command( 0x29);

  // MADCTL
														 
  LCD_spi_write_command_data(
      0x36, 1, (uint8_t[]){ LCD_screen_rotation(3) });

			ClearScreen(BLACK);
		//	LCD_BLK_Set();
 }
 
  void LCD_spi_write_command( uint8_t Command)
 {
	 LCD_DC_Clr();
	 HAL_SPI_Transmit(&hspi1, &Command, sizeof(Command),100);

	 LCD_DC_Set();
 }
 void LCD_spi_write_command_data( uint8_t Command, uint16_t DataSize, uint8_t DataArray[])
 {
	 LCD_spi_write_command( Command);
	 LCD_spi_write_data(DataArray, DataSize);
	// HAL_SPI_Transmit(&hspi1, DataArray, DataSize, 100);
 }
 
 void LCD_spi_write_data( uint8_t DataArray[], uint8_t DataSize)
 {
	 LCD_DC_Set();

	 

	 HAL_SPI_Transmit(&hspi1, DataArray, DataSize,100);

	 LCD_DC_Clr();
 }


//void LCD_pushcolour(uint16_t colour)
//{
//	ili9341_writedata8(colour>>8);
//	ili9341_writedata8(colour);
//}
  void LCD_Address_Set(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1)
	{
	 LCD_spi_write_command_data(0x2A, 4, (uint8_t[]){ (X0 >> 8) & 0xFF, X0 & 0xFF, (X1 >> 8) & 0xFF, X1 & 0xFF });
   LCD_spi_write_command_data(0x2B, 4, (uint8_t[]){ (Y0 >> 8) & 0xFF, Y0 & 0xFF, (Y1 >> 8) & 0xFF, Y1 & 0xFF });
   LCD_spi_write_command(0x2C);		
	}
	
	uint8_t LCD_screen_rotation(uint8_t orientation)
	{
		switch (orientation) {
    default:
    case isoDown:
      return 0x40 | 0x08;
    case isoRight:
      return 0x40 | 0x80 | 0x20 | 0x08;
    case isoUp:
      return 0x80 | 0x08;
    case isoLeft:
      return 0x20 | 0x08;
  }
	}
	
void LCD_draw_pixel( uint16_t color, uint16_t x, uint16_t y)
{
	LCD_Address_Set(x, y , x, y);
	
	LCD_spi_write_data( (uint8_t[]){ (color >> 8) & 0xFF, color & 0xFF} , 2);

 // HAL_SPI_Transmit(lcd->spi_hal, (uint8_t *)&color_le, 2U, __SPI_MAX_DELAY__);
}
	




void LCD_Writ_Bus(uint8_t dat) 
{	
	HAL_SPI_Transmit(&hspi1, &dat, 1, 100);
}

/******************************************************************************
      º¯ÊıËµÃ÷£ºLCDĞ´ÈëÊı¾İ
      Èë¿ÚÊı¾İ£ºdat Ğ´ÈëµÄÊı¾İ
      ·µ»ØÖµ£º  ÎŞ
******************************************************************************/


void LCD_WR_DATA8(uint8_t dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      º¯ÊıËµÃ÷£ºLCDĞ´ÈëÊı¾İ
      Èë¿ÚÊı¾İ£ºdat Ğ´ÈëµÄÊı¾İ
      ·µ»ØÖµ£º  ÎŞ
******************************************************************************/
void LCD_WR_DATA(uint16_t dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      º¯ÊıËµÃ÷£ºLCDĞ´ÈëÃüÁî
      Èë¿ÚÊı¾İ£ºdat Ğ´ÈëµÄÃüÁî
      ·µ»ØÖµ£º  ÎŞ
******************************************************************************/
void LCD_WR_REG(uint8_t dat)
{
	LCD_DC_Clr();//Ğ´ÃüÁî
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//Ğ´Êı¾İ
}





uint16_t Color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_Address_Set(x,y,x,y);//ÉèÖÃ¹â±êÎ»ÖÃ 
	LCD_WR_DATA(color);
} 

void DrawFillRect(int16_t X, int16_t Y, int16_t W, int16_t H, uint16_t Color)
{
	uint16_t Color1 = Color;

	uint32_t Num;

	Num= W * H;

	LCD_Address_Set(X, Y, X+W-1, Y+H-1);

	for(uint32_t i=0; i< Num; i++)
	{
//		SPI1->DR = Color;
//	  while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
//	  while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
      SPI1->DR = (uint8_t)((Color1 >> 8));
	    while (!((SPI1->SR)&(1<<1))) {};
			SPI1->DR = (uint8_t) (Color1 ) ;
	    while (!((SPI1->SR)&(1<<1))) {};
		
	}



}
	
void ClearScreen(uint16_t Color)
{
	DrawFillRect(0, 0, 320, 240, Color);
}


void DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //Â¼Æ‹ã—¸Â±ê”¶ÃÂ¿ 
	delta_y=y2-y1;
	uRow=x1;//Â»Â­ÏŸÆ°Âµã—¸Â±êŠ‰uCol=y1;
	if(delta_x>0)incx=1; //É¨ÖƒÂµÂ¥Â²Â½Â·Â½Ï² 
	else if (delta_x==0)incx=0;//Â´Â¹Ö±ÏŸ 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//Ë®Æ½ÏŸ 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //Ñ¡È¡Â»Ã¹Â±Â¾Ô¶ÃÂ¿×¸Â±ê–¡ 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_draw_pixel(uRow,uCol,color);//Â»Â­ÂµãŠ‰	xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}



int DrawFastHLine(uint16_t X,uint16_t Y,uint16_t W,uint16_t Color)
{
  LCD_Address_Set(X, Y, X+W-1, Y);  
  for(int i = 0; i < W; i++)
	{
    SPI1->DR = (uint8_t)((Color >> 8));
		while (!((SPI1->SR)&(1<<1))) {};
		SPI1->DR = (uint8_t) (Color ) ;
		while (!((SPI1->SR)&(1<<1))) {};
	}
return W;
}
int DrawFastVLine(uint16_t X,uint16_t Y,uint16_t H,uint16_t Color)
{
  LCD_Address_Set(X, Y, X, Y+H-1);  
  for(int i = 0; i < H; i++)
	{
    SPI1->DR = (uint8_t)((Color >> 8));
		while (!((SPI1->SR)&(1<<1))) {};
		SPI1->DR = (uint8_t) (Color ) ;
		while (!((SPI1->SR)&(1<<1))) {};
	}
  return H;
}

int DrawRectangle(uint16_t X, uint16_t Y, uint16_t W, uint16_t H,uint16_t Color)
{

  DrawFastVLine(X, Y,H, Color);
  DrawFastVLine(X+W-1, Y,H, Color);

	DrawFastHLine(X, Y,W, Color);
	DrawFastHLine(X, Y+H,W, Color);

return W;
}
void Draw_Circle(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_draw_pixel(x0-b,y0-a,color);             //3           
		LCD_draw_pixel(x0+b,y0-a,color);             //0           
		LCD_draw_pixel(x0-a,y0+b,color);             //1                
		LCD_draw_pixel(x0-a,y0-b,color);             //2             
		LCD_draw_pixel(x0+b,y0+a,color);             //4               
		LCD_draw_pixel(x0+a,y0-b,color);             //5
		LCD_draw_pixel(x0+a,y0+b,color);             //6 
		LCD_draw_pixel(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//ÅÂ¶Ï’ÂªÂ»Â­ÂµÄµãŠ‡Â·ñ¹½”Â¶
		{
			b--;
		}
	}
}



/*
**************************************************************************************************************************
**************************************************************************************************************************
font function
**************************************************************************************************************************
**************************************************************************************************************************
*/





int GetFontHeight(const FONTHEADER_BFC *pFont)
{
	int height = -1;
		height = pFont->height ;
	return height;
}
	

int DrawChar( int x0, int y0, const FONTHEADER_BFC *pFont, unsigned short ch, uint16_t FgColor, uint16_t BgColor)
{
	// 1. find the character information first

	
		uint8_t Height = pFont->height ;
		uint8_t Width =  pFont->width_table[ch-32];
		uint16_t data_size = Height * Width  ;  
	 uint8_t *pData = pFont->data_table;   // pointer to data array
    uint16_t offset = pFont->offset_table[ch - 32];

	
	LCD_Address_Set(x0, y0, x0+Width-1, y0+Height-1);
	
	for(uint32_t i =0; i < data_size; i++)
	{
		if(((pData[offset+ (i/8)] >> (i%8)) & 0x1) == 1)
		{
			SPI1->DR = (uint8_t)((FgColor >> 8));
	    while (!((SPI1->SR)&(1<<1))) {};
			SPI1->DR = (uint8_t) (FgColor ) ;
	    while (!((SPI1->SR)&(1<<1))) {};
		}
		else
			{
			SPI1->DR = (uint8_t)((BgColor >> 8));
	    while (!((SPI1->SR)&(1<<1))) {};
			SPI1->DR = (uint8_t) (BgColor ) ;
	    while (!((SPI1->SR)&(1<<1))) {};	
		}
		
	}


		return Width;
	


}
int DrawPic( int x0, int y0,  const PICHEADER *pPic)
{
	// 1. find the character information first

	uint16_t Height = pPic->height;
	uint16_t Width = pPic->width;
	uint16_t data_size = Height * Width  ;
  unsigned short *pData = pPic->data_table;

	LCD_Address_Set(x0, y0, x0+Width-1, y0+Height-1);
	
	for(uint32_t i =0; i < data_size; i++)
	{
		
			SPI1->DR = (uint8_t)((pData[i] >> 8));
	    while (!((SPI1->SR)&(1<<1))) {};
			SPI1->DR = (uint8_t) (pData[i]  ) ;
	    while (!((SPI1->SR)&(1<<1))) {};
		
		
	}


		return Width;
	


}

int DrawStringA( int x0, int y0, const char *str, const FONTHEADER_BFC *pFont,uint16_t FgColor, uint16_t BgColor)
{
	int x = x0;
	int y = y0;

int Pos=0;
	while(str[Pos] != NULL)
	{
    x += DrawChar( x, y, pFont, str[Pos], FgColor, BgColor);
		Pos++;
	}

	return x;
}


//int DrawString( int x0, int y0, const char *str, const FONTHEADER_BFC *pFont,uint16_t Color)
//{
//	uint16_t FgColor, BgColor;
//	if(Color == WN_COLOR)
//	{
//		FgColor = FOR_COLOR;
//		BgColor = BACK_COLOR;
//	}else if(Color == NW_COLOR)
//	{
//		FgColor = BACK_COLOR;
//		BgColor = FOR_COLOR;
//	}else if(Color == WB_COLOR)
//	{
//		FgColor = FOR_COLOR;
//		BgColor = TXTB_COLOR;
//	}else if(Color == GW_COLOR)
//	{
//		FgColor = GRAY;
//		BgColor = WHITE;
//	}
//	else
//	{
//		FgColor = TXTB_COLOR;
//		BgColor = FOR_COLOR;
//	}
//	
//	return DrawStringA( x0, y0, str, pFont,FgColor, BgColor);
//}
void DrawRoundRectangle(uint16_t X1, uint16_t Y1, uint16_t W, uint16_t H,uint16_t Redius, uint16_t Color)
{

  DrawFastHLine(X1 + Redius  , Y1    , W - Redius - Redius, Color); // Top
  DrawFastHLine(X1 + Redius  , Y1 + H - 1, W - Redius - Redius, Color); // Bottom
  DrawFastVLine(X1      , Y1 + Redius  , H - Redius - Redius, Color); // Left
  DrawFastVLine(X1 + W - 1, Y1 + Redius  , H - Redius - Redius, Color); // Right

  DrawCircleHelper(X1 + Redius    , Y1 + Redius    , Redius, 1, Color);
  DrawCircleHelper(X1 + W - Redius - 1, Y1 + Redius    , Redius, 2, Color);
  DrawCircleHelper(X1 + W - Redius - 1, Y1 + H - Redius - 1, Redius, 4, Color);
  DrawCircleHelper(X1 + Redius    , Y1 + H - Redius - 1, Redius, 8, Color);
}
void DrawFillRoundRectangle(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color)
{


  // smarter version
  DrawFillRect(x, y + r, w, h - r - r, color);

  // draw four corners
  FillCircleHelper(x + r, y + h - r - 1, r, 1, w - r - r - 1, color);
  FillCircleHelper(x + r    , y + r, r, 2, w - r - r - 1, color);

 
}
void FillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint32_t color)
{
  int32_t f     = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -r - r;
  int32_t y     = 0;

  delta++;

  while (y < r) {
    if (f >= 0) {
      if (cornername & 0x1) DrawFastHLine(x0 - y, y0 + r, y + y + delta, color);
      if (cornername & 0x2) DrawFastHLine(x0 - y, y0 - r, y + y + delta, color);
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }

    y++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) DrawFastHLine(x0 - r, y0 + y, r + r + delta, color);
    if (cornername & 0x2) DrawFastHLine(x0 - r, y0 - y, r + r + delta, color);
  }
}
void DrawCircleHelper( int32_t x0, int32_t y0, int32_t rr, uint8_t cornername, uint32_t color)
{
  int32_t f     = 1 - rr;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * rr;
  int32_t xe    = 0;
  int32_t xs    = 0;
  int32_t len   = 0;

  //begin_tft_write();          // Sprite class can use this function, avoiding begin_tft_write()

  while (xe < rr--)
  {
    while (f < 0) {
      ++xe;
      f += (ddF_x += 2);
    }
    f += (ddF_y += 2);

    if (xe-xs==1) {
      if (cornername & 0x1) { // left top
        DrawPoint(x0 - xe, y0 - rr, color);
        DrawPoint(x0 - rr, y0 - xe, color);
      }
      if (cornername & 0x2) { // right top
        DrawPoint(x0 + rr    , y0 - xe, color);
        DrawPoint(x0 + xs + 1, y0 - rr, color);
      }
      if (cornername & 0x4) { // right bottom
        DrawPoint(x0 + xs + 1, y0 + rr    , color);
        DrawPoint(x0 + rr, y0 + xs + 1, color);
      }
      if (cornername & 0x8) { // left bottom
        DrawPoint(x0 - rr, y0 + xs + 1, color);
        DrawPoint(x0 - xe, y0 + rr    , color);
      }
    }
    else {
      len = xe - xs++;
      if (cornername & 0x1) { // left top
        DrawFastHLine(x0 - xe, y0 - rr, len, color);
        DrawFastVLine(x0 - rr, y0 - xe, len, color);
      }
      if (cornername & 0x2) { // right top
        DrawFastVLine(x0 + rr, y0 - xe, len, color);
        DrawFastHLine(x0 + xs, y0 - rr, len, color);
      }
      if (cornername & 0x4) { // right bottom
        DrawFastHLine(x0 + xs, y0 + rr, len, color);
        DrawFastVLine(x0 + rr, y0 + xs, len, color);
      }
      if (cornername & 0x8) { // left bottom
        DrawFastVLine(x0 - rr, y0 + xs, len, color);
        DrawFastHLine(x0 - xe, y0 + rr, len, color);
      }
    }
    xs = xe;
  }
}
