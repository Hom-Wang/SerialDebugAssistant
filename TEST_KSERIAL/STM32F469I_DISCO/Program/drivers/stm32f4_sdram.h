/* #include "module_sdram.h" */

#ifndef __MODULE_SDRAM_H
#define __MODULE_SDRAM_H

#include "stm32f4xx_hal.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
uint8_t SDRAM_Init( void );
uint8_t SDRAM_ReadData( uint32_t address, uint32_t *pData, uint32_t lens );
uint8_t SDRAM_ReadDataDMA( uint32_t address, uint32_t *pData, uint32_t lens );
uint8_t SDRAM_WriteData( uint32_t address, uint32_t *pData, uint32_t lens );
uint8_t SDRAM_WriteDataDMA( uint32_t address, uint32_t *pData, uint32_t lens );
uint8_t SDRAM_SendCmd( FMC_SDRAM_CommandTypeDef *sdramCmd );

void        BSP_SDRAM_Initialization_sequence( uint32_t RefreshCount );
__weak void BSP_SDRAM_MspInit( SDRAM_HandleTypeDef  *hsdram, void *Params );
__weak void BSP_SDRAM_MspDeInit( SDRAM_HandleTypeDef *hsdram, void *Params );
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
