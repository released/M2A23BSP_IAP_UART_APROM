/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "NuMicro.h"

#include "isp_config.h"
#include "isp_user.h"
#include "targetdev.h"
#include "uart_transfer.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

__IO uint32_t timeout_cnt = 0;
unsigned long state = 0xFFFFFFFF;
static uint32_t g_written_bytes;
static uint8_t  g_last_percent;

#define ISP_BAR_WIDTH   10

/*_____ M A C R O S ________________________________________________________*/

#define ENABLE_SW_CRC32

#if defined (ENABLE_SW_CRC32)
#define USE_SRAM_TABLE
// #define USE_FLASH_TABLE

/*_____ F U N C T I O N S __________________________________________________*/

#if defined (USE_FLASH_TABLE)
const uint32_t table[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
#elif defined (USE_SRAM_TABLE)
unsigned long table[256];
#define POLYNOMIAL 0xedb88320
void calculate_table(void)
{
    unsigned b = 0;
    unsigned long v = 0;
    int i = 0;

    for (b = 0; b < 256; ++b)
    {
        v = b;
        i = 8;
        for (; --i >= 0; )
            v = (v & 1) ? ((v >> 1) ^ POLYNOMIAL) : (v >> 1);
        table[b] = v;
    }
}
#endif

unsigned long UPDC32(unsigned char octet, unsigned long crc)
{
    // The original code had this as a #define
    return table[(crc ^ octet) & 0xFF] ^ (crc >> 8);
}

#endif


uint32_t caculate_crc32_checksum(uint32_t start, uint32_t size)
{
    volatile uint32_t addr, data;    

    #if defined (USE_FLASH_TABLE)
    // LDROM_DEBUG("ENABLE_SW_CRC32:[FLASH_TABLE]\r\n");    
    #elif defined (USE_SRAM_TABLE)
    // LDROM_DEBUG("ENABLE_SW_CRC32:[SRAM_TABLE]\r\n");       
    calculate_table();
    #endif

    state = 0xFFFFFFFF;    
    addr = start;

    for(addr = start; addr < (start+size) ; addr += 4){
        data = FMC_Read(addr);
        state = UPDC32(_GET_BYTE0(data), state);
        state = UPDC32(_GET_BYTE1(data), state);
        state = UPDC32(_GET_BYTE2(data), state);
        state = UPDC32(_GET_BYTE3(data), state); 
    }

    return ~state;  

}

uint8_t verify_application_chksum(void)
{
    uint32_t chksum_cal, chksum_app;
    
    // LDROM_DEBUG("Verify Checksum\r\n");
    
    // chksum_cal = caculate_crc32_checksum(0x00000000, (g_apromSize - 4));//(g_apromSize - FMC_FLASH_PAGE_SIZE)
    chksum_cal = caculate_crc32_checksum(APROM_APPLICATION_START, (g_apromSize - 4));//(g_apromSize - FMC_FLASH_PAGE_SIZE)
    LDROM_DEBUG("Caculated:0x%08X\r\n", chksum_cal);
    
    // chksum_app = FMC_Read(g_apromSize - 4);     
    chksum_app = FMC_Read(APROM_APPLICATION_START + g_apromSize - 4);    
    LDROM_DEBUG("In APROM:0x%08X\r\n", chksum_app);
    
    if (chksum_cal == chksum_app) {
        LDROM_DEBUG("==>PASS\r\n");
        return TRUE;
    } else {
        LDROM_DEBUG("==>FAIL\r\n");
        return FALSE;
    }
}


//
// check_reset_source
//
uint8_t check_reset_source(void)
{
    uint32_t src = SYS_GetResetSrc();

    SYS->RSTSTS |= 0x1FF;
    LDROM_DEBUG("Reset Source <0x%08X>\r\n", src);

    #if 0   //DEBUG , list reset source
    if (src & BIT0)
    {
        LDROM_DEBUG("0)POR Reset Flag\r\n");       
    }
    if (src & BIT1)
    {
        LDROM_DEBUG("1)NRESET Pin Reset Flag\r\n");       
    }
    if (src & BIT2)
    {
        LDROM_DEBUG("2)WDT Reset Flag\r\n");       
    }
    if (src & BIT3)
    {
        LDROM_DEBUG("3)LVR Reset Flag\r\n");       
    }
    if (src & BIT4)
    {
        LDROM_DEBUG("4)BOD Reset Flag\r\n");       
    }
    if (src & BIT5)
    {
        LDROM_DEBUG("5)System Reset Flag \r\n");       
    }
    if (src & BIT6)
    {
        LDROM_DEBUG("6)Reserved.\r\n");       
    }
    if (src & BIT7)
    {
        LDROM_DEBUG("7)CPU Reset Flag\r\n");       
    }
    if (src & BIT8)
    {
        LDROM_DEBUG("8)CPU Lockup Reset Flag\r\n");       
    }
    #endif
    
    if (src & SYS_RSTSTS_PORF_Msk) {
        SYS_ClearResetSrc(SYS_RSTSTS_PORF_Msk);
        
        // LDROM_DEBUG("power on from POR\r\n");
        LDROM_DEBUG("POR Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_PINRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_PINRF_Msk);
        
        // LDROM_DEBUG("power on from nRESET pin\r\n");
        LDROM_DEBUG("nRESET pin Reset\r\n");
        return FALSE;
    } 
    else if (src & SYS_RSTSTS_WDTRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_WDTRF_Msk);
        
        // LDROM_DEBUG("power on from WDT Reset\r\n");
        LDROM_DEBUG("WDT Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_LVRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_LVRF_Msk);
        
        // LDROM_DEBUG("power on from LVR Reset\r\n");
        LDROM_DEBUG("LVR Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_BODRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_BODRF_Msk);
        
        // LDROM_DEBUG("power on from BOD Reset\r\n");
        LDROM_DEBUG("BOD Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_SYSRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_SYSRF_Msk);
        
        // LDROM_DEBUG("power on from System Reset\r\n");
        LDROM_DEBUG("System Reset\r\n");
        return FALSE;
    } 
    else if (src & SYS_RSTSTS_CPURF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_CPURF_Msk);

        // LDROM_DEBUG("power on from CPU reset\r\n");
        LDROM_DEBUG("CPU reset\r\n");
        return FALSE;         
    }    
    else if (src & SYS_RSTSTS_CPULKRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_CPULKRF_Msk);
        
        // LDROM_DEBUG("power on from CPU Lockup Reset\r\n");
        LDROM_DEBUG("CPU Lockup Reset\r\n");
        return FALSE;
    }   
    
    // LDROM_DEBUG("power on from unhandle reset source\r\n");
    LDROM_DEBUG("unhandle reset source\r\n");
    return FALSE;
}


void SystemReboot_RST(unsigned char addr , unsigned char sel)
{
    uint32_t u32TimeOutCnt;

    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while(!UART_IS_TX_EMPTY(UART1))
        if(--u32TimeOutCnt == 0) break;
        
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();

    switch(addr) // CONFIG: w/ IAP
    {
        case RST_ADDR_LDROM:
            /* Mask all interrupt before changing VECMAP to avoid wrong interrupt handler fetched */
            __set_PRIMASK(1);    
            FMC_SetVectorPageAddr(FMC_LDROM_BASE);
            FMC_SET_LDROM_BOOT();        
            break;
        case RST_ADDR_APROM:
            /* Mask all interrupt before changing VECMAP to avoid wrong interrupt handler fetched */
            __set_PRIMASK(1);    
            FMC_SetVectorPageAddr(FMC_APROM_BASE);
            FMC_SET_APROM_BOOT();        
            break;            
    }

    switch(sel)
    {
        case RST_SEL_NVIC:  // Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
            NVIC_SystemReset();
            break;
        case RST_SEL_CPU:   // Not reset I/O and peripherals
            SYS_ResetCPU();
            break;   
        case RST_SEL_CHIP:
            SYS_ResetChip();// Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)
            break;                       
    } 
}

void IAPSystemReboot_RST(unsigned char addr , unsigned char sel)
{

    uint32_t u32TimeOutCnt;
    
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while(!UART_IS_TX_EMPTY(UART1))
        if(--u32TimeOutCnt == 0) break;
        
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();

    switch(addr) // CONFIG: w/ IAP
    {
        case RST_ADDR_LDROM:
            /* Mask all interrupt before changing VECMAP to avoid wrong interrupt handler fetched */
            __set_PRIMASK(1);    
            FMC_SetVectorPageAddr(FMC_APROM_BASE);
            FMC_SET_APROM_BOOT();        
            break;
        case RST_ADDR_APROM:
            /* Mask all interrupt before changing VECMAP to avoid wrong interrupt handler fetched */
            __set_PRIMASK(1);    
            FMC_SetVectorPageAddr(APROM_APPLICATION_START);
            FMC_SET_APROM_BOOT();        
            break;            
    }

    switch(sel)
    {
        case RST_SEL_NVIC:  // Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
            NVIC_SystemReset();
            break;
        case RST_SEL_CPU:   // Not reset I/O and peripherals
            SYS_ResetCPU();
            break;   
        case RST_SEL_CHIP:
            SYS_ResetChip();// Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)
            break;                       
    } 
}

static void isp_log_percent(uint8_t percent)
{
    char bar[ISP_BAR_WIDTH + 1];
    uint8_t i;
    uint8_t filled;

    if (percent > 100)
        percent = 100;

    filled = (percent * ISP_BAR_WIDTH) / 100;

    for (i = 0; i < ISP_BAR_WIDTH; i++)
    {
        bar[i] = (i < filled) ? '#' : '-';
    }
    bar[ISP_BAR_WIDTH] = '\0';

    /* '\r' redraw same line, '\n' only when 100% */
    if (percent < 100)
    {
        LDROM_DEBUG("[%s] %3u%%\r", bar, percent);
    }
    else
    {
        LDROM_DEBUG("[%s] %3u%%\r\n", bar, percent);
    }
}

static void isp_update_progress(void)
{
    uint8_t percent;

    if (g_written_bytes > APROM_APPLICATION_SIZE)
        g_written_bytes = APROM_APPLICATION_SIZE;

    percent = (uint8_t)((g_written_bytes * 100UL) / APROM_APPLICATION_SIZE);

    if (percent != g_last_percent)
    {
        g_last_percent = percent;
        isp_log_percent(percent);
    }
}

void ISP_post_write_hook(uint32_t len)
{
    g_written_bytes += len;
    isp_update_progress();
}

void ISP_perform_reset(void)
{    
    uint32_t u32TimeOutCnt;

    LDROM_DEBUG("perform RESET\r\n");     
        
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while(!UART_IS_TX_EMPTY(UART1))
        if(--u32TimeOutCnt == 0) break;
    
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

    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)
    SYS_ResetChip();    
}

void ISP_timer_isr(void)
{
    timeout_cnt++;
}

void ISP_process(void)
{
    uint32_t u32TimeOutCnt;
    uint32_t lcmd;

    if ((bufhead >= 4) || (bUartDataReady == TRUE))
    {
        // uint32_t lcmd;
        lcmd = inpw(uart_rcvbuf);

        if (lcmd == CMD_CONNECT)
        {
            while (1)
            {
                if (bUartDataReady == TRUE)
                {         
                    bUartDataReady = FALSE;
                    ParseCmd(uart_rcvbuf, 64);
                    PutString();
                }
            }
        }
        else
        {
            bUartDataReady = FALSE;
            bufhead = 0;
        }
    }

    if (timeout_cnt > TIMEOUT_INTERVAL) {
        LDROM_DEBUG("Time-out, perform RESET\r\n");        
        u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
        while(!UART_IS_TX_EMPTY(DEBUG_UART_PORT))
            if(--u32TimeOutCnt == 0) break;

        // // Reset chip to enter bootloader
        SYS_UnlockReg();
        SYS_ResetChip();
    }
}

void ISP_jump_to_app(void)
{
    uint32_t u32TimeOutCnt;

    LDROM_DEBUG("Jump to <APPLICATION>\r\n");
     
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while(!UART_IS_TX_EMPTY(DEBUG_UART_PORT))
        if(--u32TimeOutCnt == 0) break;
    
    #if 1

    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();

    __set_PRIMASK(1);    
    FMC_SetVectorPageAddr(APROM_APPLICATION_START);

    // // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)
    // SYS_ResetChip();
  // Not reset I/O and peripherals
    SYS_ResetCPU();

    #else
    /* Reset system and boot from APROM */
    SYS->RSTSRC = (SYS_RSTSRC_RSTS_POR_Msk | SYS_RSTSRC_RSTS_RESET_Msk); /* Clear reset status flag */
    FMC->ISPCON = FMC->ISPCON & 0xFFFFFFFC;
    SCB->AIRCR = (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ);
    #endif
    
    /* Trap the CPU */
    while (1);   
}

void ISP_check_app(void)
{
    //
    // Stay in BOOTLOADER or jump to APPLICATION
    //
    if (!check_reset_source()) 
	{
        if (verify_application_chksum()) 
		{
            ISP_jump_to_app();
        } 
		else 
		{
            LDROM_DEBUG("BLOCK:checksum invaild\r\n");
        }
    } 
	else 
    {
        LDROM_DEBUG("BLOCK:from APPLICATION\r\n");
        
        //
        // start timer
        //
        LDROM_DEBUG("Time-out counter start\r\n");
        TIMER_Start(TIMER1);
    }
}

void ISP_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP */
    FMC->ISPCTL |=  FMC_ISPCTL_ISPEN_Msk;
    FMC_ENABLE_AP_UPDATE();

    /* Get APROM size, data flash size and address */
    g_apromSize = APROM_APPLICATION_SIZE;
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);

    /* Set Systick time-out for 300ms */
    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL  = (0x00);
    SysTick->CTRL = SysTick->CTRL | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;   /* Use CPU clock */
    
    // LDROM_DEBUG("%s\r\n",__FUNCTION__);   
}
