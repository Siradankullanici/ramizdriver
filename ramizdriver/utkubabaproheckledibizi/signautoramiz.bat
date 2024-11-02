@echo off

:: Remove any existing signature from the .sys file
signtool.exe remove /s "ramiz.sys"

:: Sign the driver with the PFX password
signtool.exe sign /f "UTKUBABAPRO.pfx" /p "UTKUBABAPRO" /fd SHA256 /t http://timestamp.digicert.com /a "ramiz.sys"

echo Driver signed successfully, and certificate added to Trusted Root store!
pause
