#pragma once
#include <iostream>
#include <windows.h>
#include <vector>
#include <thread>
#include <Pipe/Pipe.h>
void mainLogic();
void pipeHandler();
void messagesHandler(pipeMessage message);
enum messagesIDS
{
	sendID, recieveID
};