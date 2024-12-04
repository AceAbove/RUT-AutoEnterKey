#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>  
#include <cstdlib>  

const std::string CONFIG_FILE = "config.txt";

void CreateConfigFile() {
    std::ofstream configFile(CONFIG_FILE);
    if (configFile.is_open()) {
        configFile << "Rut location=\n";
        configFile << "Key=\n";
        configFile.close();
        std::cout << "Config file created successfully. Please edit the config file with the correct details and run the program again." << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.ignore();
    }
    else {
        std::cerr << "Unable to create config file." << std::endl;
    }
}
bool ReadConfigFile(std::string& windowTitle, std::string& key) {
    std::ifstream configFile(CONFIG_FILE);
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string keyName = line.substr(0, pos);
                std::string value = line.substr(pos + 1);

                if (keyName == "Rut location") {
                    windowTitle = value;
                }
                else if (keyName == "Key") {
                    key = value;
                }
            }
        }
        configFile.close();
        return true;
    }
    else {
        return false;
    }
}
void SendTextToConsoleWindow(const std::string& windowTitle, const std::string& messageToSend) {
    HWND consoleWindow = FindWindowA(NULL, windowTitle.c_str());
    if (consoleWindow == NULL) {
        std::cout << "Searching for target window... " << windowTitle << std::endl;
        return;
    }

    // Debug: Print original message
    std::cout << std::endl;

    
    std::string sentMessage;

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    for (size_t i = 0; i < messageToSend.size(); ++i) {
        PostMessage(consoleWindow, WM_CHAR, messageToSend[i], 0);
        sentMessage += messageToSend[i];
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        // Handle consecutive identical characters
        size_t j = i;
        while (j < messageToSend.size() - 1 && messageToSend[j] == messageToSend[j + 1]) {
            char randomLetter = 'X';
            PostMessage(consoleWindow, WM_CHAR, randomLetter, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            PostMessage(consoleWindow, WM_KEYDOWN, VK_BACK, 0);
            PostMessage(consoleWindow, WM_KEYUP, VK_BACK, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            PostMessage(consoleWindow, WM_CHAR, messageToSend[j + 1], 0);
            sentMessage += messageToSend[j + 1];
            ++j; // Move to the next character
        }
        i = j; // Update the outer loop index
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(0));

    // Send enter key
    PostMessage(consoleWindow, WM_KEYDOWN, VK_RETURN, 0);
    PostMessage(consoleWindow, WM_KEYUP, VK_RETURN, 0);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Attempting to initialise...\n";
}
void PeriodicallySearchAndSendText(const std::string& windowTitle, const std::string& messageToSend) {
    while (true) {
        HWND consoleWindow = FindWindowA(NULL, windowTitle.c_str());
        if (consoleWindow != NULL) {
            std::cout << "Window found: " << consoleWindow << std::endl;
            SendTextToConsoleWindow(windowTitle, messageToSend);
            std::cout << "Successfully sent key to console window!" << std::endl;
            break;
        }
        else {
            std::cout << "Searching... " << std::endl;
        }
        // Wait for a while before trying again
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
int main() {
    std::string windowTitle;
    std::string key;

    if (!ReadConfigFile(windowTitle, key)) {
        CreateConfigFile();
        return 0; // Exit the program after creating the config file
    }

    // Check if the config file has been filled out
    if (windowTitle.empty() || key.empty()) {
        std::cerr << "Config file is empty. Please fill out the config file with the correct details and run the program again." << std::endl;
        return 1;
    }

    // Launch the process
     system(("start " + windowTitle).c_str());

     PeriodicallySearchAndSendText(windowTitle, key);
    // Exit the program once the target window closes
    std::cout << "Target window closed. Exiting program..." << std::endl;
    return 0;
}
