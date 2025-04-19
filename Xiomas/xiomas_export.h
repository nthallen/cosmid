#ifndef XIOMAS_EXPORT_H_INCLUDED
#define XIOMAS_EXPORT_H_INCLUDED
#include "dasio/loop.h"
#include "dasio/socket.h"
#include "dasio/client.h"
#include "dasio/mlf_packet_logger.h"
#include "XioHarvGatewayPacket.h"
#include "nl_assert.h"

using namespace DAS_IO;


#define MAX_SPKT_LENGTH 213183
#define MAX_SPKT_DATA_PER_SERIO_PKT 956

class xiomas_tcp_rcvr;

/**
 * One-way dump of serio_pkt data to tm_ip_export for
 * incorporation into satellite or radio transmission
 * with telemetry.
 */
class xiomas_tcp_export : public Client
{
  public:
    xiomas_tcp_export(const char *iname);
    void forward_packet(const uint8_t *bfr, int n_bytes);
    static const int RCVR_BUFSIZE = 1000;
    /**
     * @returns number of bytes of data that can be added to the buffer
     */
    uint32_t circ_space();
    void request_ack(xiomas_tcp_rcvr *client);
  protected:
    /**
     * Receives acknowledges from tm_ip_export indicating how many
     * bytes have been processed.
     * ACK \d+\n
     * May also receive command packets
     */
    bool app_input() override;
    /**
     * Write data from circular buffer to the socket if we're not
     * too far ahead.
     */
    void process_queue();
    
    /**
     * @returns number of bytes of data currently in the buffer
     */
    uint32_t circ_length();

    /**
     * @param data points to data to add to the buffer
     * @param n_bytes how many bytes to add
     */
    void circ_commit(const void *data, uint32_t n_bytes);
    /**
     * Circular buffer
     */
    uint8_t *circ;
    uint32_t circ_size, circ_head, circ_tail;
    static const int CIRC_BUFFER_SIZE = 270000;
    
    /**
     * @returns the number of contiguous bytes at the head of
     * the circular buffer.
     */
    inline uint32_t circ_head_bytes()
    {
      if (circ_head > circ_size) return 0;
      return circ_tail > circ_head ?
              circ_head-circ_tail :
              circ_size - circ_head;
    }

    /**
     * Removes n_bytes from the circular buffer. n_bytes
     * must not exceed circ_head_bytes(), i.e. it must
     * address a contiguous span within the circular
     * buffer. Since this is done after transmitting,
     * the outstanding_bytes count will be updated here
     * as well.
     * @param n_bytes The number of bytes to remove.
     */
    inline void circ_consume(uint32_t n_bytes)
    {
      nl_assert(n_bytes <= circ_head_bytes());
      outstanding_bytes += n_bytes;
      circ_head += n_bytes;
      if (circ_head == circ_size)
        circ_head = 0;
      if (circ_head == circ_tail)
        circ_head = (circ_size+1); // empty
    }

    void circ_transmit(uint32_t n_bytes);

    /**
     * Records the number of bytes written to the socket that
     * have not yet been acknowledged. It's OK to get ahead
     * a bit, or we'll waste time waiting for the round trip
     */
    uint32_t outstanding_bytes;

    xiomas_tcp_rcvr *client;

    /**
     * Includes a HARD CODED PROTOCOL value of 2000 bytes, shared with
     * tm_ip_export ipx_client::tcp_txfr_confirmed
     */
    inline bool CTS() { return outstanding_bytes < 2000 && obuf_empty(); }
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
    ~xiomas_udp_export();
    void forward_packet(const unsigned char *bfr, int n_bytes);
    bool app_input() override;
    static const int RCVR_BUFSIZE = 50;
  protected:
    bool ack_pending;
    int packets_dropped;
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
    /**
     * Called from xiomas_tcp_export::process_queue() when the
     * output queue is empty. Should send a CTS message. Or
     * maybe set a timeout to send a message.
     */
    void ack();
    inline bool CTS() { return obuf_empty(); }
    static const char *mlf_base;
    static const char *mlf_config;
  protected:
    bool connected() override;
    bool protocol_input() override;
    bool protocol_timeout() override;
    uint32_t get_txmit_size();
    bool sendFlag(uint8_t flag);
    inline bool sendCTS() { return sendFlag(xhfCTS); }
    inline bool sendNCTS() { return sendFlag(xhfNCTS); }
    bool transmitting_current_packet;
    enum state_t { xtr_idle, xtr_mid_pkt, xtr_lost } state;
    uint32_t bytes_remaining_in_packet;
    uint32_t bytes_discarding;
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
 * Separate outbound UDP socket back to the
 * instrument to avoid needing to override fillbuf
 * to use recvfrom()
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
class xiomas_udp_rcvr : public Socket, mlf_packet_logger
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
    xiomas_udp_export *exp;
    xiomas_udp_txmtr *xutr;
    uint32_t block_count;
    uint8_t pkts_recd;
    bool connect_requested;
    static const int RCVR_BUFSIZE = 5000; // check this
};

#endif
