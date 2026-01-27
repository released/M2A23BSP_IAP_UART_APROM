#ifndef __ISP_CONFIG_H__
#define __ISP_CONFIG_H__

/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "NuMicro.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

#define RST_ADDR_LDROM                      (0)
#define RST_ADDR_APROM                      (1)
#define RST_SEL_NVIC                        (0)
#define RST_SEL_CPU                         (1)
#define RST_SEL_CHIP                        (2)

/*_____ D E F I N I T I O N S ______________________________________________*/

/*  
	template
	typedef struct _peripheral_manager_t
	{
		uint16_t* pu16Far;
		uint8_t u8Cmd;
		uint8_t au8Buf[33];
		uint8_t u8RecCnt;
		uint8_t bByPass;
	}PERIPHERAL_MANAGER_T;

	volatile PERIPHERAL_MANAGER_T g_PeripheralManager = 
	{
		.pu16Far = NULL,	//.pu16Far = 0	
		.u8Cmd = 0,
		.au8Buf = {0},		//.au8Buf = {100U, 200U},
		.u8RecCnt = 0,
		.bByPass = FALSE,
	};
	extern volatile PERIPHERAL_MANAGER_T g_PeripheralManager;
*/


#define DEBUG_UART_PORT							(UART1)
#define DEBUG_UART_PORT_IRQn					(UART1_IRQn)
#define DEBUG_UART_IRQHandler					(UART1_IRQHandler)

#define TIMEOUT_INTERVAL    	                (5)   // sec

#define APROM_APPLICATION_START     			(0x00003000UL)
#define APROM_APPLICATION_SIZE      			(0x0001D000UL)

/*_____ M A C R O S ________________________________________________________*/

#define LDROM_DEBUG(format, args...) 		printf("\033[1;36m" "[LDROM]" format "\033[0m", ##args)

/*_____ F U N C T I O N S __________________________________________________*/

void ISP_post_write_hook(uint32_t len);

void ISP_perform_reset(void);

void ISP_timer_isr(void);
void ISP_process(void);
void ISP_check_app(void);
void ISP_Init(void);

#endif //__ISP_CONFIG_H__
