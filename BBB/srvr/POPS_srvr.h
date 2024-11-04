#ifndef POPS_SRVR_H_INCLUDED
#define POPS_SRVR_H_INCLUDED
#include "dasio/server.h"
#include "oui.h"

using namespace DAS_IO;

typedef enum {POPS_init, POPS_idle, POPS_active, POPS_shutdown} POPS_status_t;

class Shutdown_UDP : public DAS_IO::Socket {
  public:
    Shutdown_UDP(const char *service);
    void send_shutdown();
    static Shutdown_UDP *SD;
  protected:
    bool connected() override;
};

class pops_socket : public DAS_IO::Serverside_client {
  public:
    inline pops_socket(Authenticator *Auth, const char *iname) :
      DAS_IO::Serverside_client(Auth, iname, 80) {}
  protected:
    bool protocol_input();
    bool send_status();
    /**
     * @return true if message is delivered
     */
    bool send_shutdown();
};

pops_socket *new_pops_socket(Authenticator *Auth, SubService *SS);

extern const char *pops_service;

#endif
