#ifndef DPOPS_INT_H_INCLUDED
#define DPOPS_INT_H_INCLUDED

#include <math.h>
#include "POPS.h"

#ifdef __cplusplus
  extern "C" {
#endif

extern void pkts_init_options(int argc, char **argv);

#ifdef __cplusplus
  };
#endif

#ifdef __cplusplus

#include "dasio/interface.h"

class UserPkts_UDP : public DAS_IO::Interface {
  public:
    UserPkts_UDP(int udp_port);
    bool protocol_input();
    bool process_eof();
  protected:
    //bool tm_sync();
  private:
    void Bind(int port);
    int fillbuf();
    int not_KW(char *KWbuf);
    int udp_port;
};

class POPS_Cmd : public DAS_IO::Cmd_reader {
  public:
    inline POPS_Cmd() : DAS_IO::Cmd_reader("POPS", 80, "POPS") {}
    /**
     * Handles single-character commands:
     *   S: Send shutdown code '8' over UDP to POPS instrument
     *   Q: Return true to shutdown the driver
     * @return true on 'Q' or error
     */
    bool app_input();
  private:
    void send_shutdown();
};

class POPS_client : public DAS_IO::Client {
  public:
    POPS_client();
    bool app_connected();
    bool app_input();
    bool forward(const uint8_t *cmd);
    inline bool forward(const char *cmd) {
      return forward((const uint8_t *)cmd);
    }
    bool app_process_eof();
    bool protocol_timeout();
    static POPS_client *instance;
  protected:
    bool tm_sync();
    bool connect_failed();
  private:
    uint8_t srvr_Stale;
};

#endif // __cplusplus
#endif // DPOPS_INT_H_INCLUDED
