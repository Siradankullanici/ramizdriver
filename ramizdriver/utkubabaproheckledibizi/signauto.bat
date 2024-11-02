@echo off

:: Remove any existing signature from the .sys file
signtool.exe remove /s "utkudorukbayraktarantivirus.sys"

:: Sign the driver with the PFX password
signtool.exe sign /f "UTKUBABAPRO.pfx" /p "UTKUBABAPRO" /fd SHA256 /t http://timestamp.digicert.com /a "utkudorukbayraktarantivirus.sys"

:: Install the certificate to the Trusted Root Certification Authorities to prevent "invalid signature" warnings
certutil -addstore "Root" UTKUBABAPRO.cer

echo Driver signed successfully, and certificate added to Trusted Root store!
pause
