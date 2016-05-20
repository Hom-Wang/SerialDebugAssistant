/*=====================================================================================================*/
/*=====================================================================================================*/
#include "drivers\stm32f4_system.h"
#include "drivers\stm32f4_sdram.h"

#include "module_otm8009.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
#define RAM_ADDR(__ACT_LAYER)         (LTDC_HandleStruct.LayerCfg[__ACT_LAYER].FBStartAdress)
#define POS_ADDR(__POSX, __POSY)      (__POSY * LCD_W + __POSX)

#define LCD_FB_START_ADDRESS          ((uint32_t)0xC0000000)
#define LTDC_MAX_LAYER_NUMBER         ((uint32_t)2)
#define LTDC_ACTIVE_LAYER_BACKGROUND  ((uint32_t)0)
#define LTDC_ACTIVE_LAYER_FOREGROUND  ((uint32_t)1)
#define LTDC_NB_OF_LAYERS             ((uint32_t)2)
#define LTDC_DEFAULT_ACTIVE_LAYER     LTDC_ACTIVE_LAYER_FOREGROUND
/*=====================================================================================================*/
/*=====================================================================================================*/
DMA2D_HandleTypeDef DMA2D_HandleStruct;
LTDC_HandleTypeDef LTDC_HandleStruct;
DSI_HandleTypeDef DSI_HandleStruct;

static DSI_VidCfgTypeDef DSI_VidCfgStruct;

static LCD_DrawPropTypeDef DrawProp[LTDC_MAX_LAYER_NUMBER];
static uint32_t activeLayer = LTDC_ACTIVE_LAYER_BACKGROUND;

