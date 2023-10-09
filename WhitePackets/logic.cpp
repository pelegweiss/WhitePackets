#include "logic.h"
#include "codecaves.h"
bool isconnected = false;
Pipe pipeToGui(L"pipeToGui");


void mainLogic()
{
    while (true)
    {
        if (isconnected == false)
        {
            //std::thread thread_obj(pipeHandler);
            pipeToGui.createPipe();
            pipeToGui.waitForClient();
            std::cout << "Connected" << std::endl;
            isconnected = true;
            setUpHooks();

        }
        else
        {
            
            continue;
        }
    }
    Sleep(1000);

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

void messagesHandler(pipeMessage message)
{

}