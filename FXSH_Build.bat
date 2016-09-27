@echo off
rem Do not edit! This batch file is created by CASIO fx-9860G SDK.


if exist HEX.G1A  del HEX.G1A

cd debug
if exist FXADDINror.bin  del FXADDINror.bin
"C:\Users\Nozalia\Desktop\Add-ins\fx9860g_sdk\SDK\OS\SH\Bin\Hmake.exe" Addin.mak
cd ..
if not exist debug\FXADDINror.bin  goto error

"C:\Users\Nozalia\Desktop\Add-ins\fx9860g_sdk\SDK\Tools\MakeAddinHeader363.exe" "C:\Users\Nozalia\Desktop\Add-ins\Projects\Hex"
if not exist HEX.G1A  goto error
echo Build has completed.
goto end

:error
echo Build was not successful.

:end

