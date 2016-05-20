/*====================================================================================================*/
/*====================================================================================================*/
#include "drivers\nrf51_system.h"
#include "drivers\nrf51_clock.h"
#include "modules\module_serial.h"
#include "algorithms\algorithm_mathUnit.h"

#include "experiment_nrf51.h"
/*====================================================================================================*/
/*====================================================================================================*/
typedef enum {
  INT8    = 0x00, // 8'b000x_xxxx
  INT16   = 0x20, // 8'b001x_xxxx
  INT32   = 0x40, // 8'b010x_xxxx
  INT64   = 0x60, // 8'b011x_xxxx
  FLOAT32 = 0xC0, // 8'b110x_xxxx
  FLOAT64 = 0xE0, // 8'b111x_xxxx
} SerialScope_TypeDef;

void Serial_serialScope( void *signalData, uint8_t type, uint8_t lens );
/*====================================================================================================*/
/*====================================================================================================*/
void System_Init( void )
{
  CLOCK_SourceXTAL(NRF_CLOCK_XTALFREQ_16MHz);
  CLOCK_SourceLFCLK(NRF_CLOCK_LF_SRC_RC);
  CLOCK_CmdHFCLK(ENABLE);
  CLOCK_CmdLFCLK(ENABLE);

  GPIO_Config();
  Serial_Config();

  Delay_100ms(1);
}

int main( void )
{
  static float theta = 0;

  uint32_t cov = S16_MAX;
  int16_t signalData[32] = {0};

  System_Init();

  while(1) {
    LEDR_Toggle();
    LEDG_Toggle();
    LEDB_Toggle();

    theta += 0.5f;
    if(theta == 360.0f) theta = 0;

    
    signalData[0]  = arm_sin_f32(toRad(theta))*cov;
    signalData[1]  = arm_sin_f32(toRad(theta+90.0f))*cov;
    signalData[2]  = arm_sin_f32(toRad(theta+180.0f))*cov;
    signalData[3]  = arm_sin_f32(toRad(theta+270.0f))*cov;
    signalData[4]  = signalData[0] / 2;
    signalData[5]  = signalData[1] / 2;
    signalData[6]  = signalData[2] / 2;
    signalData[7]  = signalData[3] / 2;
    signalData[8]  = signalData[0] / 3;
    signalData[9]  = signalData[1] / 3;
    signalData[10] = signalData[2] / 3;
    signalData[11] = signalData[3] / 3;
    signalData[12] = signalData[0] / 4;
    signalData[13] = signalData[1] / 4;
    signalData[14] = signalData[2] / 4;
    signalData[15] = signalData[3] / 4;
    signalData[16] = signalData[0] / 5;
    signalData[17] = signalData[1] / 5;
    signalData[18] = signalData[2] / 5;
    signalData[19] = signalData[3] / 5;
    signalData[20] = signalData[0] / 6;
    signalData[21] = signalData[1] / 6;
    signalData[22] = signalData[2] / 6;
    signalData[23] = signalData[3] / 6;
    Serial_serialScope(signalData, INT16, 12);
//    Delay_1ms(1);
  }
}
/*====================================================================================================*/
/*====================================================================================================*/
void GPIO_Config( void )
{
  nrf_gpio_range_cfg_output(LED_R, LED_B);
}
/*====================================================================================================*/
/*====================================================================================================*/
uint16_t calChecksum( uint8_t *data, const uint8_t dataLens )
{
  uint8_t count = 0;
  uint32_t checkSum = 0;

  for(count = 0; count < dataLens; count++)
    checkSum += (data[count] << 8) | (data[count + 1]);

  return (uint16_t)((dataLens & 0x01) ? checkSum : checkSum ^ (uint32_t)data[count]);
}

// int8_t  - max lens = 24
// int16_t - max lens = 12
// int32_t - max lens = 6
// int64_t - max lens = 3
void Serial_serialScope( void *signalData, uint8_t type, uint8_t lens )
{
  const uint8_t dataLens = lens << ((type & 0x60) >> 5);

  uint8_t packet[32] = {0};           // tmpData lens >= 2 * lens + 4
  uint8_t *pPacket = packet;
  uint8_t packetLens = dataLens + 6;  // all packet bytes
  uint16_t checkSum = 0;

  packet[0] = 'S';                                      // 'S' - Signal
  packet[1] = type | (dataLens & 0x1F);                 // data type & lens
  for(uint8_t count = 0; count < dataLens; count++)     // data
    packet[count + 2] = ((uint8_t*)signalData)[count];
  checkSum = calChecksum(packet + 2, lens);
  packet[packetLens - 4] = Byte8H(checkSum);            // checksum H
  packet[packetLens - 3] = Byte8L(checkSum);            // checksum L
  packet[packetLens - 2] = '\r';                        // 'r'
  packet[packetLens - 1] = '\n';                        // 'n'

  do {
    Serial_SendByte(*pPacket++);
  } while(--packetLens);
}
/*====================================================================================================*/
/*====================================================================================================*/
