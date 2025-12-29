/**
  ******************************************************************************
  * @file    usbd_tmc.h
  * @author  MCD Application Team
  * @brief   header file for the usbd_cdc.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_TMC_H
#define __USB_TMC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup usbd_cdc
  * @brief This file is the Header file for usbd_cdc.c
  * @{
  */


/** @defgroup usbd_tmc_Exported_Defines
  * @{
  */
#define TMC_IN_EP                                   0x81U  /* EP1 for data IN */
#define TMC_OUT_EP                                  0x01U  /* EP1 for data OUT */
#define TMC_CMD_EP                                  0x82U  /* EP2 for CDC commands */

#ifndef TMC_HS_BINTERVAL
#define TMC_HS_BINTERVAL                            0x10U
#endif /* TMC_HS_BINTERVAL */

#ifndef TMC_FS_BINTERVAL
#define TMC_FS_BINTERVAL                            0x10U
#endif /* TMC_FS_BINTERVAL */

/* CDC Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#define TMC_DATA_HS_MAX_PACKET_SIZE                 512U    /* Endpoint IN & OUT Packet size */
#define TMC_DATA_FS_MAX_PACKET_SIZE                 64U     /* Endpoint IN & OUT Packet size */
    /** 5.1.2 USB488 Interrupt-IN endpoint descriptor */
    /** Table 22 -- Interrupt-IN endpoint descriptor */
    /* If the device does not send vendor specific notifications, must be 0x02 */
#define TMC_CMD_PACKET_SIZE                         2U      /* Control Endpoint Packet size */

#define TMC_DATA_HS_IN_PACKET_SIZE                  TMC_DATA_HS_MAX_PACKET_SIZE
#define TMC_DATA_HS_OUT_PACKET_SIZE                 TMC_DATA_HS_MAX_PACKET_SIZE

#define TMC_DATA_FS_IN_PACKET_SIZE                  TMC_DATA_FS_MAX_PACKET_SIZE
#define TMC_DATA_FS_OUT_PACKET_SIZE                 TMC_DATA_FS_MAX_PACKET_SIZE

/*---------------------------------------------------------------------*/
/*  CDC definitions                                                    */
/*---------------------------------------------------------------------*/

/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */
typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} USBD_TMC_LineCodingTypeDef;

typedef struct _USBD_TMC_IF
{
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);
  int8_t (* Control)(uint8_t cmd, uint8_t *pbuf, uint16_t length);
  int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);
  int8_t (* TransmitCplt)(uint8_t *Buf, uint32_t *Len, uint8_t epnum);
} USBD_TMC_IfTypeDef;


typedef struct
{
  uint32_t data[TMC_DATA_HS_MAX_PACKET_SIZE / 4U];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;
} USBD_TMC_HandleTypeDef;



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef USBD_TMC;
#define USBD_TMC_CLASS &USBD_TMC
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t USBD_TMC_RegisterInterface(USBD_HandleTypeDef *pdev,
                                   USBD_TMC_IfTypeDef *fops);

uint8_t USBD_TMC_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff,
                             uint32_t length);

uint8_t USBD_TMC_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff);
uint8_t USBD_TMC_ReceivePacket(USBD_HandleTypeDef *pdev);
uint8_t USBD_TMC_TransmitPacket(USBD_HandleTypeDef *pdev);
uint8_t USBD_TMC_TransmitZLP(USBD_HandleTypeDef *pdev);
USBD_StatusTypeDef USBD_TMC_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_TMC_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