void BSP_LCD_MspDeInit( void );
void BSP_LCD_MspInit( void );
/*====================================================================================================*/
/*====================================================================================================*
**函數 : DSI_WriteCmd
**功能 : Write Command
**輸入 : WriteCmd
**輸出 : None
**使用 : DSI_WriteCmd(0x00, 0x00);
**====================================================================================================*/
/*====================================================================================================*/
static void DSI_WriteCmd( uint8_t cmdH, uint8_t cmdL )
{
  HAL_DSI_ShortWrite(&DSI_HandleStruct, 0, DSI_DCS_SHORT_PKT_WRITE_P1, cmdH, cmdL);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : DSI_WriteData
**功能 : Write Data
**輸入 : WriteData
**輸出 : None
**使用 : DSI_WriteData(data, lens);
**====================================================================================================*/
/*====================================================================================================*/
static void DSI_WriteData( uint32_t lens, const uint8_t *pData )
{
  HAL_DSI_LongWrite(&DSI_HandleStruct, 0, DSI_DCS_LONG_PKT_WRITE, lens, pData[lens], (uint8_t *)pData); 
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_LayerDefaultInit
**功能 : LCD Layer Default Init
**輸入 : layerIndex, FB_Address
**輸出 : None
**使用 : LCD_LayerDefaultInit();
**=====================================================================================================*/
/*=====================================================================================================*/
static void LCD_LayerDefaultInit( uint16_t layerIndex, uint32_t FB_Address )
{
  LTDC_LayerCfgTypeDef Layercfg;

  /* Layer Init */
  Layercfg.WindowX0        = 0;
  Layercfg.WindowX1        = LCD_W;
  Layercfg.WindowY0        = 0;
  Layercfg.WindowY1        = LCD_H;
  Layercfg.PixelFormat     = LTDC_PIXEL_FORMAT_ARGB8888;
  Layercfg.FBStartAdress   = FB_Address;
  Layercfg.Alpha           = 255;
  Layercfg.Alpha0          = 0;
  Layercfg.Backcolor.Blue  = 0;
  Layercfg.Backcolor.Green = 0;
  Layercfg.Backcolor.Red   = 0;
  Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  Layercfg.ImageWidth      = LCD_W;
  Layercfg.ImageHeight     = LCD_H;
  
  HAL_LTDC_ConfigLayer(&LTDC_HandleStruct, &Layercfg, layerIndex); 

  DrawProp[layerIndex].fontColor = LCD_BLACK;
  DrawProp[layerIndex].backColor = LCD_WHITE;
  DrawProp[layerIndex].pFont     = &Font24;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_Init
**功能 : LCD Init
**輸入 : None
**輸出 : None
**使用 : LCD_Init();
**=====================================================================================================*/
/*=====================================================================================================*/
#define OTM8009A_ORIENTATION_PORTRAIT   ((uint32_t)0x00) /* Portrait orientation choice of LCD screen  */
#define OTM8009A_ORIENTATION_LANDSCAPE  ((uint32_t)0x01) /* Landscape orientation choice of LCD screen */

#define OTM8009A_FORMAT_RGB888          ((uint32_t)0x00) /* Pixel format chosen is RGB888 : 24 bpp */
#define OTM8009A_FORMAT_RBG565          ((uint32_t)0x02) /* Pixel format chosen is RGB565 : 16 bpp */

#define OTM8009A_CMD_NOP                   0x00  /* NOP command      */
#define OTM8009A_CMD_SWRESET               0x01  /* Sw reset command */
#define OTM8009A_CMD_RDDMADCTL             0x0B  /* Read Display MADCTR command : read memory display access ctrl */
#define OTM8009A_CMD_RDDCOLMOD             0x0C  /* Read Display pixel format */
#define OTM8009A_CMD_SLPIN                 0x10  /* Sleep In command */
#define OTM8009A_CMD_SLPOUT                0x11  /* Sleep Out command */
#define OTM8009A_CMD_PTLON                 0x12  /* Partial mode On command */
#define OTM8009A_CMD_DISPOFF               0x28  /* Display Off command */
#define OTM8009A_CMD_DISPON                0x29  /* Display On command */
#define OTM8009A_CMD_CASET                 0x2A  /* Column address set command */
#define OTM8009A_CMD_PASET                 0x2B  /* Page address set command */
#define OTM8009A_CMD_RAMWR                 0x2C  /* Memory (GRAM) write command */
#define OTM8009A_CMD_RAMRD                 0x2E  /* Memory (GRAM) read command  */
#define OTM8009A_CMD_PLTAR                 0x30  /* Partial area command (4 parameters) */
#define OTM8009A_CMD_TEOFF                 0x34  /* Tearing Effect Line Off command : command with no parameter */
#define OTM8009A_CMD_TEEON                 0x35  /* Tearing Effect Line On command : command with 1 parameter 'TELOM' */
#define OTM8009A_CMD_MADCTR                0x36  /* Memory Access write control command  */
#define OTM8009A_CMD_IDMOFF                0x38  /* Idle mode Off command */
#define OTM8009A_CMD_IDMON                 0x39  /* Idle mode On command  */
#define OTM8009A_CMD_COLMOD                0x3A  /* Interface Pixel format command */
#define OTM8009A_MADCTR_MODE_PORTRAIT      0x00
#define OTM8009A_MADCTR_MODE_LANDSCAPE     0x60  /* MY = 0, MX = 1, MV = 1, ML = 0, RGB = 0 */
#define OTM8009A_COLMOD_RGB565             0x55
#define OTM8009A_COLMOD_RGB888             0x77
#define OTM8009A_CMD_RAMWRC                0x3C  /* Memory write continue command */
#define OTM8009A_CMD_RAMRDC                0x3E  /* Memory read continue command  */
#define OTM8009A_CMD_WRTESCN               0x44  /* Write Tearing Effect Scan line command */
#define OTM8009A_CMD_RDSCNL                0x45  /* Read  Tearing Effect Scan line command */
#define OTM8009A_CMD_WRDISBV               0x51  /* Write Display Brightness command          */
#define OTM8009A_CMD_WRCTRLD               0x53  /* Write CTRL Display command                */
#define OTM8009A_CMD_WRCABC                0x55  /* Write Content Adaptive Brightness command */
#define OTM8009A_CMD_WRCABCMB              0x5E  /* Write CABC Minimum Brightness command     */

typedef enum {
  LCD_DSI_PIXEL_DATA_FMT_RBG888  = 0x00,
  LCD_DSI_PIXEL_DATA_FMT_RBG565  = 0x02,
  LCD_DSI_PIXEL_DATA_FMT_INVALID = 0x03
} LCD_DsiPixelDataFmtTypeDef;

const uint8_t lcdRegData1[]  = {0x80,0x09,0x01,0xFF};
const uint8_t lcdRegData2[]  = {0x80,0x09,0xFF};
const uint8_t lcdRegData3[]  = {0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01,0xE1};
const uint8_t lcdRegData4[]  = {0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01,0xE2};
const uint8_t lcdRegData5[]  = {0x79,0x79,0xD8};
const uint8_t lcdRegData6[]  = {0x00,0x01,0xB3};
const uint8_t lcdRegData7[]  = {0x85,0x01,0x00,0x84,0x01,0x00,0xCE};
const uint8_t lcdRegData8[]  = {0x18,0x04,0x03,0x39,0x00,0x00,0x00,0x18,0x03,0x03,0x3A,0x00,0x00,0x00,0xCE};
const uint8_t lcdRegData9[]  = {0x18,0x02,0x03,0x3B,0x00,0x00,0x00,0x18,0x01,0x03,0x3C,0x00,0x00,0x00,0xCE};
const uint8_t lcdRegData10[] = {0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00,0xCF};
const uint8_t lcdRegData11[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData12[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData13[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData14[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData15[] = {0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData16[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData17[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB};
const uint8_t lcdRegData18[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCB};
const uint8_t lcdRegData19[] = {0x00,0x26,0x09,0x0B,0x01,0x25,0x00,0x00,0x00,0x00,0xCC};
const uint8_t lcdRegData20[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0A,0x0C,0x02,0xCC};
const uint8_t lcdRegData21[] = {0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC};
const uint8_t lcdRegData22[] = {0x00,0x25,0x0C,0x0A,0x02,0x26,0x00,0x00,0x00,0x00,0xCC};
const uint8_t lcdRegData23[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0B,0x09,0x01,0xCC};
const uint8_t lcdRegData24[] = {0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC};
const uint8_t lcdRegData25[] = {0xFF,0xFF,0xFF,0xFF};
const uint8_t lcdRegData27[] = {0x00,0x00,0x03,0x1F,OTM8009A_CMD_CASET};
const uint8_t lcdRegData28[] = {0x00,0x00,0x01,0xDF,OTM8009A_CMD_PASET};

uint8_t LCD_Init( void )
{
  DSI_PLLInitTypeDef DSI_PLLInitStruct;
  static RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  uint32_t laneByteClk_kHz = 62500;
  uint32_t Clockratio = laneByteClk_kHz / 27429;
  uint32_t VSA = 12;
  uint32_t VBP = 12;
  uint32_t VFP = 12;
  uint32_t HSA = 120;
  uint32_t HBP = 120;
  uint32_t HFP = 120;
  uint32_t VACT = LCD_H;
  uint32_t HACT = LCD_W;

  BSP_LCD_MspInit();

  DSI_HandleStruct.Instance           = DSI;
  DSI_HandleStruct.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
  DSI_HandleStruct.Init.TXEscapeCkdiv = laneByteClk_kHz / 15620;
  HAL_DSI_DeInit(&DSI_HandleStruct);

  DSI_PLLInitStruct.PLLNDIV = 125;
  DSI_PLLInitStruct.PLLIDF  = DSI_PLL_IN_DIV2;
  DSI_PLLInitStruct.PLLODF  = DSI_PLL_OUT_DIV1;
  HAL_DSI_Init(&DSI_HandleStruct, &DSI_PLLInitStruct);

  DSI_VidCfgStruct.VirtualChannelID             = 0;
  DSI_VidCfgStruct.ColorCoding                  = LCD_DSI_PIXEL_DATA_FMT_RBG888;
  DSI_VidCfgStruct.VSPolarity                   = DSI_VSYNC_ACTIVE_HIGH;
  DSI_VidCfgStruct.HSPolarity                   = DSI_HSYNC_ACTIVE_HIGH;
  DSI_VidCfgStruct.DEPolarity                   = DSI_DATA_ENABLE_ACTIVE_HIGH;  
  DSI_VidCfgStruct.Mode                         = DSI_VID_MODE_BURST;
  DSI_VidCfgStruct.NullPacketSize               = 0xFFF;
  DSI_VidCfgStruct.NumberOfChunks               = 0;
  DSI_VidCfgStruct.PacketSize                   = HACT;
  DSI_VidCfgStruct.HorizontalSyncActive         = HSA * Clockratio;
  DSI_VidCfgStruct.HorizontalBackPorch          = HBP * Clockratio;
  DSI_VidCfgStruct.HorizontalLine               = (HACT + HSA + HBP + HFP)*Clockratio;
  DSI_VidCfgStruct.VerticalSyncActive           = VSA;
  DSI_VidCfgStruct.VerticalBackPorch            = VBP;
  DSI_VidCfgStruct.VerticalFrontPorch           = VFP;
  DSI_VidCfgStruct.VerticalActive               = VACT;
  DSI_VidCfgStruct.LPCommandEnable              = DSI_LP_COMMAND_ENABLE;
  DSI_VidCfgStruct.LPLargestPacketSize          = 64;
  DSI_VidCfgStruct.LPVACTLargestPacketSize      = 64;
  DSI_VidCfgStruct.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;
  DSI_VidCfgStruct.LPHorizontalBackPorchEnable  = DSI_LP_HBP_ENABLE;
  DSI_VidCfgStruct.LPVerticalActiveEnable       = DSI_LP_VACT_ENABLE;
  DSI_VidCfgStruct.LPVerticalFrontPorchEnable   = DSI_LP_VFP_ENABLE;
  DSI_VidCfgStruct.LPVerticalBackPorchEnable    = DSI_LP_VBP_ENABLE;
  DSI_VidCfgStruct.LPVerticalSyncActiveEnable   = DSI_LP_VSYNC_ENABLE;
  HAL_DSI_ConfigVideoMode(&DSI_HandleStruct, &DSI_VidCfgStruct);

  HAL_DSI_Start(&DSI_HandleStruct);

  PeriphClkInitStruct.PeriphClockSelection  = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN        = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIR        = 7;
  PeriphClkInitStruct.PLLSAIDivR            = RCC_PLLSAIDIVR_2;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct); 

  LTDC_HandleStruct.Instance                = LTDC;
  LTDC_HandleStruct.Init.Backcolor.Blue     = 0;
  LTDC_HandleStruct.Init.Backcolor.Green    = 0;
  LTDC_HandleStruct.Init.Backcolor.Red      = 0;
  LTDC_HandleStruct.Init.PCPolarity         = LTDC_PCPOLARITY_IPC;
  LTDC_HandleStruct.Init.HorizontalSync     = (HSA - 1);
  LTDC_HandleStruct.Init.AccumulatedHBP     = (HSA + HBP - 1);
  LTDC_HandleStruct.Init.AccumulatedActiveW = (LCD_W + HSA + HBP - 1);
  LTDC_HandleStruct.Init.TotalWidth         = (LCD_W + HSA + HBP + HFP - 1);
  LTDC_HandleStruct.LayerCfg->ImageWidth    = LCD_W;
  LTDC_HandleStruct.LayerCfg->ImageHeight   = LCD_H;
  HAL_LTDC_StructInitFromVideoConfig(&LTDC_HandleStruct, &DSI_VidCfgStruct);
  HAL_LTDC_Init(&LTDC_HandleStruct);

  SDRAM_Init();

  // OTM8009 Init
  DSI_WriteCmd(0x00, 0x00);
  DSI_WriteData(3,   lcdRegData1);
  DSI_WriteCmd(0x00, 0x80);
  DSI_WriteData(2,   lcdRegData2);
  DSI_WriteCmd(0x00, 0x80);
  DSI_WriteCmd(0xC4, 0x30);
  HAL_Delay(10);
  DSI_WriteCmd(0x00, 0x8A);
  DSI_WriteCmd(0xC4, 0x40);
  HAL_Delay(10);
  DSI_WriteCmd(0x00, 0xB1);
  DSI_WriteCmd(0xC5, 0xA9);
  DSI_WriteCmd(0x00, 0x91);
  DSI_WriteCmd(0xC5, 0x34);
  DSI_WriteCmd(0x00, 0xB4);
  DSI_WriteCmd(0xC0, 0x50);
  DSI_WriteCmd(0x00, 0x00);
  DSI_WriteCmd(0xD9, 0x4E);
  DSI_WriteCmd(0x00, 0x81);
  DSI_WriteCmd(0xC1, 0x66);
  DSI_WriteCmd(0x00, 0xA1);
  DSI_WriteCmd(0xC1, 0x08);
  DSI_WriteCmd(0x00, 0x92);
  DSI_WriteCmd(0xC5, 0x01);
  DSI_WriteCmd(0x00, 0x95);
  DSI_WriteCmd(0xC5, 0x34);
  DSI_WriteCmd(0x00, 0x00);
  DSI_WriteCmd(0xC4, 0x40);
  DSI_WriteCmd(0x00, 0x94);
  DSI_WriteCmd(0xC5, 0x33);
  DSI_WriteCmd(0x00, 0xA3);
  DSI_WriteCmd(0xC0, 0x1B);
  DSI_WriteCmd(0x00, 0x82);
  DSI_WriteCmd(0xC5, 0x83);
  DSI_WriteCmd(0x00, 0x81);
  DSI_WriteCmd(0xC4, 0x83);
  DSI_WriteCmd(0x00, 0xA1);
  DSI_WriteCmd(0xC1, 0x0E);
  DSI_WriteCmd(0x00, 0xA6);
  DSI_WriteData(2,   lcdRegData6);
  DSI_WriteCmd(0x00, 0x80);
  DSI_WriteData(6,   lcdRegData7);
  DSI_WriteCmd(0x00, 0xA0);
  DSI_WriteData(14,  lcdRegData8);
  DSI_WriteCmd(0x00, 0xB0);
  DSI_WriteData(14,  lcdRegData9);
  DSI_WriteCmd(0x00, 0xC0);
  DSI_WriteData(10,  lcdRegData10);
  DSI_WriteCmd(0x00, 0xD0);
  DSI_WriteCmd(0xCF, 0x00);
  DSI_WriteCmd(0x00, 0x80);
  DSI_WriteData(10,  lcdRegData11);
  DSI_WriteCmd(0x00, 0x90);
  DSI_WriteData(15,  lcdRegData12);
  DSI_WriteCmd(0x00, 0xA0);
  DSI_WriteData(15,  lcdRegData13);
  DSI_WriteCmd(0x00, 0xB0);
  DSI_WriteData(10,  lcdRegData14);
  DSI_WriteCmd(0x00, 0xC0);
  DSI_WriteData(15,  lcdRegData15);
  DSI_WriteCmd(0x00, 0xD0);
  DSI_WriteData(15,  lcdRegData16);
  DSI_WriteCmd(0x00, 0xE0);
  DSI_WriteData(10,  lcdRegData17);
  DSI_WriteCmd(0x00, 0xF0);
  DSI_WriteData(10,  lcdRegData18);
  DSI_WriteCmd(0x00, 0x80);
  DSI_WriteData(10,  lcdRegData19);
  DSI_WriteCmd(0x00, 0x90);
  DSI_WriteData(15,  lcdRegData20);
  DSI_WriteCmd(0x00, 0xA0);
  DSI_WriteData(15,  lcdRegData21);
  DSI_WriteCmd(0x00, 0xB0);
  DSI_WriteData(10,  lcdRegData22);
  DSI_WriteCmd(0x00, 0xC0);
  DSI_WriteData(15,  lcdRegData23);
  DSI_WriteCmd(0x00, 0xD0);
  DSI_WriteData(15,  lcdRegData24);
  DSI_WriteCmd(0x00, 0x81);
  DSI_WriteCmd(0xC5, 0x66);
  DSI_WriteCmd(0x00, 0xB6);
  DSI_WriteCmd(0xF5, 0x06);
  DSI_WriteCmd(0x00, 0x00);
  DSI_WriteData(3,   lcdRegData25);
  DSI_WriteCmd(0x00, 0x00);
  DSI_WriteCmd(0x00, 0x00);
  DSI_WriteData(16,  lcdRegData3);
  DSI_WriteCmd(0x00, 0x00);
  DSI_WriteData(16,  lcdRegData4);
  DSI_WriteCmd(OTM8009A_CMD_SLPOUT, 0x00);
  HAL_Delay(120);

  switch(DSI_VidCfgStruct.ColorCoding) {
    case OTM8009A_FORMAT_RBG565:
      DSI_WriteCmd(OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB565);
      break;
    case OTM8009A_FORMAT_RGB888:
      DSI_WriteCmd(OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB888);
      break;
    default:
      break;
  }

#ifdef LCD_ORIENTATION_LANDSCAPE
  DSI_WriteCmd(OTM8009A_CMD_MADCTR, OTM8009A_MADCTR_MODE_LANDSCAPE);
  DSI_WriteData(4,  lcdRegData27);
  DSI_WriteData(4,  lcdRegData28);
#endif

  DSI_WriteCmd(OTM8009A_CMD_WRDISBV, 0x7F);
  DSI_WriteCmd(OTM8009A_CMD_WRCTRLD, 0x2C);
  DSI_WriteCmd(OTM8009A_CMD_WRCABC, 0x02);
  DSI_WriteCmd(OTM8009A_CMD_WRCABCMB, 0xFF);
  DSI_WriteCmd(OTM8009A_CMD_DISPON, 0x00);
  DSI_WriteCmd(0x00, 0x00);
  DSI_WriteCmd(OTM8009A_CMD_RAMWR, 0x00);

  LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
  LCD_Clear(LCD_WHITE);

  return HAL_OK;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
__weak void BSP_LCD_MspInit( void )
{
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_LTDC_CLK_ENABLE();
  __HAL_RCC_LTDC_FORCE_RESET();
  __HAL_RCC_LTDC_RELEASE_RESET();
  __HAL_RCC_DMA2D_CLK_ENABLE();
  __HAL_RCC_DMA2D_FORCE_RESET();
  __HAL_RCC_DMA2D_RELEASE_RESET();
  __HAL_RCC_DSI_CLK_ENABLE();
  __HAL_RCC_DSI_FORCE_RESET();
  __HAL_RCC_DSI_RELEASE_RESET();

  GPIO_InitTypeDef gpio_init_structure;

  /* Configure the GPIO on PH7 - RST */
  gpio_init_structure.Pin   = GPIO_PIN_7;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_OD;
  gpio_init_structure.Pull  = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_Delay(20);
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_Delay(10);    

  HAL_NVIC_SetPriority(LTDC_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(LTDC_IRQn);

  HAL_NVIC_SetPriority(DMA2D_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DMA2D_IRQn);

  HAL_NVIC_SetPriority(DSI_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DSI_IRQn);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
__weak void BSP_LCD_MspDeInit( void )
{
  HAL_NVIC_DisableIRQ(LTDC_IRQn);
  HAL_NVIC_DisableIRQ(DMA2D_IRQn);
  HAL_NVIC_DisableIRQ(DSI_IRQn);

  __HAL_RCC_LTDC_FORCE_RESET();
  __HAL_RCC_DMA2D_FORCE_RESET();
  __HAL_RCC_DSI_FORCE_RESET();
  __HAL_RCC_LTDC_CLK_DISABLE();
  __HAL_RCC_DMA2D_CLK_DISABLE();
  __HAL_RCC_DSI_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_FillBuffer
**功能 : Clear Windows
**輸入 : Color
**輸出 : None
**使用 : LCD_FillBuffer(layerIndex, pDst, xSize, ySize, offLine, color);
**====================================================================================================*/
/*====================================================================================================*/
static void LCD_FillBuffer( uint32_t layerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t offLine, uint32_t color )
{
  DMA2D_HandleStruct.Instance          = DMA2D;
  DMA2D_HandleStruct.Init.Mode         = DMA2D_R2M;
  DMA2D_HandleStruct.Init.ColorMode    = DMA2D_ARGB8888;
  DMA2D_HandleStruct.Init.OutputOffset = offLine;

  if(HAL_DMA2D_Init(&DMA2D_HandleStruct) == HAL_OK) {
    if(HAL_DMA2D_ConfigLayer(&DMA2D_HandleStruct, layerIndex) == HAL_OK) {
      if(HAL_DMA2D_Start(&DMA2D_HandleStruct, color, (uint32_t)pDst, xSize, ySize) == HAL_OK) {
        HAL_DMA2D_PollForTransfer(&DMA2D_HandleStruct, 10);
      }
    }
  }
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_Clear
**功能 : Clear Windows
**輸入 : Color
**輸出 : None
**使用 : LCD_Clear(BLACK);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_Clear( uint32_t color )
{
  LCD_FillBuffer(activeLayer, (uint32_t *)RAM_ADDR(activeLayer), LCD_W, LCD_H, 0, color);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_Display
**功能 : Display
**輸入 : onoff
**輸出 : None
**使用 : LCD_Display(ENABLE);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_Display( uint8_t onoff )
{
  if(onoff == ENABLE)
    HAL_DSI_ShortWrite(&DSI_HandleStruct, DSI_VidCfgStruct.VirtualChannelID, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_DISPON,  0x00);
  else
    HAL_DSI_ShortWrite(&DSI_HandleStruct, DSI_VidCfgStruct.VirtualChannelID, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_DISPOFF, 0x00);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_SelectLayer
**功能 : Select Layer
**輸入 : layerIndex
**輸出 : None
**使用 : LCD_SelectLayer(0);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_SelectLayer( uint32_t layerIndex )
{
  activeLayer = layerIndex;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_SetLayerWindow
**功能 : Set Layer Window
**輸入 : posX, posY, width, height
**輸出 : None
**使用 : LCD_SetLayerWindow(layerIndex, posX, posY, width, height);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_SetLayerWindow( uint16_t layerIndex, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height )
{
  HAL_LTDC_SetWindowSize(&LTDC_HandleStruct, width, height, layerIndex);
  HAL_LTDC_SetWindowPosition(&LTDC_HandleStruct, posX, posY, layerIndex);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_SetTextColor
**功能 : Set Text Color
**輸入 : color
**輸出 : None
**使用 : LCD_SetTextColor(color);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_SetTextColor( uint32_t color )
{
  DrawProp[activeLayer].fontColor = color;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_SetBackColor
**功能 : Set Back Color
**輸入 : color
**輸出 : None
**使用 : LCD_SetBackColor(color);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_SetBackColor( uint32_t color )
{
  DrawProp[activeLayer].backColor = color;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_SetFont
**功能 : Set Font
**輸入 : *fonts
**輸出 : None
**使用 : LCD_SetFont(fonts);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_SetFont( sFONT *fonts )
{
  DrawProp[activeLayer].pFont = fonts;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_SetLayerAddress
**功能 : Set Layer Address
**輸入 : layerIndex, address
**輸出 : None
**使用 : LCD_SetLayerAddress(LTDC_ACTIVE_LAYER_BACKGROUND, addresss);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_SetLayerAddress( uint32_t layerIndex, uint32_t address )
{
  HAL_LTDC_SetAddress(&LTDC_HandleStruct, address, layerIndex);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_SetLayerVisible
**功能 : Set Layer Visible
**輸入 : layerIndex, state
**輸出 : None
**使用 : LCD_SetLayerVisible(LTDC_ACTIVE_LAYER_BACKGROUND, ENABLE);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_SetLayerVisible( uint32_t layerIndex, FunctionalState state )
{
  if(state == ENABLE)
    __HAL_LTDC_LAYER_ENABLE(&LTDC_HandleStruct, layerIndex);
  else
    __HAL_LTDC_LAYER_DISABLE(&LTDC_HandleStruct, layerIndex);

  __HAL_LTDC_RELOAD_CONFIG(&LTDC_HandleStruct);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_SetTransparency
**功能 : Set Transparency
**輸入 : layerIndex, transparency
**輸出 : None
**使用 : LCD_SetTransparency(LTDC_ACTIVE_LAYER_BACKGROUND, 128);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_SetTransparency( uint32_t layerIndex, uint8_t transparency )
{
  HAL_LTDC_SetAlpha(&LTDC_HandleStruct, transparency, layerIndex);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_GetTextColor
**功能 : Get Text Color
**輸入 : None
**輸出 : color
**使用 : color = LCD_GetTextColor();
**====================================================================================================*/
/*====================================================================================================*/
uint32_t LCD_GetTextColor( void )
{
  return DrawProp[activeLayer].fontColor;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_GetBackColor
**功能 : Get Back Color
**輸入 : None
**輸出 : color
**使用 : color = LCD_GetBackColor();
**====================================================================================================*/
/*====================================================================================================*/
uint32_t LCD_GetBackColor( void )
{
  return DrawProp[activeLayer].backColor;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : BSP_LCD_GetFont
**功能 : Get Text Color
**輸入 : None
**輸出 : *sFONT
**使用 : fonts = BSP_LCD_GetFont();
**====================================================================================================*/
/*====================================================================================================*/
sFONT *BSP_LCD_GetFont( void )
{
  return DrawProp[activeLayer].pFont;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_DrawPixel
**功能 : Draw a Pixel
**輸入 : posX, posY, color
**輸出 : None
**使用 : LCD_DrawPixel(posX, posY, color);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_DrawPixel( uint16_t posX, uint16_t posY, uint32_t color )
{
  *(__IO uint32_t*) (RAM_ADDR(activeLayer) + (POS_ADDR(posX, posY) << 2)) = color;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_ReadPixel
**功能 : Read a Pixel
**輸入 : None
**輸出 : color
**使用 : color = LCD_ReadPixel(posX, posY);
**====================================================================================================*/
/*====================================================================================================*/
uint32_t LCD_ReadPixel( uint16_t posX, uint16_t posY )
{
  return *(__IO uint32_t*) (RAM_ADDR(activeLayer) + (POS_ADDR(posX, posY) << 1));
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_DrawLineX
**功能 : Draw X-Axis Line
**輸入 : posX, posY, length, color
**輸出 : None
**使用 : LCD_DrawLineX(posX, posY, length, color);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_DrawLineX( uint16_t posX, uint16_t posY, uint16_t length, uint32_t color )
{
  uint32_t address = 0;

  address = RAM_ADDR(activeLayer) + (POS_ADDR(posX, posY) << 2);
  LCD_FillBuffer(activeLayer, (uint32_t *)address, length, 1, 0, color);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_DrawLineY
**功能 : Draw Y-Axis Line
**輸入 : posX, posY, length, color
**輸出 : None
**使用 : LCD_DrawLineY(posX, posY, length, color)
**====================================================================================================*/
/*====================================================================================================*/
void LCD_DrawLineY( uint16_t posX, uint16_t posY, uint16_t length, uint32_t color )
{
  uint32_t address = 0;

  address = RAM_ADDR(activeLayer) + (POS_ADDR(posX, posY) << 2);
  LCD_FillBuffer(activeLayer, (uint32_t *)address, 1, length, LCD_W - 1, color);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_DrawLine
**功能 : Draw Line
**輸入 : posX1, posY1, posX2, posY2, color
**輸出 : None
**使用 : LCD_DrawLine(posX1, posY1, posX2, posY2, color);
**====================================================================================================*/
/*====================================================================================================*/
//void BSP_LCD_DrawLine( uint16_t posX1, uint16_t posY1, uint16_t posX2, uint16_t posY2, uint32_t color )
//{
//  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
//  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, curpixel = 0;

//  deltax = ABS(posX2 - posX1);
//  deltay = ABS(posY2 - posY1);
//  x = posX1;
//  y = posY1;

//  if(posX2 >= posX1) {
//    xinc1 = 1;
//    xinc2 = 1;
//  }
//  else {
//    xinc1 = -1;
//    xinc2 = -1;
//  }

//  if(posY2 >= posY1) {
//    yinc1 = 1;
//    yinc2 = 1;
//  }
//  else {
//    yinc1 = -1;
//    yinc2 = -1;
//  }

//  if (deltax >= deltay) {
//    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
//    yinc2 = 0;                  /* Don't change the y for every iteration */
//    den = deltax;
//    num = deltax >> 1;
//    numadd = deltay;
//    numpixels = deltax;         /* There are more x-values than y-values */
//  }
//  else {
//    xinc2 = 0;                  /* Don't change the x for every iteration */
//    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
//    den = deltay;
//    num = deltay >> 1;
//    numadd = deltax;
//    numpixels = deltay;         /* There are more y-values than x-values */
//  }

//  for (curpixel = 0; curpixel <= numpixels; curpixel++)
//  {
//    LCD_DrawPixel(x, y, color);
//    num += numadd;                            /* Increase the numerator by the top of the fraction */
//    if(num >= den) {
//      num -= den;                             /* Calculate the new numerator value */
//      x += xinc1;                             /* Change the x as appropriate */
//      y += yinc1;                             /* Change the y as appropriate */
//    }
//    x += xinc2;                               /* Change the x as appropriate */
//    y += yinc2;                               /* Change the y as appropriate */
//  }
//}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_DrawRect
**功能 : Draw Rectangle
**輸入 : posX, posY, width, height, color
**輸出 : None
**使用 : LCD_DrawRect(posX, posY, width, height, color);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_DrawRect( uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t color )
{
  LCD_DrawLineX(posX,             posY,              width,  color);
  LCD_DrawLineX(posX,             posY + height - 1, width,  color);
  LCD_DrawLineY(posX,             posY,              height, color);
  LCD_DrawLineY(posX + width - 1, posY,              height, color);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_DrawRectFill
**功能 : Draw Rectangle
**輸入 : posX, posY, Length, width, height, color
**輸出 : None
**使用 : LCD_DrawRectFill(posX, posY, width, height, color);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_DrawRectFill( uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t color )
{
  uint32_t address = 0;

  address = RAM_ADDR(activeLayer) + (POS_ADDR(posX, posY) << 2);
  LCD_FillBuffer(activeLayer, (uint32_t *)address, width, height, (LCD_W - width), color);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_DrawCircle
**功能 : Draw Circle
**輸入 : posX, posY, radius, color
**輸出 : None
**使用 : LCD_DrawCircle(posX, posY, radius, color);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_DrawCircle( uint16_t posX, uint16_t posY, uint16_t radius, uint32_t color )
{
  int32_t D;
  uint32_t curX;
  uint32_t curY;

  D = 3 - (radius << 1);
  curX = 0;
  curY = radius;

  while(curX <= curY) {
    LCD_DrawPixel(posX + curX, posY - curY, color);
    LCD_DrawPixel(posX - curX, posY - curY, color);
    LCD_DrawPixel(posX + curY, posY - curX, color);
    LCD_DrawPixel(posX - curY, posY - curX, color);
    LCD_DrawPixel(posX + curX, posY + curY, color);
    LCD_DrawPixel(posX - curX, posY + curY, color);
    LCD_DrawPixel(posX + curY, posY + curX, color);
    LCD_DrawPixel(posX - curY, posY + curX, color);

    if(D < 0) {
      D += (curX << 2) + 6;
    }
    else {
      D += ((curX - curY) << 2) + 10;
      curY--;
    }
    curX++;
  }
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_DrawPicture
**功能 : Draw Picture
**輸入 : CoordiX, CoordiY, Length, Width, Pic
**輸出 : None
**使用 : LCD_DrawPicture(CoordiX, CoordiY, Width, Height, Pic);
**====================================================================================================*/
/*====================================================================================================*/
//void LCD_DrawPicture( uint16_t CoordiX, uint16_t CoordiY, uint16_t Width, uint16_t Height, const uint8_t *Pic )
//{
//  uint16_t *readPixel = (uint16_t*)Pic;
//  uint32_t i = 0, j = Height * Width;

//  LCD_SetWindow(CoordiX, CoordiY, CoordiX+Width-1, CoordiY+Height-1);

//  for(i = 0; i < j; i++)
//    LCD_WriteData(readPixel[i]);
//}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_PutChar
**功能 : Put Char
**輸入 : posX, posY, pChar, fontColor, backColor
**輸出 : None
**使用 : LCD_PutChar(posX, posY, 'C', LCD_WHITE, LCD_BLACK);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_PutChar( uint16_t posX, uint16_t posY, uint8_t word, uint32_t fontColor, uint32_t backColor )
{
  uint8_t  *pchar;
  uint16_t width    = DrawProp[activeLayer].pFont->Width;
  uint16_t height   = DrawProp[activeLayer].pFont->Height;
  uint16_t tmpWidth = (width + 7) >> 3;
  uint16_t offset   = (tmpWidth << 3) -  width;
  uint32_t line = 0;

  const uint8_t *pFontTable = &DrawProp[activeLayer].pFont->table[(word - ' ') * height * tmpWidth];

  for(uint32_t i = 0; i < height; i++) {
    pchar = ((uint8_t *)pFontTable + tmpWidth * i);

    switch(tmpWidth) {
      case 1:
        line = pchar[0];
        break;

      case 2:
        line = (pchar[0] << 8) | pchar[1];
        break;
      case 3:

      default:
        line = (pchar[0] << 16) | (pchar[1] << 8) | pchar[2];
        break;
    }

    for(uint32_t j = 0; j < width; j++) {
      if(line & (1 << (width - j + offset - 1)))
        LCD_DrawPixel((posX + j), posY, fontColor);
      else
        LCD_DrawPixel((posX + j), posY, backColor);
    }
    posY++;
  }
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_PutStr
**功能 : Put String
**輸入 : posX, posY, string, mode, fontColor, backColor
**輸出 : None
**使用 : LCD_PutStr(10, 10, "TFT LCD Font Test ...", LEFT_MODE, LCD_WHITE, LCD_BLACK);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_PutStr( uint16_t posX, uint16_t posY, char *string, Text_AlignModeTypdef mode, uint32_t fontColor, uint32_t backColor )
{
  uint16_t i         = 0;
  uint16_t refCol    = 1;
  uint16_t fontWidth = DrawProp[activeLayer].pFont->Width;
  uint32_t strLens   = lenOfStr(string);
  uint32_t xSize     = (LCD_W / fontWidth);

  switch(mode) {
    case CENTER_MODE:
        refCol = posX + (((xSize - strLens) * fontWidth) >> 1);
        break;
    case RIGHT_MODE:
        refCol = - posX + ((xSize - strLens) * fontWidth);
        break;
    case LEFT_MODE:
    default:
        refCol = posX;
        break;
  }

  if((refCol < 1) || (refCol >= 0x8000))
    refCol = 1;

  while((*string != 0) & (((LCD_W - (i * fontWidth)) & 0xFFFF) >= fontWidth)) {
    LCD_PutChar(refCol, posY, *string, fontColor, backColor);
    refCol += fontWidth;
    string++;
    i++;
  }

}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : LCD_PutNum
**功能 : Put Number
**輸入 : posX, posY, type, lens, number, fontColor, backColor
**輸出 : None
**使用 : LCD_PutNum(posX, posY, Type_D, lens, number, LCD_WHITE, LCD_BLACK);
**====================================================================================================*/
/*====================================================================================================*/
void LCD_PutNum( uint16_t posX, uint16_t posY, StringType type, uint8_t lens, uint32_t number, uint32_t fontColor, uint32_t backColor )
{
  char strBuf[16] = {0};

  num2Str(type, lens, strBuf, number);
  LCD_PutStr(posX, posY, strBuf, LEFT_MODE, fontColor, backColor);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_TestColorBar
**功能 : Draw Color Bar
**輸入 : None
**輸出 : None
**使用 : LCD_TestColorBar();
**=====================================================================================================*/
/*=====================================================================================================*/
#define COLOR_NUMBER      (25)
#define COLOR_BAR_HEIGHT  (LCD_W / COLOR_NUMBER)  // 800/25 = 32

void LCD_TestColorBar( void )
{
  uint32_t drawColor[COLOR_NUMBER] = {
    LCD_BLUE,
    LCD_GREEN,
    LCD_RED,
    LCD_CYAN,
    LCD_MAGENTA,
    LCD_YELLOW,
    LCD_LIGHTBLUE,
    LCD_LIGHTGREEN,
    LCD_LIGHTRED,
    LCD_LIGHTCYAN,
    LCD_LIGHTMAGENTA,
    LCD_LIGHTYELLOW,
    LCD_DARKBLUE,
    LCD_DARKGREEN,
    LCD_DARKRED,
    LCD_DARKCYAN,
    LCD_DARKMAGENTA,
    LCD_DARKYELLOW,
    LCD_WHITE,
    LCD_LIGHTGRAY,
    LCD_GRAY,
    LCD_DARKGRAY,
    LCD_BLACK,
    LCD_BROWN,
    LCD_ORANGE,
//    LCD_TRANSPARENT
  };

  for(uint8_t i = 0; i < COLOR_NUMBER; i++)
    LCD_DrawRectFill(i * COLOR_BAR_HEIGHT, 0, COLOR_BAR_HEIGHT, LCD_H, drawColor[i]);
}
/*====================================================================================================*/
/*====================================================================================================*/
void LCD_TestColorClear( void )
{
  uint32_t drawColor[26] = {
    LCD_BLUE,
    LCD_GREEN,
    LCD_RED,
    LCD_CYAN,
    LCD_MAGENTA,
    LCD_YELLOW,
    LCD_LIGHTBLUE,
    LCD_LIGHTGREEN,
    LCD_LIGHTRED,
    LCD_LIGHTCYAN,
    LCD_LIGHTMAGENTA,
    LCD_LIGHTYELLOW,
    LCD_DARKBLUE,
    LCD_DARKGREEN,
    LCD_DARKRED,
    LCD_DARKCYAN,
    LCD_DARKMAGENTA,
    LCD_DARKYELLOW,
    LCD_WHITE,
    LCD_LIGHTGRAY,
    LCD_GRAY,
    LCD_DARKGRAY,
    LCD_BLACK,
    LCD_BROWN,
    LCD_ORANGE,
    LCD_TRANSPARENT
  };

  for(uint32_t i = 0; i < 26; i++) {
    LCD_Clear(drawColor[i]);
    delay_ms(100);
  }
}
/*=====================================================================================================*/
/*=====================================================================================================*/
