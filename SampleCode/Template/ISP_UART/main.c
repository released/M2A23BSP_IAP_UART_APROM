/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

// #include "misc_config.h"

#include "isp_config.h"
#include "isp_user.h"
#include "targetdev.h"
#include "uart_transfer.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/


/*_____ D E F I N I T I O N S ______________________________________________*/

// volatile unsigned int counter_systick = 0;
// volatile unsigned int counter_tick = 0;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

// unsigned int get_systick(void)
// {
// 	return (counter_systick);
// }

// void set_systick(unsigned int t)
// {
// 	counter_systick = t;
// }

// void systick_counter(void)
// {
// 	counter_systick++;
// }

// void SysTick_Handler(void)
// {
//     systick_counter();

//     // if ((get_systick() % 1000) == 0)
//     // {
       
//     // }

//     #if defined (ENABLE_TICK_EVENT)
//     TickCheckTickEvent();
//     #endif    
// }

// void SysTick_delay(unsigned int delay)
// {  
    
//     unsigned int tickstart = get_systick(); 
//     unsigned int wait = delay; 

//     while((get_systick() - tickstart) < wait) 
//     { 
//     } 

// }

// void SysTick_enable(unsigned int ticks_per_second)
// {
//     set_systick(0);
//     if (SysTick_Config(SystemCoreClock / ticks_per_second))
//     {
//         /* Setup SysTick Timer for 1 second interrupts  */
//         printf("Set system tick error!!\n");
//         while (1);
//     }

//     #if defined (ENABLE_TICK_EVENT)
//     TickInitTickEvent();
//     #endif
// }

// uint32_t get_tick(void)
// {
// 	return (counter_tick);
// }

// void set_tick(uint32_t t)
// {
// 	counter_tick = t;
// }

// void tick_counter(void)
// {
// 	counter_tick++;
// }

// void delay_ms(uint16_t ms)
// {	
// 	#if 1
// 	uint32_t start = get_tick();
//     while ((uint32_t)(get_tick() - start) < (uint32_t)ms) 
// 	{
		
// 	}
// 	#else
// 	TIMER_Delay(TIMER0, 1000*ms);
// 	#endif
// }

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
		ISP_timer_isr();
    }
}

void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    // TIMER_Start(TIMER1);
}

// void loop(void)
// {

// }

// void UARTx_Process(void)
// {
// 	uint8_t res = 0;
// 	res = UART_READ(DEBUG_UART_PORT);

// 	if (res > 0x7F)
// 	{
// 		LDROM_DEBUG("invalid command\r\n");
// 	}
// 	else
// 	{
// 		LDROM_DEBUG("press : %c\r\n" , res);
// 		switch(res)
// 		{
// 			case '1':
// 				break;

// 			case 'X':
// 			case 'x':
// 			case 'Z':
// 			case 'z':
//                 SYS_UnlockReg();
// 				// NVIC_SystemReset();	// Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
//                 // SYS_ResetCPU();     // Not reset I/O and peripherals
//                 SYS_ResetChip();    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)	
// 				break;
// 		}
// 	}
// }

// void DEBUG_UART_IRQHandler(void)
// {

//     if(UART_GET_INT_FLAG(DEBUG_UART_PORT, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
//     {
//         while(UART_GET_RX_EMPTY(DEBUG_UART_PORT) == 0)
//         {
//             UARTx_Process();
//         }
//     }

//     if(DEBUG_UART_PORT->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
//     {
//         UART_ClearIntFlag(DEBUG_UART_PORT, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
//     }	
// }

void DEBUG_UART_Init(void)
{
    SYS_ResetModule(UART1_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(DEBUG_UART_PORT, 115200);
    // UART_EnableInt(DEBUG_UART_PORT, UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk);
    // NVIC_EnableIRQ(DEBUG_UART_PORT_IRQn);
	
	// #if (_debug_log_UART_ == 1)	//debug
	// LDROM_DEBUG("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	// LDROM_DEBUG("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
	// LDROM_DEBUG("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	// LDROM_DEBUG("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	// LDROM_DEBUG("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	// LDROM_DEBUG("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
	// #endif	
}

void GPIO_Init (void)
{
    SYS->GPF_MFPH = (SYS->GPF_MFPH & ~(SYS_GPF_MFPH_PF14MFP_Msk)) | (SYS_GPF_MFPH_PF14MFP_GPIO);
		
	// EVM LED_R
    GPIO_SetMode(PF, BIT14, GPIO_MODE_OUTPUT);		
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

//	CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);	

//	CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);	

    /* Set core clock to 72MHz */
    CLK_SetCoreClock(72000000);

    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK->AHBCLK |= CLK_AHBCLK_GPIOACKEN_Msk | CLK_AHBCLK_GPIOBCKEN_Msk |
                   CLK_AHBCLK_GPIOCCKEN_Msk | CLK_AHBCLK_GPIODCKEN_Msk |
                   CLK_AHBCLK_GPIOFCKEN_Msk;


    // ISP UART
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

    // DEBUG PORT
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL2_UART1SEL_HIRC, CLK_CLKDIV0_UART1(1));
    SET_UART1_RXD_PA8();
    SET_UART1_TXD_PA9();

    // CLK_EnableModuleClock(TMR0_MODULE);
  	// CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);

    CLK_EnableModuleClock(TMR1_MODULE);
  	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);


   /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

int main()
{
    SYS_Init();

	DEBUG_UART_Init();    
	UART_Init();
	TIMER1_Init();

    ISP_Init();
    ISP_check_app();

    SYS_UnlockReg();
    FMC_Open();  
    /* Got no where to go, just loop forever */
    while(1)
    {
        // loop();
        ISP_process();

    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
