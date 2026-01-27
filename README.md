# M2A23BSP_IAP_UART_app
M2A23BSP_IAP_UART_app

update @ 2026/01/23

1. under boot code project (ISP_UART) , add SW CRC32  , check define : ENABLE_SW_CRC32 to enable

2. check define : USE_SRAM_TABLE , USE_FLASH_TABLE , to use CRC32 table put in SRAM or FLASH

3. Scenario notice:

	- Boot loader project : ISP_UART 
	
		- under sct file (uart_iap.sct) , will allocate flash size 
							
		APROM_Bootloader.bin : 0x00000 0x3000 (reserve 12K size , to store extra boot loader code) 

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/boot_KEIL_sct.jpg)		
	
		- when power on , will check power on source (ex : power on reset , nReset , from application code)
	
		- use CRC to calculate Application code checksum (length : 0x20000-4 : 0x1FFFC )
		
		- load Application code checksum , from specific address (at 0x20000 last 4 bytes : 0x1FFFC)
		
		- power on from ISP_UART , and CRC calculate correct		

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/boot_from_boot_to_app.jpg)
				
		- if two checksum result are different , will stuck in Boot loader , and wait for ISP code update
		
![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/boot_checksum_err.jpg)		
		
		- if boot from application code , to ISP_UART , by press digit Z

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_press_Z_to_boot.jpg)	

		- if boot from application code , to ISP_UART , by press nRESET pin in EVM

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_press_nRESET_to_boot.jpg)	

		- when use ISP tool , select APROM , Reset and run 
		
![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/ISP_connect.jpg)

		- when use ISP tool , during update 
		
![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/ISP_during_update.jpg)

		- under BOOT code  , during update 
		
![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/boot_during_upgrade.jpg)

		- when ISP upgrade finish ( compare check and jump to app code)
		
![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/boot_upgrade_finish.jpg)

	
	- Application code project : AP

		- under sct file (APROM_application.sct) , will allocate app code flash size (0x3000 ~ 0x20000 , size : 0x1D000)
		
![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_KEIL_sct.jpg)		
	
		- use SRecord , to calculate application code checksum , add binary to hex , by SRecord tool

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_KEIL_checksum_calculate.jpg)
	
		- SRecord file : srec_cat.exe 

		- under generateChecksum.bat will execute checksum_config.cmd ( MODIFY address base on requirement )
		
		- check sum calculate will start from 0x3000 to 0x20000-4 : 0x1FFFC , and store in 0x1FFFC , the last 4 bytes 
				
![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_KEIL_output_file.jpg)

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_SRecord_cmd_file.jpg)
		
		- after project compile finish , binary size will be 116K (total application code size : 0x1D000)
		
		- under app code , press 1 , will erase checksum , and return to BOOT code

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_erase_checksum.jpg)		
		
				
4. Flash allocation
	
	- APROM_Bootloader.bin : 0x0000 0x3000
	
	- APROM_application.bin : 0x3000 0x20000
			
	- Application code Chcecksum storage : 0x1FFFC

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/FLASH_calculate.jpg)
	
5. Function assignment

	- debug port : UART1 (PA8 , PA9) , in Boot loader an Application code project
	
	- ISP UART port : UART0 (PB12 , PB13) , in Boot loader project
		
6. Need to use ICP tool , to programm boot loader project file (APROM_Bootloader.bin @ APROM 0x0000)

below is boot loader project , Config setting (APROM WITH IAP)

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/boot_ICP_Config.jpg)

below is boot loader project , ICP programming setting 

- APROM_Bootloader.bin : @ APROM 0x0000

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/boot_ICP_Update.jpg)

7. under Application code KEIL project setting 

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_KEIL_checksum_calculate.jpg)

in Application project , press 'z' , 'Z' will reset to Boot loader 

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/app_press_Z_to_boot.jpg)

8. under boot loader project , below is sct file content

![image](https://github.com/released/M2A23BSP_IAP_UART_APROM/blob/main/boot_KEIL_sct.jpg)
