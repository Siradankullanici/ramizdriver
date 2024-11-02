@echo off
del UTKUBABAPRO.pvk 2>nul
del UTKUBABAPRO.cer 2>nul

:: Remove previous certificates with the same name
certutil -delstore my "UTKU DORUK BAYRAKTAR"

:: Create a self-signed certificate with makecert.exe
makecert.exe -r -pe -n "CN=UTKU DORUK BAYRAKTAR, E=semaemirhan555@gmail.com" -sv UTKUBABAPRO.pvk UTKUBABAPRO.cer -len 2048 -b 01/01/2024 -e 01/01/2034

pause
