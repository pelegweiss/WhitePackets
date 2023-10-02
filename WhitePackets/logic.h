#pragma once
#include <iostream>
#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <thread>
#include <map>
#include <codecvt>
#include "pipe.h"
void mainLogic();
void pipeHandler();
void messagesHandler(pipeMessage message);
enum messagesIDS
{
	changeState, sendPacket, recievePacket, getPacketData
};