#pragma once
#include <Windows.h>
#include <iostream>
class clientPipe
{
public:
    std::wstring pipeName;
    HANDLE hNamedPipe;

    clientPipe(std::wstring name)
    {
        this->pipeName = L"\\\\.\\pipe\\" + name;
    };
    bool connectPipe()
    {
        this->hNamedPipe = CreateFile(
            pipeName.c_str(),  // Pipe name
            GENERIC_READ,               // Desired access (read-only)
            0,                          // Share mode (0 means no sharing)
            NULL,                       // Security attributes
            OPEN_EXISTING,              // Open an existing pipe
            0,                          // File attributes
            NULL                        // Template file
        );

        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            std::wcout << "Failed to open pipe: " << this->pipeName << std::endl;
            return false;
        }
        std::wcout << "Connected Pipe: " << this->pipeName << std::endl;
        return true;
    }
    void readMessage()
    {
        const int bufferSize = 100;
        char buffer[bufferSize];
        DWORD bytesRead;

        // Read data from the named pipe
        if (!ReadFile(this->hNamedPipe, &buffer, 100, &bytesRead, NULL)) {
            std::cerr << "Failed to read from named pipe." << std::endl;
            CloseHandle(hNamedPipe);
        }

        std::wcout << "Data received from Data Provider: " << buffer << std::endl;
    }

};
int main()
{

}
