#ifndef XIOMAS_EXPORT_H_INCLUDED
#define XIOMAS_EXPORT_H_INCLUDED
#include "dasio/loop.h"
#include "dasio/socket.h"
#include "dasio/client.h"
#include "mlf.h"
#include "XioHarvGatewayPacket.h"

using namespace DAS_IO;

/**
 * One-way dump of serio_pkt data to tm_ip_export for
 * incorporation into satellite or radio transmission
 * with telemetry.
 */
class xiomas_tcp_export : public Client
{
  public:
    xiomas_tcp_export(const char *iname);
    static const int RCVR_BUFSIZE = 50;
};

/**
 * One-way dump of serio_pkt data to tm_ip_export for
 * incorporation into satellite or radio transmission
 * with telemetry. This particular Client is for packets
 * that arrived via UDP and that tm_ip_export should
 * transmit via UDP, but the actjual connection to
 * tm_ip_export is via TCP.
 */
class xiomas_udp_export : public Client
{
  public:
    xiomas_udp_export(const char *iname);
    void forward_packet(const unsigned char *bfr, int n_bytes);
    bool app_input() override;
    static const int RCVR_BUFSIZE = 50;
  protected:
    bool ack_pending;
    int packets_dropped;
};

class mlf_packet_logger {
  public:
    mlf_packet_logger(const char *iname,
      const char *mlf_base, const char *mlf_config);
    void log_packet(const unsigned char *bfr, uint16_t pkt_len);
    void next_file();
  protected:
    const char *miname;
    mlf_def_t *mlf;
    int ofd;
    int Bytes_in_File;
    const static int Bytes_per_File = 40960;
};

/**
 * TCP client connection for streaming data from external
 * instrument. There should probably be only one allowed
 * per port.
 */
class xiomas_tcp_rcvr : public Socket, mlf_packet_logger
{
  public:
    xiomas_tcp_rcvr(Socket *orig, const char *iname, int fd,
        xiomas_tcp_export *exp);
    static const char *mlf_base;
    static const char *mlf_config;
  protected:
    bool connected() override;
    bool protocol_input() override;
    bool sendFlag(uint8_t flag);
    inline bool sendCTS() { return sendFlag(xhfCTS); }
    inline bool sendNCTS() { return sendFlag(xhfNCTS); }
    uint32_t block_count;
    xiomas_tcp_export *exp;
    static const int RCVR_BUFSIZE = 50000;
};

/**
 * Server socket for TCP connection from external instrument.
 */
class xiomas_tcp_svc : public Socket
{
  public:
    xiomas_tcp_svc(const char *iname, xiomas_tcp_export *exp);
    Socket *new_client(const char *iname, int fd) override;
  protected:
    xiomas_tcp_export *exp;
};

/**
 * Separate outbound UDP socket to avoid needing
 * to override fillbuf to use recvfrom()
 */
class xiomas_udp_txmtr : public Socket
{
  public:
    xiomas_udp_txmtr(const char *iname);
    bool send(const char *p, unsigned nc);
    bool CTS();
  protected:
    static const int RCVR_BUFSIZE = 30;
};

/**
 * Receive control messages from external instrument.
 * Replies go out on xutr.
 */
class xiomas_udp_rcvr : public Socket
{
  public:
    xiomas_udp_rcvr(const char *iname, xiomas_udp_export *exp,
                    xiomas_udp_txmtr *xutr);
    /**
     * Invocation will trigger xutr->connect() if disconnected.
     * This allows us to delay connecting until we know the
     * network connection is alive (because we are receiving
     * data from xiomas)
     * @returns true if we can send UDP back to xiomas
     */
    inline bool CTS() { return xutr && xutr->CTS(); }
    static const char *mlf_base;
    static const char *mlf_config;
  protected:
    bool protocol_input() override;
    bool sendFlag(uint8_t flag);
    inline bool sendCTS() { return sendFlag(xhfCTS); }
    inline bool sendNCTS() { return sendFlag(xhfNCTS); }
    void log_packet(const unsigned char *bfr, uint16_t pkt_len);
    void next_file();
    xiomas_udp_export *exp;
    xiomas_udp_txmtr *xutr;
    uint32_t block_count;
    uint8_t pkts_recd;
    bool connect_requested;
    mlf_def_t *mlf;
    int ofd;
    int Bytes_in_File;
    static const int RCVR_BUFSIZE = 5000; // check this
    static const int Bytes_per_File = 40960;

};

#endif
