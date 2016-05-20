/* #include "app_waveForm.h" */

#ifndef __APP_WAVEFORM_H
#define __APP_WAVEFORM_H

#include "stm32f4xx_hal.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
#define WaveChannelMax  4
#define WaveWindowX     0
#define WaveWindowY     0
#define WaveFormW       800
#define WaveFormH       240
#define WaveForm2H      480
/*=====================================================================================================*/
/*=====================================================================================================*/
typedef struct {
  uint8_t  Channel;
  int16_t  Data[WaveChannelMax];
  uint16_t Scale[WaveChannelMax];
  uint32_t PointColor[WaveChannelMax];
  uint32_t WindowColor;
  uint32_t BackColor;
} WaveForm_Struct;
/*=====================================================================================================*/
/*=====================================================================================================*/
void WaveFormInit( WaveForm_Struct *waveForm );
void WaveFormPrint( WaveForm_Struct *waveForm, uint8_t display );
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
