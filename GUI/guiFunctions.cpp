#include "guiFunctions.h"
#include <sstream>
#include "controls.h"
#include <iomanip>

extern ListView* lvPackets;
Pipe pipeToGui(L"pipeToGui");
bool allBytesAreASCII(const std::vector<unsigned char>& bytes) {
    for (unsigned char byte : bytes) {
        if (byte > 127) {
            return false; // Byte is not in the ASCII range
        }
    }
    return true; // All bytes are in the ASCII range
}
std::wstring bytesToWString(const std::vector<unsigned char>& bytes) {
    std::wstring result;
    for (unsigned char byte : bytes) {
        result += static_cast<wchar_t>(byte);
    }
    return result;
}

void messagesHandler(pipeMessage message)
{
    switch (message.id)
    {
    case 0:
    {

    }
    break;
    case 1:
    {
        std::wstringstream callerAddress;
        callerAddress << std::hex << std::uppercase << message.data.callerAddress;
        std::wstring hexStringCallerAddress = callerAddress.str();
        std::wstring callerAddressFinal = L"0X" + hexStringCallerAddress;

        std::wstringstream header;
        header << std::setfill(L'0') << std::setw(4) << std::uppercase << message.data.header;
        std::wstring hexStringHeader = header.str();


        std::vector<std::wstring> buf;
        buf.emplace_back(callerAddressFinal);
        buf.emplace_back(L"Send");
        buf.emplace_back(hexStringHeader);
        std::wstring data;
        message.data.data.erase(message.data.data.begin());
        for (int i = 0; i < message.data.data.size(); i++)
        {
            std::wstringstream ss;

            // Convert each byte to hexadecimal and store it in the wide string stream
            int elementSize = message.data.data.at(i).size();
            bool isGreaterThan4Bytes = false;
            if (elementSize > 4)
            {
                isGreaterThan4Bytes = true;
            }
            for (size_t j = 0; j < elementSize; ++j) {
                ss << std::hex << std::setw(2) << std::setfill(L'0') << std::uppercase << static_cast<int>(message.data.data[i].at(j));
            }
            if (isGreaterThan4Bytes == true)
            {
                bool isAscii = allBytesAreASCII(message.data.data.at(i));
                if (isAscii == true)
                {
                    ss << L"\"[" << bytesToWString(message.data.data.at(i)) << L"\"]" << L' ';
                }
            }
            else
            {
                ss << L' ';

            }


            // Get the resulting wstring
            std::wstring hexString = ss.str();
            if (!hexString.empty())
            {
                hexString.erase(hexString.size() - 1);

            }

            data += hexString + L" ";
        }
        if (data.empty())
        {
            buf.emplace_back(L"");
        }
        else
        {
            data.erase(data.size() - 1);
            buf.emplace_back(data);

        }
        lvPackets->add_item(buf);

    }
    break;

    }
}
void pipeHandler()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));

    while (!pipeToGui.connectPipe())
    {
        std::cout << "Failed to connect pipe, retrying" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    pipeMessage message = pipeToGui.readMessage();
    while (true)
    {

        messagesHandler(message);
        message = pipeToGui.readMessage();
    }
    std::cout << "Connection ended, pipe is no longer exist" << std::endl;
}
bool runMaplestory(std::wstring maplestoryPath, std::wstring dllPath)
{
    const wchar_t* mPath = L"C:\\Users\\Peleg\\Desktop\\Personal\\Heaven.MS.v83.pack\\HeavenMS - Source\\HeavenMS-master\\HeavenMS-master\\MapleStory\\HeavenMS-localhost-WINDOW.exe";
    const wchar_t* dPath = L"C:\\WhitePackets\\WhitePackets\\Debug\\WhitePackets.dll";
    ShellExecute(nullptr, L"open", mPath, nullptr, nullptr, SW_SHOWNORMAL);
    Sleep(3000);
    int i = 0;
    DWORD procID = GetProcId(L"HeavenMS-localhost-WINDOW.exe");
    while (procID == NULL && i < 10)
    {
        Sleep(50);
        i++;
        procID = GetProcId(L"HeavenMS-localhost-WINDOW.exe");

    }
    if (i == 10)
    {
        std::cout << "Failed to launch" << std::endl;
        return false;
    }
    else
    {
        std::cout << "Maplestory launched sucssesfully going to inject" << std::endl;
        Sleep(1000);
        if (inject(L"HeavenMS-localhost-WINDOW.exe", dPath) == true)
        {
            //std::thread thread_obj(pipeHandlerFunc);
            DWORD tID;
            HANDLE t1 = CreateThread(
                0,
               0,
                (LPTHREAD_START_ROUTINE)pipeHandler,
                0,
                0,
                &tID
            );

            //pipeToCreate.createPipe();
            //pipeToCreate.waitForClient();
            return true;
        }
        else
        {
            std::cout << "Failed injecting DLL" << std::endl;
            return false;

        }
    }
}
