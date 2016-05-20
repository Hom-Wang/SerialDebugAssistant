/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "stm32f4_sdram.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
static SDRAM_HandleTypeDef SDRAM_HandleStruct;
static FMC_SDRAM_TimingTypeDef FMC_SDRAM_TimingStruct;
static FMC_SDRAM_CommandTypeDef FMC_SDRAM_CommandStruct;
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : SDRAM_Init
**功能 : SDRAM Init
**輸入 : None
**輸出 : None
**使用 : SDRAM_Init();
**=====================================================================================================*/
/*=====================================================================================================*/
#define REFRESH_COUNT   ((uint32_t)0x0569)
uint8_t SDRAM_Init( void )
{
  static uint8_t state = HAL_ERROR;

  FMC_SDRAM_TimingStruct.LoadToActiveDelay    = 2;
  FMC_SDRAM_TimingStruct.ExitSelfRefreshDelay = 7;
  FMC_SDRAM_TimingStruct.SelfRefreshTime      = 4;
  FMC_SDRAM_TimingStruct.RowCycleDelay        = 7;
  FMC_SDRAM_TimingStruct.WriteRecoveryTime    = 2;
  FMC_SDRAM_TimingStruct.RPDelay              = 2;
  FMC_SDRAM_TimingStruct.RCDDelay             = 2;

  SDRAM_HandleStruct.Instance                = FMC_SDRAM_DEVICE;
  SDRAM_HandleStruct.Init.SDBank             = FMC_SDRAM_BANK1;
  SDRAM_HandleStruct.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
  SDRAM_HandleStruct.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_11;
  SDRAM_HandleStruct.Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32;
  SDRAM_HandleStruct.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  SDRAM_HandleStruct.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;
  SDRAM_HandleStruct.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  SDRAM_HandleStruct.Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;
  SDRAM_HandleStruct.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
  SDRAM_HandleStruct.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;

  BSP_SDRAM_MspInit(&SDRAM_HandleStruct, (void *)NULL);
  if(HAL_SDRAM_Init(&SDRAM_HandleStruct, &FMC_SDRAM_TimingStruct) != HAL_OK)
    state = HAL_ERROR;
  else
    state = HAL_OK;

  BSP_SDRAM_Initialization_sequence(REFRESH_COUNT);

  return state;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
uint8_t SDRAM_DeInit( void )
{
  static uint8_t state = HAL_ERROR;
  SDRAM_HandleStruct.Instance = FMC_SDRAM_DEVICE;

  if(HAL_SDRAM_DeInit(&SDRAM_HandleStruct) == HAL_OK) {
    state = HAL_OK;
    BSP_SDRAM_MspDeInit(&SDRAM_HandleStruct, (void *)NULL);
  }

  return state;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
#define SDRAM_TIMEOUT      ((uint32_t)0xFFFF)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

void BSP_SDRAM_Initialization_sequence( uint32_t RefreshCount )
{
  __IO uint32_t tmpmrd = 0;

  FMC_SDRAM_CommandStruct.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
  FMC_SDRAM_CommandStruct.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  FMC_SDRAM_CommandStruct.AutoRefreshNumber      = 1;
  FMC_SDRAM_CommandStruct.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&SDRAM_HandleStruct, &FMC_SDRAM_CommandStruct, SDRAM_TIMEOUT);

  HAL_Delay(1);

  FMC_SDRAM_CommandStruct.CommandMode            = FMC_SDRAM_CMD_PALL;
  FMC_SDRAM_CommandStruct.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  FMC_SDRAM_CommandStruct.AutoRefreshNumber      = 1;
  FMC_SDRAM_CommandStruct.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&SDRAM_HandleStruct, &FMC_SDRAM_CommandStruct, SDRAM_TIMEOUT);

  FMC_SDRAM_CommandStruct.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  FMC_SDRAM_CommandStruct.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  FMC_SDRAM_CommandStruct.AutoRefreshNumber      = 8;
  FMC_SDRAM_CommandStruct.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&SDRAM_HandleStruct, &FMC_SDRAM_CommandStruct, SDRAM_TIMEOUT);

  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |\
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |\
                     SDRAM_MODEREG_CAS_LATENCY_3           |\
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |\
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  FMC_SDRAM_CommandStruct.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
  FMC_SDRAM_CommandStruct.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  FMC_SDRAM_CommandStruct.AutoRefreshNumber      = 1;
  FMC_SDRAM_CommandStruct.ModeRegisterDefinition = tmpmrd;
  HAL_SDRAM_SendCommand(&SDRAM_HandleStruct, &FMC_SDRAM_CommandStruct, SDRAM_TIMEOUT);
  HAL_SDRAM_ProgramRefreshRate(&SDRAM_HandleStruct, RefreshCount);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
