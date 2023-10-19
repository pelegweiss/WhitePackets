#include "logic.h"
#include "codecaves.h"
bool isconnected = false;
Pipe pipeToGui(L"pipeToGui");
Pipe pipeToDLL(L"pipeToDLL");
std::vector<WORD> blockedHeaders;
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
            HANDLE t1 = CreateThread(
                0,
                0,
                (LPTHREAD_START_ROUTINE)pipeHandler,
                0,
                0,
                &tID
            );

            setUpHooks();

        }
        else
        {
            
            continue;
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
        message = pipeToDLL.readPipeMessage();
    }
    std::cout << "Connection ended, pipe is no longer exist" << std::endl;
}

void messageHandler(pipeMessage message)
{
    switch (message.id)
    {
        case 0:
        {
            Header h = processHeader(message);
            if (h.action == 0)
            {
                blockedHeaders.erase(std::remove(blockedHeaders.begin(), blockedHeaders.end(), h.header), blockedHeaders.end());

            }
            else
            {
                blockedHeaders.emplace_back(h.header);

            }
        }
    } 
}
Header processHeader(pipeMessage message)
{
    return *(Header*)message.data;
}