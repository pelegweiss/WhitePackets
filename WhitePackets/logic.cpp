#include "pch.h"
#include "logic.h"
bool shouldStop = false;
void mainLogic()
{
    std::thread thread_obj(pipeHandler);
    Pipe pipeToGui(L"pipeToGui");
    pipeToGui.createPipe();
    pipeToGui.waitForClient();
	while (shouldStop == false)
	{
		std::cout << "hello world" << std::endl;
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

void messagesHandler(pipeMessage message)
{
    switch (message.id)
    {
        case changeState:
        {
            shouldStop = !shouldStop;
        }
        break;
    }
}