__weak void BSP_SDRAM_MspInit( SDRAM_HandleTypeDef  *hsdram, void *Params )
{
  static DMA_HandleTypeDef DMA_HandleStruct;
  GPIO_InitTypeDef GPIO_InitStruct;

  if(hsdram != (SDRAM_HandleTypeDef *)NULL) {
    __HAL_RCC_FMC_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    DMA_HandleStruct.Instance                 = DMA2_Stream0;
    DMA_HandleStruct.Init.Channel             = DMA_CHANNEL_0;
    DMA_HandleStruct.Init.Direction           = DMA_MEMORY_TO_MEMORY;
    DMA_HandleStruct.Init.PeriphInc           = DMA_PINC_ENABLE;
    DMA_HandleStruct.Init.MemInc              = DMA_MINC_ENABLE;
    DMA_HandleStruct.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    DMA_HandleStruct.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    DMA_HandleStruct.Init.Mode                = DMA_NORMAL;
    DMA_HandleStruct.Init.Priority            = DMA_PRIORITY_HIGH;
    DMA_HandleStruct.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    DMA_HandleStruct.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    DMA_HandleStruct.Init.MemBurst            = DMA_MBURST_SINGLE;
    DMA_HandleStruct.Init.PeriphBurst         = DMA_PBURST_SINGLE;
 
    __HAL_LINKDMA(hsdram, hdma, DMA_HandleStruct);

    HAL_DMA_DeInit(&DMA_HandleStruct);
    HAL_DMA_Init(&DMA_HandleStruct);

    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  }
}

__weak void BSP_SDRAM_MspDeInit( SDRAM_HandleTypeDef *hsdram, void *Params )
{
  static DMA_HandleTypeDef DMA_HandleStruct;

  if(hsdram != (SDRAM_HandleTypeDef *)NULL) {
    HAL_NVIC_DisableIRQ(DMA2_Stream0_IRQn);
    DMA_HandleStruct.Instance = DMA2_Stream0;
    HAL_DMA_DeInit(&DMA_HandleStruct);
  }
}
/*=====================================================================================================*/
/*=====================================================================================================*/
uint8_t SDRAM_ReadData( uint32_t address, uint32_t *pData, uint32_t lens )
{
  return HAL_SDRAM_Read_32b(&SDRAM_HandleStruct, (uint32_t *)address, pData, lens);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
uint8_t SDRAM_ReadDataDMA( uint32_t address, uint32_t *pData, uint32_t lens )
{
  return HAL_SDRAM_Read_DMA(&SDRAM_HandleStruct, (uint32_t *)address, pData, lens);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
uint8_t SDRAM_WriteData( uint32_t address, uint32_t *pData, uint32_t lens )
{
  return HAL_SDRAM_Write_32b(&SDRAM_HandleStruct, (uint32_t *)address, pData, lens);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
uint8_t SDRAM_WriteDataDMA( uint32_t address, uint32_t *pData, uint32_t lens )
{
  return HAL_SDRAM_Write_DMA(&SDRAM_HandleStruct, (uint32_t *)address, pData, lens);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
uint8_t SDRAM_SendCmd( FMC_SDRAM_CommandTypeDef *sdramCmd )
{
  return HAL_SDRAM_SendCommand(&SDRAM_HandleStruct, sdramCmd, SDRAM_TIMEOUT);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
//void BSP_SDRAM_DMA_IRQHandler( void )
//{
//  HAL_DMA_IRQHandler(SDRAM_HandleStruct.hdma);
//}
/*=====================================================================================================*/
/*=====================================================================================================*/
