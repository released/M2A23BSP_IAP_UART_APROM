/**************************************************************************//**
 * @file     uart_transfer.c
 * @version  V1.00
 * @brief    General UART ISP slave Sample file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include <string.h>
#include "targetdev.h"
#include "uart_transfer.h"

__attribute__((aligned(4))) uint8_t  uart_rcvbuf[MAX_PKT_SIZE] = {0};

uint8_t volatile bUartDataReady = 0;
uint8_t volatile bufhead = 0;


/* please check "targetdev.h" for chip specifc define option */

/*---------------------------------------------------------------------------------------------------------*/
/* INTSTS to handle UART Channel 0 interrupt event                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void ISP_UART_PORT_IRQHandler(void)
{
    /* Determine interrupt source */
    uint32_t u32IntSrc = ISP_UART_PORT->INTSTS;

    /* RDA FIFO interrupt and RDA timeout interrupt */
    if(u32IntSrc & (UART_INTSTS_RXTOIF_Msk | UART_INTSTS_RDAIF_Msk))
    {

        /* Read data until RX FIFO is empty or data is over maximum packet size */
        while(((ISP_UART_PORT->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0) && (bufhead < MAX_PKT_SIZE))
        {
            uart_rcvbuf[bufhead++] = ISP_UART_PORT->DAT;
        }
    }

    /* Reset data buffer index */
    if(bufhead == MAX_PKT_SIZE)
    {
        bUartDataReady = TRUE;
        bufhead = 0;
    }
    else if(u32IntSrc & UART_INTSTS_RXTOIF_Msk)
    {
        bufhead = 0;
    }
}

extern __attribute__((aligned(4))) uint8_t response_buff[64];
void PutString(void)
{
    uint32_t i;

    /* UART send response to master */
    for(i = 0; i < MAX_PKT_SIZE; i++)
    {

        /* Wait for TX not full */
        while((ISP_UART_PORT->FIFOSTS & UART_FIFOSTS_TXFULL_Msk));

        /* UART send data */
        ISP_UART_PORT->DAT = response_buff[i];
    }
}

void UART_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Select UART function mode */
    ISP_UART_PORT->FUNCSEL = UART_FUNCSEL_UART;
    /* Set UART line configuration */
    ISP_UART_PORT->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
    /* Set UART Rx and RTS trigger level */
    ISP_UART_PORT->FIFO = UART_FIFO_RFITL_6BYTES | UART_FIFO_RTSTRGLV_6BYTES;
    /* Set UART baud rate */
    ISP_UART_PORT->BAUD = (UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER(__HIRC, 115200));
    /* Set time-out interrupt comparaTOUT */
    ISP_UART_PORT->TOUT = (ISP_UART_PORT->TOUT & ~UART_TOUT_TOIC_Msk) | (0x40);
    NVIC_SetPriority(ISP_UART_PORT_IRQn, 2);
    NVIC_EnableIRQ(ISP_UART_PORT_IRQn);
    /* Enable tim-out counter, Rx tim-out interrupt and Rx ready interrupt */
    ISP_UART_PORT->INTEN = (UART_INTEN_TOCNTEN_Msk | UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk);
}
