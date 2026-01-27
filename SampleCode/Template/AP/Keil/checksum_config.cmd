@echo off
:: =========================================================
:: Flash absolute layout (DESIGN INTENT)
:: =========================================================

set APROM_BASE=0x0000
set APROM_SIZE=0x20000

set APP_START=0x3000
set APP_SIZE=0x1D000

:: CRC is always at the last 4 bytes of APROM
set CRC_ADDR=0x1FFFC
