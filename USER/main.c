#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "queue.h"

#include "usb_device.h"
#include "usbd_tmc_if.h"

#define SN_STRING           "F00DCAFE"

#define STRING_IDN          "mfac,model," SN_STRING ",v1.00\n"
#define STRING_READ51       "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.00000\n"
#define STRING_READ52       "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000\n"
#define STRING_READ53       "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E\n"
#define STRING_READ56       "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00\n"
#define STRING_READ113      "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;\n"
#define STRING_READ116      "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0E\n"
#define STRING_READ601      "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0" \
                            "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0" \
                            "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0" \
                            "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0" \
                            "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0" \
                            "+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0.000000E+00;+0" \
                            "\n"

static uint8_t data_usbtmc[512];
static int count_usbtmc;

static respond_item item = {
    0,
    0,
    NULL,
};

int main(void)
{
    int retry = 0;
// 1. 初始化
    HAL_Init();                         //初始化 HAL 库
    SystemClock_Config();               //设置时钟, 168Mhz

    uart2_init(115200);

    MX_USB_DEVICE_Init();
    USBTMC_Change_SN(SN_STRING);

    while (1) {
        usbtmc_respond_task();
        count_usbtmc = usbtmc_read(&data_usbtmc, sizeof(data_usbtmc) - 1);
        if (count_usbtmc > 0) {
            data_usbtmc[count_usbtmc] = '\0';
            printf("usbtmc:read %d bytes-> %s\n", count_usbtmc, data_usbtmc);
            if(strncmp((char *)data_usbtmc, "*IDN?", 5) == 0)
            {
                item.data = STRING_IDN;
                item.len = 26;
                usbtmc_prepare_respond(item);
            }
            if (strncmp((char *)data_usbtmc, ":READ51", 7) == 0)     // alignment one byte +12 = MPS, full packet
            {
                item.data = STRING_READ51;
                item.len = 51;
                usbtmc_prepare_respond(item);
            }
            if (strncmp((char *)data_usbtmc, ":READ52", 7) == 0)     // +12 = MPS, full packet
            {
                item.data = STRING_READ52;
                item.len = 52;
                usbtmc_prepare_respond(item);
            }
            if (strncmp((char *)data_usbtmc, ":READ53", 7) == 0)     // alignment 3 bytes +12 > MPS
            {
                item.data = STRING_READ53;
                item.len = 53;
                usbtmc_prepare_respond(item);
            }
            if (strncmp((char *)data_usbtmc, ":READ56", 7) == 0) {     // +12 > MPS
                item.data = STRING_READ56;
                item.len = 56;
                usbtmc_prepare_respond(item);
            }
            if (strncmp((char *)data_usbtmc, ":READ113", 8) == 0) {     // alignment 3 bytes +12 = 2*MPS, full packet
                item.data = STRING_READ113;
                item.len = 113;
                usbtmc_prepare_respond(item);
            }
            if (strncmp((char *)data_usbtmc, ":READ116", 8) == 0) {     // +12 = 2*MPS, full packet
                item.data = STRING_READ116;
                item.len = 116;
                usbtmc_prepare_respond(item);
            }
            if (strncmp((char *)data_usbtmc, ":READ601", 8) == 0) {     // +12 > n*MPS
                item.data = STRING_READ601;
                item.len = 601;
                usbtmc_prepare_respond(item);
            }
            if (strncmp((char *)data_usbtmc, ":IMG", 4) == 0) {     // +12 > n*MPS
                item.data = (void*)0x20000000;
                item.len = -2048;
                usbtmc_prepare_respond(item);
            }
            memset(data_usbtmc, 0, 64);
        }

//        printf("F");
//        delay_ms(1000);
//        if (PBin(0) == 1) {
//            PBout(0) = 0;
//        } else {
//            PBout(0) = 1;
//        }
    }

    return 0;
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
