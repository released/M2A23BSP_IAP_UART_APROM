@echo off
:: =========================================================
:: Flash absolute layout (semantic definition)
:: =========================================================

set APROM_BASE=0x0000
set APROM_SIZE=0x20000

set APP_START=0x3000
set APP_SIZE=0x1D000

set CRC_ADDR=0x1FFFC

:: =========================================================
:: Derived (used internally by batch)
:: =========================================================
:: Relative offset inside app-only binary
set CRC_OFFSET=0x1CFFC
