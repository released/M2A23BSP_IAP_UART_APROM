/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "misc_config.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_TIMER_PERIOD_500MS                   	(flag_PROJ_CTL.bit1)
#define FLAG_PROJ_ERASE_CHECKSUM                 	    (flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                              (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned int counter_systick = 0;
volatile uint32_t counter_tick = 0;

#define APROM_1
// #define APROM_2

// #define APROM_APPLICATION_START     		            (0x00000000UL)

#define DEBUG_UART_PORT							        (UART1)
#define DEBUG_UART_PORT_IRQn					        (UART1_IRQn)
#define DEBUG_UART_IRQHandler					        (UART1_IRQHandler)

//use excel to calculate , boot code : 12K (LDROM : 4K + APROM : 8K) , application : 116K  (size : 0x1D000 , end : 0x1FFFF) 
#define CHECKSUM_ADDRESS                                (0x1FFFC)

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned int get_systick(void)
{
	return (counter_systick);
}

void set_systick(unsigned int t)
{
	counter_systick = t;
}

void systick_counter(void)
{
	counter_systick++;
}

void SysTick_Handler(void)
{

    systick_counter();

    // if ((get_systick() % 1000) == 0)
    // {
       
    // }

    #if defined (ENABLE_TICK_EVENT)
    TickCheckTickEvent();
    #endif    
}

void SysTick_delay(unsigned int delay)
{  
    
    unsigned int tickstart = get_systick(); 
    unsigned int wait = delay; 

    while((get_systick() - tickstart) < wait) 
    { 
    } 

}

void SysTick_enable(unsigned int ticks_per_second)
{
    set_systick(0);
    if (SysTick_Config(SystemCoreClock / ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        printf("Set system tick error!!\n");
        while (1);
    }

    #if defined (ENABLE_TICK_EVENT)
    TickInitTickEvent();
    #endif
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
}

void delay_ms(uint16_t ms)
{	
	#if 1
	uint32_t start = get_tick();
    while ((uint32_t)(get_tick() - start) < (uint32_t)ms) 
	{
		
	}
	#else
	TIMER_Delay(TIMER0, 1000*ms);
	#endif
}


void FMC_ISP(uint32_t u32Cmd, uint32_t u32Addr, uint32_t u32Data)
{
    uint32_t u32TimeOutCnt;

    FMC_ENABLE_AP_UPDATE();    
    
    FMC->ISPCMD = u32Cmd;
    FMC->ISPADDR = u32Addr;
    FMC->ISPDAT = u32Data;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
    __ISB();                                    /* To make sure ISP/CPU be Synchronized */
    
    u32TimeOutCnt = FMC_TIMEOUT_READ;
    while(FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)    /* Waiting for ISP Done */
    {
        if(--u32TimeOutCnt == 0)
        {
            // g_FMC_i32ErrCode = -1;
            while(1);
        }
    }
}

void erase_checksum(uint32_t u32Addr, uint32_t u32Data)
{
    uint32_t res = 0;

    printf("[app]%s:0x%8X,0x%8X\r\n",__FUNCTION__,u32Addr,u32Data);
    while(!UART_IS_TX_EMPTY(DEBUG_UART_PORT));

    SYS_UnlockReg();
    // CLK->AHBCLK |= CLK_AHBCLK_ISP_EN_Msk;
    // FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk | FMC_ISPCTL_APUEN_Msk;
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();

    res = FMC_Read(u32Addr);
    printf("[app]%s:0x%8X, result:0x%8X(before)\r\n" ,__FUNCTION__, u32Addr , res);
   
    if ((u32Addr & (FMC_FLASH_PAGE_SIZE - 1)) == 0)
        FMC_ISP(FMC_ISPCMD_PAGE_ERASE, u32Addr, 0);


    FMC_ISP(FMC_ISPCMD_PROGRAM, u32Addr, u32Data);

    res = FMC_Read(u32Addr);
    printf("[app]%s:0x%8X, result:0x%8X(after)\r\n" ,__FUNCTION__, u32Addr , res);

}


//
// check_reset_source
//
uint8_t check_reset_source(void)
{
    uint32_t src = SYS_GetResetSrc();

    SYS->RSTSTS |= 0x1FF;
    printf("Reset Source <0x%08X>\r\n", src);

    #if 1   //DEBUG , list reset source
    if (src & BIT0)
    {
        printf("0)POR Reset Flag\r\n");       
    }
    if (src & BIT1)
    {
        printf("1)NRESET Pin Reset Flag\r\n");       
    }
    if (src & BIT2)
    {
        printf("2)WDT Reset Flag\r\n");       
    }
    if (src & BIT3)
    {
        printf("3)LVR Reset Flag\r\n");       
    }
    if (src & BIT4)
    {
        printf("4)BOD Reset Flag\r\n");       
    }
    if (src & BIT5)
    {
        printf("5)System Reset Flag \r\n");       
    }
    if (src & BIT6)
    {
        printf("6)Reserved.\r\n");       
    }
    if (src & BIT7)
    {
        printf("7)CPU Reset Flag\r\n");       
    }
    if (src & BIT8)
    {
        printf("8)CPU Lockup Reset Flag\r\n");       
    }
    #endif
    
    if (src & SYS_RSTSTS_PORF_Msk) {
        SYS_ClearResetSrc(SYS_RSTSTS_PORF_Msk);
        
        printf("power on from POR\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_PINRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_PINRF_Msk);
        
        printf("power on from nRESET pin\r\n");
        return FALSE;
    } 
    else if (src & SYS_RSTSTS_WDTRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_WDTRF_Msk);
        
        printf("power on from WDT Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_LVRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_LVRF_Msk);
        
        printf("power on from LVR Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_BODRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_BODRF_Msk);
        
        printf("power on from BOD Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_SYSRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_SYSRF_Msk);
        
        printf("power on from System Reset\r\n");
        return FALSE;
    } 
    else if (src & SYS_RSTSTS_CPURF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_CPURF_Msk);

        printf("power on from CPU reset\r\n");
        return FALSE;         
    }    
    else if (src & SYS_RSTSTS_CPULKRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_CPULKRF_Msk);
        
        printf("power on from CPU Lockup Reset\r\n");
        return FALSE;
    }   
    
    printf("power on from unhandle reset source\r\n");
    return FALSE;
}

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
		tick_counter();

		if ((get_tick() % 1000) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_1000MS = 1;//set_flag(flag_timer_period_1000ms ,ENABLE);
		}
		if ((get_tick() % 500) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_500MS = 1;
		}

		if ((get_tick() % 50) == 0)
		{

		}	
    }
}

