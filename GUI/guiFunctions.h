#pragma once
#include <string>
#include "Pipe/Pipe.h"
#include "injector.h"
#include <thread>
#include "Pipe/Pipe.h"
#include <functional>

bool runMaplestory(std::wstring maplestoryPath, std::wstring dllPath, std::function<void()> pipeHandlerFunc, Pipe pipeToCreate);