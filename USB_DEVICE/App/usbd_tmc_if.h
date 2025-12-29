/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_tmc_if.h
  * @version        : v1.0_Cube
  * @brief          : Header for usbd_tmc_if.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_TMC_IF_H__
#define __USBD_TMC_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_tmc.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief For Usb device.
  * @{
  */

/** @defgroup USBD_TMC_IF USBD_TMC_IF
  * @brief Usb VCP device module
  * @{
  */

/** @defgroup USBD_TMC_IF_Exported_Defines USBD_TMC_IF_Exported_Defines
  * @brief Defines.
  * @{
  */
/* USER CODE BEGIN EXPORTED_DEFINES */
/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#define APP_RX_DATA_SIZE    USB_FS_MAX_PACKET_SIZE
#define APP_TX_DATA_SIZE    USB_FS_MAX_PACKET_SIZE
#define USBTMC_MSP_SIZE     USB_FS_MAX_PACKET_SIZE

#define RINGBUF_SIZE        1024


//#define USBTMC_SUPPORT_TERMCHAR
//#define USB488_SUPPORT_TRIGGER

// USBTMC 消息处理状态
typedef enum
{
    USBTMC_STATE_IDLE = 0,
    USBTMC_STATE_RECEIVE_BULKOUT_DEV_DEP_MSG_OUT,               // 接收命令消息
    USBTMC_STATE_WAIT_UPPER_GET_ALL_BULKOUT_DEV_DEP_MSG_OUT,    // 接收完成，等待上层应用读命令消息
    USBTMC_STATE_WAIT_UPPER_PARSE_ALL_BULKOUT_DEV_DEP_MSG_OUT,  // 等待上层应用完成消息分析和处理
    USBTMC_STATE_REQUEST_BULKIN_DEV_DEP_MSG_IN,                 // 接收到 REQUEST_BULKIN_DEV_DEP_MSG_IN，还没有开始发送消息
    USBTMC_STATE_SEND_BULKIN_DEV_DEP_MSG_IN_FIRST_PACKET,       // 正在响应 DEV_DEP_MSG_IN 请求，向主机发送执行结果，发送的是第一个数据包
    USBTMC_STATE_SEND_BULKIN_DEV_DEP_MSG_IN_NOT_LAST_PACKET,    // 正在响应 DEV_DEP_MSG_IN 请求，向主机发送执行结果，非最后一个数据包
    USBTMC_STATE_SEND_BULKIN_DEV_DEP_MSG_IN_LAST_PACKET,        // 正在响应 DEV_DEP_MSG_IN 请求，向主机发送执行结果，发送的是最后一个数据包
    USBTMC_STATE_SEND_BULKIN_DEV_DEP_MSG_IN_ZLP_PACKET,         // 正在响应DEV_DEP_MSG_IN请求，向主机发送 ZLP 数据包
}usbtmc_state_type_t;

// USBTMC 传输状态机
typedef struct
{
    usbtmc_state_type_t bulkout_state;
    usbtmc_state_type_t bulkin_state;

    uint8_t MsgID;
    uint8_t bTag;
    uint8_t bTagInverse;

    // 当前正在处理的消息的信息
    uint8_t  DEV_DEP_MSG_OUT_bTag;
    uint32_t DEV_DEP_MSG_OUT_TransferSize;
    uint8_t  DEV_DEP_MSG_OUT_bmTransferAttributes;
    uint32_t DEV_DEP_MSG_OUT_CountTransfered;

    uint8_t  REQUEST_DEV_DEP_MSG_IN_bTag;
    uint32_t REQUEST_DEV_DEP_MSG_IN_TransferSize;
    uint8_t  REQUEST_DEV_DEP_MSG_IN_bmTransferAttributes;
    uint8_t  REQUEST_DEV_DEP_MSG_IN_TermChar;

    uint8_t  DEV_DEP_MSG_IN_bTag;
    uint32_t DEV_DEP_MSG_IN_TransferSize;
    uint8_t  DEV_DEP_MSG_IN_bmTransferAttributes;
    uint32_t DEV_DEP_MSG_IN_CountTransfered;

    uint32_t data_remaining_size;
    uint32_t data_transfer_idx;
}usbtmc_context_type_t;

/* USER CODE END EXPORTED_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_TMC_IF_Exported_Types USBD_TMC_IF_Exported_Types
  * @brief Types.
  * @{
  */

/* USER CODE BEGIN EXPORTED_TYPES */

typedef struct _respond_item_ {
    int len;            // positive for ascii; negative for binary
    char *data;
    void (*free)(void *);
}respond_item;

/* USER CODE END EXPORTED_TYPES */

/**
  * @}
  */

/** @defgroup USBD_TMC_IF_Exported_Macros USBD_TMC_IF_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* USER CODE BEGIN EXPORTED_MACRO */

/* USER CODE END EXPORTED_MACRO */

/**
  * @}
  */

/** @defgroup USBD_TMC_IF_Exported_Variables USBD_TMC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

/** TMC Interface callback. */
extern USBD_TMC_IfTypeDef USBD_Interface_fops_FS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_TMC_IF_Exported_FunctionsPrototype USBD_TMC_IF_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
  */

uint8_t TMC_Transmit_FS(uint8_t* Buf, uint16_t Len);


/* USER CODE BEGIN EXPORTED_FUNCTIONS */
int usbtmc_read(void *data, int number);
int usbtmc_prepare_respond(respond_item item);
void usbtmc_respond_task(void);
/* USER CODE END EXPORTED_FUNCTIONS */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_TMC_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
