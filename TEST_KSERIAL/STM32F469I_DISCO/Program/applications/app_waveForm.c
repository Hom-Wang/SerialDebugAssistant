/*=====================================================================================================*/
/*=====================================================================================================*/
#include "drivers\stm32f4_system.h"
#include "modules\module_otm8009.h"

#include "app_waveForm.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
void WaveFormInit( WaveForm_Struct *waveForm )
{
  waveForm->Channel = 3;
  waveForm->WindowColor = LCD_WHITE;
  waveForm->BackColor = LCD_BLACK;
  waveForm->Data[0] = 0;
  waveForm->Data[1] = 0;
  waveForm->Data[2] = 0;
  waveForm->Scale[0] = 100;
  waveForm->Scale[1] = 100;
  waveForm->Scale[2] = 100;
  waveForm->PointColor[0] = LCD_RED;
  waveForm->PointColor[1] = LCD_GREEN;
  waveForm->PointColor[2] = LCD_BLUE;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void WaveFormPrint( WaveForm_Struct *waveForm, uint8_t display )
{
  int16_t TempY[WaveChannelMax] = {0};
  int16_t CoordiY[WaveChannelMax] = {0};

  static uint16_t WavePic[WaveChannelMax][WaveFormW] = {0};

  /* 更新顯示座標 */
  for(int16_t i = 0; i < waveForm->Channel; i++) {
    for(int16_t j = 0; j < WaveFormW - 1; j++)
      WavePic[i][j] = WavePic[i][j+1];
    TempY[i] = (int16_t)((float)waveForm->Data[i] / waveForm->Scale[i] + 0.5f);
    CoordiY[i] = WaveFormH - TempY[i];
    if((CoordiY[i] > 0) && (CoordiY[i] < WaveForm2H))
      WavePic[i][WaveFormW-1] = CoordiY[i];
  }
  if(display == ENABLE) {
    /* 顯示 */
    for(int16_t i = 0; i < WaveFormW - 1; i++) {
      /* 清除上筆數據 */
      for(int16_t j = 0; j < waveForm->Channel; j++)
        LCD_DrawPixel(WaveWindowX+i+1, WaveWindowY+WavePic[j][i], waveForm->BackColor);
      /* 畫輔助線 */
      if((i % 10) < 3) {
        for(int16_t j = 0; j <= WaveFormH; j += (WaveFormH / 10)) {
          LCD_DrawPixel(WaveWindowX+i, WaveWindowY+WaveFormH+j, waveForm->WindowColor);
          LCD_DrawPixel(WaveWindowX+i, WaveWindowY+WaveFormH-j, waveForm->WindowColor);
        }
      }
      /* 畫中線 */
      LCD_DrawPixel(WaveWindowX+i, WaveWindowY+WaveFormH, waveForm->WindowColor);
      /* 更新顯示新數據 */
      for(int16_t j = 0; j < waveForm->Channel; j++)
        LCD_DrawPixel(WaveWindowX+i, WaveWindowY+WavePic[j][i], waveForm->PointColor[j]);
    }
    for(int16_t j = 0; j < waveForm->Channel; j++)
      LCD_DrawPixel(WaveWindowX, WaveWindowY+WavePic[j][0], waveForm->BackColor);
    /* 畫邊框 */
    LCD_DrawLineX(WaveWindowX,                 WaveWindowY,                  WaveFormW,  waveForm->WindowColor);
    LCD_DrawLineX(WaveWindowX,                 WaveWindowY + WaveForm2H - 1, WaveFormW,  waveForm->WindowColor);
    LCD_DrawLineY(WaveWindowX,                 WaveWindowY,                  WaveForm2H, waveForm->WindowColor);
    LCD_DrawLineY(WaveWindowX + WaveFormW - 1, WaveWindowY,                  WaveForm2H, waveForm->WindowColor);
  }
}
/*=====================================================================================================*/
/*=====================================================================================================*/
