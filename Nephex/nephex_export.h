#ifndef NEPHEX_EXPORT_H_INCLUDED
#define NEPHEX_EXPORT_H_INCLUDED

#include "dasio/socket.h"
#include "dasio/client.h"
#include "dasio/mlf_packet_logger.h"

using namespace DAS_IO;

class APid
{
  public:
    APid();
    uint16_t get_size(int APID);
    bool OK_to_transmit(int APID);
    bool not_reported(int APID);
    static APid *APid_defs;
  protected:
    void def(const char *mnc, int APID, uint16_t size, float per, uint16_t dec);
    static const int MAX_APID = 151;
    struct {
      const char *mnc;
      uint16_t size;
      float period;
      uint16_t decimation_rate;
      uint16_t n_to_skip;
      uint16_t reported;
    } defs[MAX_APID+1];
};

class nephex_tcp_export : public Client {
  public:
    nephex_tcp_export(const char *iname);
    /**
     * Forwards packet to tm_ip_export.
     * @param hdr pointer to validated serio_pkt_hdr
     */
    void forward_packet(const uint8_t *hdr, uint16_t len);
    inline bool CTS() { return outstanding_bytes < 2000 && obuf_empty(); }
  protected:
    bool app_input() override;
    /**
     * Records the number of bytes written to the socket that
     * have not yet been acknowledged. It's OK to get ahead
     * a bit, or we'll waste time waiting for the round trip
     */
    uint32_t outstanding_bytes;
    static const int IBUFSZ = 4096;
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
    bool protocol_timeout() override;
    bool process_eof() override;
    uint32_t block_count;
    nephex_tcp_export *exp;
    static const int RCVR_BUFSIZE = 50000;
};

#endif
