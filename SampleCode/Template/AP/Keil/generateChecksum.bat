@echo off
setlocal

call checksum_config.cmd

set SREC=srec_cat

set APP_BIN=obj\APROM_application.bin
set TMP_IMG=obj\_aprom_crc_tmp.bin

echo ========================================================
echo Generate CRC32 (ABSOLUTE address semantics)
echo --------------------------------------------------------
echo APP_START  = %APP_START%
echo APP_SIZE   = %APP_SIZE%
echo CRC_ADDR   = %CRC_ADDR%
echo CRC_OFFSET = %CRC_OFFSET%
echo APP_BIN    = %APP_BIN%
echo ========================================================

:: --------------------------------------------------------
:: Step 1: Build temporary APROM image and calculate CRC
:: (Used only for CRC calculation / dump)
:: --------------------------------------------------------
%SREC% ^
  %APP_BIN% -binary ^
  -offset %APP_START% ^
  -fill 0xFF %APROM_BASE% %APROM_SIZE% ^
  -crop %APROM_BASE% %CRC_ADDR% ^
  -crc32-l-e %CRC_ADDR% ^
  -o %TMP_IMG% -binary

if errorlevel 1 goto err

:: --------------------------------------------------------
:: Step 2: Dump checksum (last 4 bytes) to terminal
:: --------------------------------------------------------
echo.
echo ---- CRC32 @ 0x%CRC_ADDR% (HEX dump) ----
%SREC% ^
  %TMP_IMG% -binary ^
  -crop %CRC_ADDR% %APROM_SIZE% ^
  -o - -HEX_Dump

:: --------------------------------------------------------
:: Step 3: Write CRC back to app-only binary (relative offset)
:: --------------------------------------------------------
%SREC% ^
  %APP_BIN% -binary ^
  -fill 0xFF 0x0000 %APP_SIZE% ^
  -crop 0x0000 %CRC_OFFSET% ^
  -crc32-l-e %CRC_OFFSET% ^
  -o %APP_BIN% -binary

if errorlevel 1 goto err

echo.
echo CRC written back to app-only binary successfully.
exit /b 0

:err
echo CRC generation FAILED
exit /b 1
