#ifndef NEPHEX_EXPORT_H_INCLUDED
#define NEPHEX_EXPORT_H_INCLUDED

#include "dasio/socket.h"
#include "dasio/client.h"
#include "mlf_packet_logger.h"

using namespace DAS_IO;

class nephex_tcp_export : public Client {
  public:
    nephex_tcp_export(const char *iname);
    /**
     * Forwards packet to tm_ip_export.
     * @param hdr pointer to validated serio_pkt_hdr
     */
    void forward_packet(uint8_t *hdr);
    inline bool CTS() { return !txfr_pending; }
  protected:
    bool txfr_pending;
    static const int BUFSZ = 4096;
};

/**
 * TCP client connection for streaming data from external
 * instrument. There should probably be only one allowed
 * per port.
 */
class nephex_tcp_rcvr : public Socket, mlf_packet_logger
{
  public:
    nephex_tcp_rcvr(const char *iname, nephex_tcp_export *exp);
    static const char *mlf_base;
    static const char *mlf_config;
  protected:
    bool connected() override;
    bool protocol_input() override;
    uint32_t block_count;
    nephex_tcp_export *exp;
    static const int RCVR_BUFSIZE = 50000;
};

#endif
