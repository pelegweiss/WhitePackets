#include "guiFunctions.h"
#include <sstream>
#include "controls.h"
#include <iomanip>
#include <filesystem>

Pipe pipeToGui(L"pipeToGui");
Pipe pipeToDLL(L"pipeToDLL");
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

            if (!sniff)
                break;

            Packet p = processPacketMessage(message);
            // Convert caller address and header to byte vectors
            std::vector<BYTE> callerAddressBytes = convertToBytes(p.callerAddress);
            std::vector<BYTE> headerBytes = convertToBytes(p.header);
            std::wstring callerAddressString = L"0x" + toHexString(callerAddressBytes);
            std::wstring headerString = toHexString(headerBytes);

            std::vector<std::wstring> filterBuffer;
            filterBuffer.emplace_back((std::wstring)L"Filter");
            filterBuffer.emplace_back(headerString);
            if (!isHeaderFiltered(filterBuffer, lvFilters))
            {

                std::vector<std::wstring> buf;
                buf.emplace_back(callerAddressString);
                buf.emplace_back(L"Send");
                buf.emplace_back(headerString);

                p.segments.erase(p.segments.begin());
                std::wstring data;
                data = segmentsToWstring(p.segments);
                buf.emplace_back(data);
                lvPackets->add_item(buf);
            }


        }
        break;
        case 2:
        {
            if (!sniff)
                break;

            Packet p = processPacketMessage(message);
            // Convert caller address and header to byte vectors
            std::vector<BYTE> callerAddressBytes = convertToBytes(p.callerAddress);
            std::vector<BYTE> headerBytes = convertToBytes(p.header);
            std::wstring callerAddressString = L"0x" + toHexString(callerAddressBytes);
            std::wstring headerString = toHexString(headerBytes);

            std::vector<std::wstring> filterBuffer;
            filterBuffer.emplace_back((std::wstring)L"Filter");
            filterBuffer.emplace_back(headerString);
            if (!isHeaderFiltered(filterBuffer, lvFilters))
            {

                std::vector<std::wstring> buf;
                buf.emplace_back(callerAddressString);
                buf.emplace_back(L"Recv");
                buf.emplace_back(headerString);
                p.segments.erase(p.segments.begin());
                std::wstring data;
                data = segmentsToWstring(p.segments);
                buf.emplace_back(data);
                lvPackets->add_item(buf);
            }

            break;


        }
        break;

    }
}
void pipeHandler()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    bool isConnected = pipeToGui.connectPipe();
    if (!isConnected)
    {
        std::cout << "Failed connecting, please restart the program";

    }
    else
    {
        pipeMessage message = pipeToGui.readPipeMessage();
        while (message.id != -1)
        {

            messagesHandler(message);
            delete[] message.data;
            message = pipeToGui.readPipeMessage();
        }
        CloseHandle(pipeToDLL.hNamedPipe);
        isPipeToDLLConnected = false;
        std::cout << "Connection ended, pipe is no longer exist" << std::endl;
    }


}
bool runMaplestory(std::wstring maplestoryPath, std::wstring dllPath) {
    // Launch Maplestory
    std::wstring processToInject = L"HeavenMS-localhost-WINDOW.exe";
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path filePath(maplestoryPath);
    SetCurrentDirectory(filePath.parent_path().wstring().c_str());
    ShellExecute(nullptr, L"open", maplestoryPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    SetCurrentDirectory(currentPath.wstring().c_str());
    // Check if the process has started
    int retries = 0;
    const int maxRetries = 10;
    
    DWORD procID = GetProcId(processToInject.c_str());//L"HeavenMS-localhost-WINDOW.exe");

    while (procID == NULL && retries < maxRetries) {
        Sleep(100);
        procID = GetProcId(processToInject.c_str());//L"HeavenMS-localhost-WINDOW.exe");
        retries++;
    }

    if (retries == maxRetries) {
        std::cout << "Failed to launch" << std::endl;
        return false;
    }
    else {
        std::cout << "Maplestory launched successfully, preparing for injection" << std::endl;

        if (inject(processToInject.c_str(), dllPath.c_str())) {
            DWORD tID;
            HANDLE t1 = CreateThread(
                0,
                0,
                (LPTHREAD_START_ROUTINE)pipeHandler,
                0,
                0,
                &tID
            );

            // Create pipe and connect to client
            pipeToDLL.createPipe();
            isPipeToDLLConnected = pipeToDLL.waitForClient();
            // Send blocked headers
            for (int i = 0; i < blockedHeaders.size(); i++) {
                WORD wordValue = blockedHeaders.at(i);
                pipeMessage message;
                Header h;
                h.action = 1;
                h.header = wordValue;
                message.id = bHeader;
                message.data = (void*)&h;
                pipeToDLL.sendBlockHeaderMessage(message);
            }
            return true;
        }
        else {
            std::cout << "Failed injecting DLL" << std::endl;
            return false;
        }
    }
}

Packet processPacketMessage(pipeMessage message)
{
    Packet p;
    DWORD callerAddress;
    WORD header;
    std::vector<Segment> segments;
    int segmentsLen;
    int offSet = 0;
    memcpy(&callerAddress, message.data, sizeof(DWORD));
    offSet += 4;
    memcpy(&header, (BYTE*)message.data + offSet, 2);
    offSet += 2;
    memcpy(&segmentsLen, (BYTE*)message.data + offSet, 4);
    offSet += 4;
    for (int i = 0; i < segmentsLen; i++)
    {
        Segment seg;
        std::vector<BYTE> bytes;
        int type;
        int elementLen;
        memcpy(&type, (BYTE*)message.data + offSet, 4);
        offSet += 4;
        memcpy(&elementLen, (BYTE*)message.data + offSet, 4);
        offSet += 4;
        for (int j = 0; j < elementLen; j++)
        {
            BYTE b;
            memcpy(&b, (BYTE*)message.data + offSet, 1);
            offSet += 1;
            bytes.emplace_back(b);
        }
        seg.type = type;
        seg.bytes = bytes;
        segments.emplace_back(seg);
    }
    p.callerAddress = callerAddress;
    p.header = header;
    p.segments = segments;
    return p;
}