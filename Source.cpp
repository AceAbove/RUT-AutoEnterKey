#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>

bool CreateConfigFileIfNeeded() {
    std::ifstream configFile("config.txt");
    if (!configFile) {
        
        std::ofstream outFile("config.txt");
        if (outFile) {
            outFile << "YourRutLocation=C:\\Users\\Username\\Downloads\\\n";
            outFile << "YourKey=your key\n";
            outFile.close();
            std::cout << "Config file created. Please edit config.txt with your specific details.\n";
            std::cout << "Then re-launch this application.\n";
            std::cout << "Press Enter to exit..." << std::endl;
            std::cin.get(); 
            return true; 
        }
        else {
            std::cerr << "Failed to create config file\n";
            return false;
        }
    }
    return false;
}

void ReadConfigFile(std::string& rutLocation, std::string& key) {
    std::ifstream configFile("config.txt");
    if (!configFile) {
        std::cerr << "Could not open config.txt" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        size_t delimiterPos = line.find("=");
        if (delimiterPos != std::string::npos) {
            std::string configKey = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            if (configKey == "YourRutLocation") {
                rutLocation = value;
            }
            else if (configKey == "YourKey") {
                key = value;
            }
        }
    }
}

void SendTextToConsoleWindow(const std::string& rutLocation, const std::string& key) {
    HWND consoleWindow = FindWindowA(NULL, rutLocation.c_str());
    if (consoleWindow == NULL) {
        std::cout << "Searching for target window... " << rutLocation << std::endl;
        return;
    }
    SetForegroundWindow(consoleWindow);
    while (FindWindowA(NULL, rutLocation.c_str()) != NULL) {
        for (char c : key) {
            PostMessage(consoleWindow, WM_CHAR, c, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
        }
        PostMessage(consoleWindow, WM_KEYDOWN, VK_RETURN, 0);
        PostMessage(consoleWindow, WM_KEYUP, VK_RETURN, 0);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Entering key...\n";
    }
}

void PeriodicallySearchAndSendText(const std::string& rutLocation, const std::string& key) {
    while (true) {
        HWND consoleWindow = FindWindowA(NULL, rutLocation.c_str());
        if (consoleWindow != NULL) {
            std::cout << "Window found: " << consoleWindow << std::endl;
            SendTextToConsoleWindow(rutLocation, key);
            std::cout << "Successfully sent key to console window!" << std::endl;
            break;
        }
        else {
            std::cout << "Searching for window... " << rutLocation << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::string rutLocation;
    std::string key;

    if (CreateConfigFileIfNeeded()) {
        return 0;
    }

    ReadConfigFile(rutLocation, key);

    system(("start " + rutLocation).c_str());

    PeriodicallySearchAndSendText(rutLocation, key);

    std::cout << "Target window closed. Exiting program..." << std::endl;
    return 0;
}
