#ifndef XIOMAS_EXPORT_H_INCLUDED
#define XIOMAS_EXPORT_H_INCLUDED
#include "dasio/loop.h"
#include "dasio/socket.h"
#include "dasio/client.h"

using namespace DAS_IO;

/**
 * One-way dump of serio_pkt data to tm_ip_export for
 * incorporation into satellite or radio transmission
 * with telemetry.
 */
class xiomas_export : public Client
{
  public:
    xiomas_export(const char *iname);
    static const int RCVR_BUFSIZE = 50;
};

/**
 * TCP client connection for streaming data from external
 * instrument. There should probably be only one allowed
 * per port.
 */
class xiomas_tcp_rcvr : public Socket
{
  public:
    xiomas_tcp_rcvr(Socket *orig, const char *iname, int fd,
        xiomas_export *exp);
  protected:
    bool protocol_input() override;
    static const int RCVR_BUFSIZE = 50000;
};

/**
 * Server socket for TCP connection from external instrument.
 */
class xiomas_tcp_svc : public Socket
{
  public:
    xiomas_tcp_svc(const char *iname, xiomas_export *exp);
    Socket *new_client(const char *iname, int fd) override;
  protected:
    xiomas_export *exp;
};

/**
 * Separate outbound UDP socket to avoid needing
 * to override fillbuf to use recvfrom()
 */
class xiomas_udp_txmtr : public Socket
{
  public:
    xiomas_udp_txmtr(const char *iname);
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
    xiomas_udp_rcvr(const char *iname, xiomas_export *exp,
                    xiomas_udp_txmtr *xutr);
  protected:
    bool protocol_input() override;
    xiomas_export *exp;
    xiomas_udp_txmtr *xutr;
    static const int RCVR_BUFSIZE = 5000; // check this
};

#endif
