#include "guiFunctions.h"
#include <sstream>
#include "controls.h"
#include <iomanip>


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

    while (!pipeToGui.connectPipe())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    pipeMessage message = pipeToGui.readPipeMessage();
    while (message.id != -1)
    {

        messagesHandler(message);
        message = pipeToGui.readPipeMessage();
    }
    std::cout << "Connection ended, pipe is no longer exist" << std::endl;
    CloseHandle(pipeToDLL.hNamedPipe);
    isPipeToDLLConnected = false;
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
            DWORD tID;
            HANDLE t1 = CreateThread(
                0,
               0,
                (LPTHREAD_START_ROUTINE)pipeHandler,
                0,
                0,
                &tID
            );
            pipeToDLL.createPipe();
            pipeToDLL.waitForClient();
            isPipeToDLLConnected = true;
                    


            for (int i = 0; i < blockedHeaders.size(); i++)
            {
                WORD wordValue = blockedHeaders.at(i);
                void* voidPointer = (void*)&wordValue; // Casting DWORD to void*
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
        else
        {
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