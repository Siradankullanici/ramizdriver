@echo off
del UTKUBABAPRO.pfx 2>nul

:: Convert the PVK and CER files to a PFX file
pvk2pfx.exe -pvk UTKUBABAPRO.pvk -spc UTKUBABAPRO.cer -pfx UTKUBABAPRO.pfx -po UTKUBABAPRO

echo PFX file created successfully: UTKUBABAPRO.pfx
pause
