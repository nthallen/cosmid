#ifndef NEPHEX_EXPORT_H_INCLUDED
#define NEPHEX_EXPORT_H_INCLUDED

#include "dasio/socket.h"

class nephex_tcp_export : public Client {
  public:
    nephex_tcp_export(const char *iname);
};

/**
 * TCP client connection for streaming data from external
 * instrument. There should probably be only one allowed
 * per port.
 */
class nephex_tcp_rcvr : public Socket, mlf_packet_logger
{
  public:
    nephex_tcp_rcvr(nephex_tcp_export *exp);
    static const char *mlf_base;
    static const char *mlf_config;
  protected:
    bool connected() override;
    bool protocol_input() override;
    bool sendFlag(uint8_t flag);
    inline bool sendCTS() { return sendFlag(xhfCTS); }
    inline bool sendNCTS() { return sendFlag(xhfNCTS); }
    uint32_t block_count;
    nephex_tcp_export *exp;
    static const int RCVR_BUFSIZE = 50000;
};

#endif
