/* #include "module_otm8009.h" */

#ifndef __MODULE_OTM8009_H
#define __MODULE_OTM8009_H

#include "stm32f4xx_hal.h"
#include "algorithms\algorithm_string.h"

#include "fonts.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
//#define LCD_ORIENTATION_PORTRAIT
#define LCD_ORIENTATION_LANDSCAPE
  
#ifdef LCD_ORIENTATION_PORTRAIT
  #define LCD_W 480
  #define LCD_H 800
#else
  #define LCD_W 800
  #define LCD_H 480 
#endif

#define RGB_TO_GARY(C_R, C_G, C_B)  ((uint8_t)(0.299f*C_R + 0.587f*C_G + 0.114f*C_B))
#define ToRGB565(RGB888)            ((uint16_t)((RGB888&0xF80000>>8)|(RGB888&0x00FC00>>5)|(RGB888&0x0000F8>>3)))
#define ToRGB888(RGB565)            ((uint16_t)((RGB565&0xF800<<8)|(RGB565&0x07E0<<5)|(RGB565&0x001F<<3)))

#define LCD_BLUE          ((uint32_t) 0xFF0000FF)
#define LCD_GREEN         ((uint32_t) 0xFF00FF00)
#define LCD_RED           ((uint32_t) 0xFFFF0000)
#define LCD_CYAN          ((uint32_t) 0xFF00FFFF)
#define LCD_MAGENTA       ((uint32_t) 0xFFFF00FF)
#define LCD_YELLOW        ((uint32_t) 0xFFFFFF00)
#define LCD_LIGHTBLUE     ((uint32_t) 0xFF8080FF)
#define LCD_LIGHTGREEN    ((uint32_t) 0xFF80FF80)
#define LCD_LIGHTRED      ((uint32_t) 0xFFFF8080)
#define LCD_LIGHTCYAN     ((uint32_t) 0xFF80FFFF)
#define LCD_LIGHTMAGENTA  ((uint32_t) 0xFFFF80FF)
#define LCD_LIGHTYELLOW   ((uint32_t) 0xFFFFFF80)
#define LCD_DARKBLUE      ((uint32_t) 0xFF000080)
#define LCD_DARKGREEN     ((uint32_t) 0xFF008000)
#define LCD_DARKRED       ((uint32_t) 0xFF800000)
#define LCD_DARKCYAN      ((uint32_t) 0xFF008080)
#define LCD_DARKMAGENTA   ((uint32_t) 0xFF800080)
#define LCD_DARKYELLOW    ((uint32_t) 0xFF808000)
#define LCD_WHITE         ((uint32_t) 0xFFFFFFFF)
#define LCD_LIGHTGRAY     ((uint32_t) 0xFFD3D3D3)
#define LCD_GRAY          ((uint32_t) 0xFF808080)
#define LCD_DARKGRAY      ((uint32_t) 0xFF404040)
#define LCD_BLACK         ((uint32_t) 0xFF000000)
#define LCD_BROWN         ((uint32_t) 0xFFA52A2A)
#define LCD_ORANGE        ((uint32_t) 0xFFFFA500)
#define LCD_TRANSPARENT   ((uint32_t) 0xFF000000)
/*=====================================================================================================*/
/*=====================================================================================================*/
typedef struct {
  uint32_t fontColor;
  uint32_t backColor;
  sFONT    *pFont;
} LCD_DrawPropTypeDef;

typedef enum {
  CENTER_MODE = 0x01,
  RIGHT_MODE  = 0x02,
  LEFT_MODE   = 0x03
} Text_AlignModeTypdef;
/*=====================================================================================================*/
/*=====================================================================================================*/
uint8_t LCD_Init( void );

void    LCD_Clear( uint32_t color );
//void    LCD_SetCursor( uint16_t CoordiX, uint16_t CoordiY );
//void    LCD_SetWindow( uint16_t StartX, uint16_t StartY, uint16_t EndX, uint16_t EndY );
//void    LCD_SetBackLight( uint16_t BackLight );
void    LCD_DrawPixel( uint16_t posX, uint16_t posY, uint32_t color );
void    LCD_DrawLineX( uint16_t posX, uint16_t posY, uint16_t length, uint32_t color );
void    LCD_DrawLineY( uint16_t posX, uint16_t posY, uint16_t length, uint32_t color );
void    LCD_DrawLine( uint16_t posX1, uint16_t posY1, uint16_t posX2, uint16_t posY2, uint32_t color );
void    LCD_DrawRect( uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t color );
void    LCD_DrawRectFill( uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t color );
void    LCD_DrawCircle( uint16_t posX, uint16_t posY, uint16_t radius, uint32_t color );
//void    LCD_DrawPicture( uint16_t CoordiX, uint16_t CoordiY, uint16_t Width, uint16_t Height, const uint8_t *Pic );
void    LCD_PutChar( uint16_t posX, uint16_t posY, uint8_t word, uint32_t fontColor, uint32_t backColor );
void    LCD_PutStr( uint16_t posX, uint16_t posY, char *string, Text_AlignModeTypdef mode, uint32_t fontColor, uint32_t backColor );
void    LCD_PutNum( uint16_t posX, uint16_t posY, StringType type, uint8_t lens, uint32_t number, uint32_t fontColor, uint32_t backColor );
void    LCD_TestColorBar( void );
void    LCD_TestColorClear( void );
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
