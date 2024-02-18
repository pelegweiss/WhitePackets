#pragma once
#include <string>
#include "injector.h"
#include <thread>
#include "Pipe/Pipe.h"
#include <functional>
#include "encoding.h"
#include "controls.h"
#include <windowsx.h>
bool runMaplestory(std::wstring maplestoryPath, std::wstring dllPath);

extern bool sniff;
extern ListView * lvPackets;
extern ListView* lvFilters;
extern Control* autoInject;
extern bool isHeaderFiltered(std::vector<std::wstring> buf, ListView* lv);
void pipeHandler();
Packet processPacketMessage(pipeMessage message);
extern BOOL isPipeToDLLConnected;
extern std::vector<WORD> blockedHeaders;
