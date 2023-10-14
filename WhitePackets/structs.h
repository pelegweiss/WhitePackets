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
    BYTE * m_aRecvBuff;
    unsigned short m_uLength; //length with shitty bytes
    unsigned short m_uRawSeq;
    unsigned short m_uDatalen;
    unsigned int m_ofOffset;

};

struct CClientSocket{};
