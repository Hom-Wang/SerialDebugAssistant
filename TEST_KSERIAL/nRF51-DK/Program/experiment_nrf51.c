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
  KS_INT8    = 0x00, // 8'b000x_xxxx, max lens = 24
  KS_INT16   = 0x20, // 8'b001x_xxxx, max lens = 12
  KS_INT32   = 0x40, // 8'b010x_xxxx, max lens = 6
  KS_INT64   = 0x60, // 8'b011x_xxxx, max lens = 3
  KS_FLOAT32 = 0xC0, // 8'b110x_xxxx, max lens = 6
  KS_FLOAT64 = 0xE0, // 8'b111x_xxxx, max lens = 3
} KSerial_TypeDef;

void kSerial_sendData( void *signalData, uint8_t type, uint8_t lens );
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
  float rad = 0.0f;

  float data = 0.0f;
  float tmpData[16] = {0.0f};
  int16_t signalData[16] = {0};

  System_Init();

  while(1) {

    rad += 0.01f;
    data = 0.0f;
    for(uint8_t i = 1; i < 15; i = i + 2) {
      tmpData[i>>1] = (1.0f / i) * arm_sin_f32(i * rad);
      data += tmpData[i>>1];
    }

    signalData[0]  = tmpData[0] * S16_MAX;
    signalData[1]  = tmpData[1] * S16_MAX;
    signalData[2]  = tmpData[2] * S16_MAX;
    signalData[3]  = tmpData[3] * S16_MAX;
    signalData[4]  = tmpData[4] * S16_MAX;
    signalData[5]  = tmpData[5] * S16_MAX;
    signalData[6]  = tmpData[6] * S16_MAX;
    signalData[7]  = tmpData[7] * S16_MAX;
    signalData[8]  = tmpData[8] * S16_MAX;
    signalData[9]  = tmpData[9] * S16_MAX;
    signalData[10] = data * S16_MAX;
    kSerial_sendData(signalData, KS_INT16, 12);

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
static uint16_t kSerial_calChecksum( uint8_t *data, const uint8_t lens )
{
  uint32_t checkSum = 0;

  for(uint8_t count = 0; count < lens; count += 2)
    checkSum += (data[count] << 8) | (data[count + 1]);

  return (uint16_t)(checkSum);
}

// int8_t  - max lens = 24
// int16_t - max lens = 12
// int32_t - max lens = 6
// int64_t - max lens = 3
void kSerial_sendData( void *signalData, uint8_t type, uint8_t lens )
{
  uint8_t packet[32] = {0};           // tmpData lens >= 2 * lens + 4
  uint8_t *pPacket = packet;

  uint8_t convLens = (type & 0x60) >> 5;
  uint8_t dataLens = lens << convLens;
  uint8_t packetLens = dataLens + 6;  // all packet bytes

  uint16_t checkSum = 0;

  packet[0] = 'S';                                      // 'S' - Signal
  packet[1] = type | (dataLens & 0x1F);                 // data type & lens
  for(uint8_t count = 0; count < dataLens; count++)     // data
    packet[count + 2] = ((uint8_t*)signalData)[count];
  checkSum = kSerial_calChecksum(packet + 2, lens);
  packet[packetLens - 4] = (checkSum & 0xFF00) >> 8;    // checksum H
  packet[packetLens - 3] = (checkSum & 0x00FF);         // checksum L
  packet[packetLens - 2] = '\r';                        // 'r'
  packet[packetLens - 1] = '\n';                        // 'n'

  do {
    Serial_SendByte(*pPacket++);
  } while(--packetLens);
}
/*====================================================================================================*/
/*====================================================================================================*/
