/***************************************************************************//**
 * @file     targetdev.h
 * @brief    ISP support function header file
 * @version  0x32
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "NuMicro.h"
#include "isp_user.h"


/* rename for uart_transfer.c */
#define ISP_UART_PORT					UART0
#define ISP_UART_PORT_IRQn			    UART0_IRQn
#define ISP_UART_PORT_IRQHandler		UART0_IRQHandler

#define CONFIG_SIZE 8 // in bytes
