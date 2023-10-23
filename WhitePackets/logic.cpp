#include "logic.h"
#include "codecaves.h"
bool isconnected = false;
Pipe pipeToGui(L"pipeToGui");
Pipe pipeToDLL(L"pipeToDLL");
std::vector<WORD> blockedHeaders;

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
void mainLogic()
{
    while (true)
    {
        if (isconnected == false)
        {
            pipeToGui.createPipe();
            pipeToGui.waitForClient();
            isconnected = true;
            Sleep(1000);
            DWORD tID;
            HANDLE t1 = CreateThread(0,0,(LPTHREAD_START_ROUTINE)pipeHandler,0,0,&tID);
            setUpHooks();
        }
    }

}
void pipeHandler()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));

    while (!pipeToDLL.connectPipe())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    pipeMessage message = pipeToDLL.readPipeMessage();
    while (message.id != -1)
    {

        messageHandler(message);
        delete[] message.data;
        message = pipeToDLL.readPipeMessage();
    }
    std::cout << "Connection ended, pipe is no longer exist" << std::endl;
}

void messageHandler(pipeMessage message)
{
    switch (message.id)
    {
        case bHeader:
        {
            Header h = processHeader(message);
            switch (h.action)
            {
            case 0:blockedHeaders.erase(std::remove(blockedHeaders.begin(), blockedHeaders.end(), h.header), blockedHeaders.end()); break;
            case 1:blockedHeaders.emplace_back(h.header); break;
            }
        }
        case injectOutPacket:
        {
            Packet p = processPacketMessage(message);
            injectedPacket = p;
            SendPacket(p);
;       }
        break;
        case injectInPacket:
        {
            Packet p = processPacketMessage(message);
            injectedPacket = p;
            ProcessPacket(p);
        }
        break;
    } 
}
Header processHeader(pipeMessage message)
{
    return *(Header*)message.data;
}