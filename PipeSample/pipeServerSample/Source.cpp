#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <thread>
void pipeHandler();

struct pipeMessage
{
    std::wstring type;
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
    }
    bool sendMessage(const pipeMessage& message)
    {
        DWORD bytesWritten;

        // Serialize the message into a byte array
        std::wstring serializedType = message.type;
        DWORD typeSize = static_cast<DWORD>(serializedType.size() * sizeof(wchar_t));

        // Calculate the total message size
        DWORD totalSize = sizeof(DWORD) + typeSize;

        // Allocate memory for the serialized message
        BYTE* serializedMessage = new BYTE[totalSize];

        // Copy the type size and type data into the serialized message
        memcpy(serializedMessage, &typeSize, sizeof(DWORD));
        memcpy(serializedMessage + sizeof(DWORD), serializedType.c_str(), typeSize);

        // Send the serialized message over the pipe
        if (!WriteFile(this->hNamedPipe, serializedMessage, totalSize, &bytesWritten, NULL))
        {
            delete[] serializedMessage;
            return false;
        }

        delete[] serializedMessage;
        return true;
    }

};


int main()
{
    std::thread thread_obj(pipeHandler);
    Pipe pipeToGui(L"pipeToGui");
    pipeToGui.createPipe();
    pipeToGui.waitForClient();
    while (true)
    {
        pipeMessage message;
        std::cout << "Enter a messages type: ";
        std::wcin >> message.type;
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
        message = pipeToDLL.readMessage();
    }
    std::cout << "Connection ended, pipe is no longer exist" << std::endl;
}