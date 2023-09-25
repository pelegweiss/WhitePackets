#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <thread>
#include <map>

void pipeHandler();
struct pipeMessage
{
    int id;
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
    bool readMessage()
    {
        return 0;
        /*
        pipeMessage receivedMessage;

        DWORD bytesRead;
        DWORD typeSize;

        // Read the size of the type field
        if (!ReadFile(this->hNamedPipe, &typeSize, sizeof(DWORD), &bytesRead, NULL))
        {
            std::cerr << "Failed to read type size from input named pipe." << std::endl;
            receivedMessage.type = L"Error";
            return receivedMessage;
        }

        // Allocate memory for the type field and read it
        wchar_t* typeBuffer = new wchar_t[typeSize / sizeof(wchar_t)];
        if (!ReadFile(this->hNamedPipe, typeBuffer, typeSize, &bytesRead, NULL))
        {
            std::cerr << "Failed to read type from input named pipe." << std::endl;
            delete[] typeBuffer;
            receivedMessage.type = L"Error";
            return receivedMessage;
        }

        receivedMessage.type = std::wstring(typeBuffer, typeSize / sizeof(wchar_t));
        delete[] typeBuffer;

        // Initialize the data pointer as NULL
        receivedMessage.data = nullptr;

        return receivedMessage;
        */
        return 0;
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


int main()
{
    std::map<std::string, int> map;
    map["error"] = 0;
    map["stop"] = 1;
    map["send"] = 2;
    map["receive"] = 4;    
    std::thread thread_obj(pipeHandler);
    Pipe pipeToGui(L"pipeToGui");
    pipeToGui.createPipe();
    pipeToGui.waitForClient();
    while (true)
    {
        pipeMessage message;
        std::string type;
        std::cout << "Enter a messages type: ";
        std::cin >> type;
        message.id = map[type];
        std::cout << "Enter a messages data: ";
        std::wcin >> message.data;
        pipeToGui.sendMessage(message);

    }
}

void pipeHandler()
{
    using namespace std::literals::chrono_literals;
    Pipe pipeToDLL(L"pipeToDLL");
    while (!pipeToDLL.connectPipe())
    {
        std::cout << "Failed to connect pipe, retrying" << std::endl;
        std::this_thread::sleep_for(1s);
    }
    while (true)
    {
        pipeMessage message;
        //message = pipeToDLL.readMessage();
    }
    std::cout << "Connection ended, pipe is no longer exist" << std::endl;
}