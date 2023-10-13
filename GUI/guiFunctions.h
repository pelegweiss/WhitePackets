#pragma once
#include <string>
#include "injector.h"
#include <thread>
//#include "Pipe/Pipe.h"
#include <functional>
#include "Pipe.h"
#include "encoding.h"
bool runMaplestory(std::wstring maplestoryPath, std::wstring dllPath);

extern bool sniff;