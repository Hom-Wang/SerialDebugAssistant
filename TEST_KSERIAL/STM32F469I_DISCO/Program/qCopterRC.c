/*====================================================================================================*/
/*====================================================================================================*/
#include "drivers\stm32f4_system.h"
#include "modules\module_otm8009.h"
#include "algorithms\algorithm_mathUnit.h"
#include "applications\app_kSerial.h"
#include "applications\app_waveForm.h"

#include "qCopterRC.h"
/*====================================================================================================*/
/*====================================================================================================*/
WaveForm_Struct WaveForm;
/*====================================================================================================*/
/*====================================================================================================*/
void QRCD_Init( void )
{
  HAL_Init();

  QRCD_GPIO_Config();
  QRCD_UART_Config();
  QRCD_KSerial_Config();
  QRCD_LCD_Config();

	WaveForm.Channel     = 4;
	WaveForm.WindowColor = LCD_WHITE;
	WaveForm.BackColor   = LCD_BLACK;
  for(uint8_t i = 0; i < WaveForm.Channel; i++) {
    WaveForm.Data[i]  = 0;
    WaveForm.Scale[i] = 100;
  }
	WaveForm.PointColor[0] = LCD_RED;
  WaveForm.PointColor[1] = LCD_BLUE;
  WaveForm.PointColor[2] = LCD_YELLOW;
  WaveForm.PointColor[3] = LCD_GREEN;

  LCD_Clear(LCD_BLACK);
}
/*====================================================================================================*/
/*====================================================================================================*/
void QRCD_Loop( void )
{
  float rad = 0.0f;

  float data = 0.0f;
  float tmpData[4] = {0.0f};
  int16_t signalData[32] = {0};

  while(1) {
    LED_G_Toggle();

    rad += 0.01f;
    data = 0;
    for(uint8_t i = 1; i < 10; i = i + 2)
      data += (1.0f / i) * sinf(i * rad);

    tmpData[0] = sinf(rad);
    tmpData[1] = (1.0f/3) * sinf(3 * rad);
    tmpData[2] = (1.0f/5) * sinf(5 * rad);
    tmpData[3] = data;

    WaveForm.Data[0] = tmpData[0] * 20000;
    WaveForm.Data[1] = tmpData[1] * 20000;
    WaveForm.Data[2] = tmpData[2] * 20000;
    WaveForm.Data[3] = tmpData[3] * 20000;
    WaveFormPrint(&WaveForm, ENABLE);

    signalData[0] = tmpData[0] * S16_MAX;
    signalData[1] = tmpData[1] * S16_MAX;
    signalData[2] = tmpData[2] * S16_MAX;
    signalData[3] = tmpData[3] * S16_MAX;
    kSerial_sendData(signalData, KS_INT16, 12);

    while(KEY_Read())
      LCD_Clear(LCD_BLACK);
  }
}
/*====================================================================================================*/
/*====================================================================================================*/
