#pragma once
#include "hook.h"
#include "addresses.h"
Hook* sendPacketHook;
Hook* recvPacketHook;

Hook* CoutPacketHook;
Hook* Encode1Hook;
Hook* Encode2Hook;
Hook* Encode4Hook;
Hook* EncodeStringHook;
Hook* EncodeBufferHook;

Hook* Decode1Hook;
Hook* Decode2Hook;
Hook* Decode4Hook;
Hook* DecodeStringHook;
Hook* DecodeBufferHook;