void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}

void loop(void)
{
	static uint32_t LOG1 = 0;
	// static uint32_t LOG2 = 0;

    if ((get_systick() % 1000) == 0)
    {
        // printf("%s(systick) : %4d\r\n",__FUNCTION__,LOG2++);    
    }

    #if defined (APROM_1)
    if (FLAG_PROJ_TIMER_PERIOD_1000MS)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;
        printf("[app1]%s : %4d\r\n",__FUNCTION__,LOG1++);
        PF14 ^= 1; 
    }
    #endif

    #if defined (APROM_2)
    if (FLAG_PROJ_TIMER_PERIOD_500MS)
    {
        FLAG_PROJ_TIMER_PERIOD_500MS = 0;
        printf("[app2]%s : %4d\r\n",__FUNCTION__,LOG1++);
        PF14 ^= 1; 
    }
    #endif


    if (FLAG_PROJ_ERASE_CHECKSUM)
    {
        FLAG_PROJ_ERASE_CHECKSUM  = 0;

        // clear check sum to force update APROM when return to boot code
        erase_checksum(CHECKSUM_ADDRESS , 0x00000000);

        printf("[app]Perform RST to enter BOOTLOADER\r\n\r\n");
        while(!UART_IS_TX_EMPTY(DEBUG_UART_PORT));

        /* Unlock protected registers */
        SYS_UnlockReg();
        /* Enable FMC ISP function */
        FMC_Open();

        __set_PRIMASK(1);        
        #if 1
        FMC_SetVectorPageAddr(FMC_APROM_BASE);
        FMC_SET_APROM_BOOT();
        #else
        FMC_SetVectorPageAddr(FMC_LDROM_BASE);
        FMC_SET_LDROM_BOOT(); 
        #endif

        // Not reset I/O and peripherals
        // SYS_ResetCPU();
        // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)
        SYS_ResetChip();

        // FMC_DISABLE_AP_UPDATE();           /* Disable APROM update. */
        FMC_Close();                       /* Disable FMC ISP function */
        SYS_LockReg();                     /* Lock protected registers */  
    }    

}

void UARTx_Process(void)
{
	uint8_t res = 0;
	res = UART_READ(DEBUG_UART_PORT);

	if (res > 0x7F)
	{
		printf("invalid command\r\n");
	}
	else
	{
		printf("press : %c\r\n" , res);
		switch(res)
		{
			case '1':                
                FLAG_PROJ_ERASE_CHECKSUM = 1;
				break;

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
                SYS_UnlockReg();
				// NVIC_SystemReset();	// Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
                // SYS_ResetCPU();     // Not reset I/O and peripherals
                SYS_ResetChip();    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)	
				break;
		}
	}
}

void DEBUG_UART_IRQHandler(void)
{

    if(UART_GET_INT_FLAG(DEBUG_UART_PORT, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(DEBUG_UART_PORT) == 0)
        {
            UARTx_Process();
        }
    }

    if(DEBUG_UART_PORT->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(DEBUG_UART_PORT, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    }	
}

void DEBUG_UART_Init(void)
{
    SYS_ResetModule(UART1_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(DEBUG_UART_PORT, 115200);
    UART_EnableInt(DEBUG_UART_PORT, UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk);
    NVIC_EnableIRQ(DEBUG_UART_PORT_IRQn);
	
	#if (_debug_log_UART_ == 1)	//debug
	printf("\r\n[app]CLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("[app]CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
	printf("[app]CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("[app]CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("[app]CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("[app]CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
	#endif	

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

    // CLK_EnableModuleClock(UART0_MODULE);
    // CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    // SET_UART0_RXD_PB12();
    // SET_UART0_TXD_PB13();

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

	GPIO_Init();
	DEBUG_UART_Init();
	TIMER1_Init();
    check_reset_source();

    SysTick_enable(1000);
    #if defined (ENABLE_TICK_EVENT)
    TickSetTickEvent(1000, TickCallback_processA);  // 1000 ms
    TickSetTickEvent(5000, TickCallback_processB);  // 5000 ms
    #endif


    /* Got no where to go, just loop forever */
    while(1)
    {
        loop();

    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
