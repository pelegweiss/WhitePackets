#pragma once
#include <string>
#include "injector.h"
#include <thread>
//#include "Pipe/Pipe.h"
#include <functional>
#include "Pipe.h"
#include "encoding.h"
#include "controls.h"
bool runMaplestory(std::wstring maplestoryPath, std::wstring dllPath);

extern bool sniff;
extern ListView * lvPackets;
extern ListView* lvFilters;
extern bool isHeaderFiltered(std::vector<std::wstring> buf, ListView* lv);