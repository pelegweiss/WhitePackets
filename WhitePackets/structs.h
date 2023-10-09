#pragma once


struct COutPacket
{
    int m_bLoopback;
    unsigned char* m_aSendBuff;
    unsigned int m_uOffset;
    int m_bIsEncrpytedByShanda;
};
struct CInPacket
{
    int m_bLoopBack;
    long m_State;
    unsigned char* m_aRecvBuff;
    unsigned short m_uLength;
    unsigned short m_uRawSeq;
    unsigned short m_uDatalen;
    unsigned int m_ofOffset;

};

struct CClientSocket{};
