#ifndef NEPHEX_EMULATOR_H_INCLUDED
#define NEPHEX_EMULATOR_H_INCLUDED
#include "dasio/interface.h"
#include "dasio/socket.h"

using namespace DAS_IO;

class nephex_tcp;

class NE_ingest : public Interface
{
  public:
    NE_ingest();
    /**
     * @return true if client is already set
     */
    bool set_client(nephex_tcp *client);
    void reset_client();
    static const char *nephex_input_file;
  protected:
    bool protocol_input() override;
    bool protocol_timeout() override;
    uint32_t last_secs, last_msecs;
    uint32_t packet_sn;
    nephex_tcp *client;
};

class nephex_tcp : public Socket
{
  public:
    nephex_tcp(Socket *orig, const char *iname, int fd, NE_ingest *NE);
    bool forward_packet(uint8_t *pkt, uint16_t len);
    inline bool CTS() { return obuf_empty(); }
  protected:
    ~nephex_tcp();
    bool connected() override;
    NE_ingest *NE;
    static const int TCP_BUFSIZE = 500; // Just small commands
};

/**
 * Server socket for TCP connection from external instrument.
 */
class nephex_tcp_svc : public Socket
{
  public:
    nephex_tcp_svc(const char *iname, NE_ingest *NE);
    Socket *new_client(const char *iname, int fd) override;
  protected:
    NE_ingest *NE;
};

#endif
