#include "hooks.h"
#include "codecaves.h"
void setUpHooks()
{
    sendPacketHook = new Hook((BYTE*)CClientSocket_SendPacketAddress, (BYTE*)&my_send_packet, 5);
    recvPacketHook = new Hook((BYTE*)CClientSocket_ProcessPacketAddress, (BYTE*)&my_recv_packet, 5);

    CoutPacketHook = new Hook((BYTE*)COutPacket_COutPacketAddress, (BYTE*)&my_coutpacket, 5);
    Encode1Hook = new Hook((BYTE*)COutPacket_Encode1Address, (BYTE*)&my_encode1, 5);
    Encode2Hook = new Hook((BYTE*)COutPacket_Encode2Address, (BYTE*)&my_encode2, 5);
    Encode4Hook = new Hook((BYTE*)COutPacket_Encode4Address, (BYTE*)&my_encode4, 5);
    EncodeStringHook = new Hook((BYTE*)COutPacket_EncodeStrAddress, (BYTE*)&my_encodestr, 5);
    EncodeBufferHook = new Hook((BYTE*)COutPacket_EncodeBufferAddress, (BYTE*)&my_encodebuffer, 6);

    Decode1Hook = new Hook((BYTE*)CInPacket_Decode1Address, (BYTE*)&my_decode1, 7);
    Decode2Hook = new Hook((BYTE*)CInPacket_Decode2Address, (BYTE*)&my_decode2, 7);
    Decode4Hook = new Hook((BYTE*)CInPacket_Decode4Address, (BYTE*)&my_decode4, 7);
    DecodeStringHook = new Hook((BYTE*)CInPacket_DecodeStrAddress, (BYTE*)&my_decodeStr, 7);
    DecodeBufferHook = new Hook((BYTE*)CInPacket_DecodeBufferAddress, (BYTE*)&my_decodeBuffer, 6);

    sendPacketHook->Enable();
    recvPacketHook->Enable();
    CoutPacketHook->Enable();
    Encode1Hook->Enable();
    Encode2Hook->Enable();
    Encode4Hook->Enable();
    EncodeStringHook->Enable();
    EncodeBufferHook->Enable();
    Decode1Hook->Enable();
    Decode2Hook->Enable();
    Decode4Hook->Enable();
    DecodeStringHook->Enable();
    DecodeBufferHook->Enable();
}