#pragma once
#include <iostream>
#include <windows.h>
#include <vector>
#include <thread>
//#include <Pipe/Pipe.h>
#include "Pipe.h"
void mainLogic();
void pipeHandler();
void messageHandler(pipeMessage message);
enum messagesIDS
{
	sendID, recieveID
};

Header processHeader(pipeMessage message);
extern Packet injectedPacket;