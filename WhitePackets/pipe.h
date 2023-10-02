#pragma once
#include <Windows.h>
#include <iostream>
#include <codecvt>
#include <vector>
struct pipeMessage
{
    int id;           // ID field
    std::wstring data;
};
class Pipe
{
public:
    std::wstring pipeName;
    HANDLE hNamedPipe;

    Pipe(std::wstring name)
    {
        this->pipeName = L"\\\\.\\pipe\\" + name;
    };
    bool createPipe()
    {
        this->hNamedPipe = CreateNamedPipe(
            this->pipeName.c_str(),  // Pipe name
            PIPE_ACCESS_OUTBOUND,      // Outbound access
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
            1,                          // Max instances
            0,                          // Buffer size
            0,                          // Output buffer size
            0,                          // Default timeout
            NULL                        // Security attributes
        );

        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create named pipe." << std::endl;
            return false;
        }
        return true;
    }
    bool waitForClient()
    {
        if (!ConnectNamedPipe(this->hNamedPipe, NULL)) {
            std::cerr << "Failed to connect to named pipe." << std::endl;
            CloseHandle(hNamedPipe);
            return false;
        }
        return true;
    }
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
    pipeMessage readMessage()
    {
        pipeMessage receivedMessage;

        DWORD bytesRead;

        int messageID;
        int dataLen;

        if (!ReadFile(this->hNamedPipe, &messageID, sizeof(int), &bytesRead, NULL))
        {
            receivedMessage.id = -1; // Indicate an error
            return receivedMessage;
        }
        SetFilePointer(this->hNamedPipe, 4, NULL, FILE_CURRENT);
        
        if (!ReadFile(this->hNamedPipe, &dataLen, sizeof(int), &bytesRead, NULL))
        {
            receivedMessage.id = -1; // Indicate an error
            return receivedMessage;
        }
        SetFilePointer(this->hNamedPipe, dataLen, NULL, FILE_CURRENT);

        std::vector<char> buffer(dataLen);
        // Read the string data from the pipe into the buffer
        if (!ReadFile(this->hNamedPipe, buffer.data(), dataLen, &bytesRead, NULL))
        {
            receivedMessage.id = -1; // Indicate an error
            return receivedMessage;
        }
        receivedMessage.id = messageID;
        std::wstring wideData = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(buffer.data(), dataLen));

        receivedMessage.data = wideData;
        return receivedMessage;
    }
    bool sendMessage(const pipeMessage& message)
    {
        DWORD bytesWritten;

        // Serialize the message into a byte vector
        std::vector<BYTE> serializedData;

        // Serialize the id
        serializedData.insert(serializedData.end(), reinterpret_cast<const BYTE*>(&message.id), reinterpret_cast<const BYTE*>(&message.id) + sizeof(int));

        // Serialize the string length
        int strLength = static_cast<int>(message.data.size());
        serializedData.insert(serializedData.end(), reinterpret_cast<const BYTE*>(&strLength), reinterpret_cast<const BYTE*>(&strLength) + sizeof(int));

        // Serialize the string data
        serializedData.insert(serializedData.end(), message.data.begin(), message.data.end());

        // Send the serialized message over the pipe
        if (!WriteFile(this->hNamedPipe, serializedData.data(), static_cast<DWORD>(serializedData.size()), &bytesWritten, NULL))
        {
            return false;
        }

        return true;
    }

};
