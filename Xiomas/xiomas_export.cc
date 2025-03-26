#include "xiomas_export.h"
#include "dasio/appid.h"
#include "nl.h"
#include "oui.h"

using namespace DAS_IO;

xiomas_export::xiomas_export(const char *iname)
  : Client("xexp", RCVR_BUFSIZE, 0, "tmipexp", 0)
{
}

xiomas_tcp_rcvr::xiomas_tcp_rcvr(Socket *orig, const char *iname, int fd,
        xiomas_export *exp)
    : Socket(orig, iname, RCVR_BUFSIZE, fd)
{
  // Should at least try to validate the remote IP address
}

bool xiomas_tcp_rcvr::protocol_input()
{
  // Validate incoming packet
  // Packetize into serio_pkts
  // Log packet with a timestamp packet
  // Forward packet to tm_ip_export
  return false;
}

xiomas_tcp_svc::xiomas_tcp_svc(const char *iname, xiomas_export *exp)
    : Socket(iname, "xtcp", Socket_TCP),
      exp(exp)
{
}

Socket *xiomas_tcp_svc::new_client(const char *iname, int fd)
{
  Socket *rv = new xiomas_tcp_rcvr(this, iname, fd, exp);
  if (ELoop) ELoop->add_child(rv);
  return rv;
}

xiomas_udp_txmtr::xiomas_udp_txmtr(const char *iname)
    : Socket(iname, "xutx", "xutx", RCVR_BUFSIZE, UDP_WRITE)
{
}

xiomas_udp_rcvr::xiomas_udp_rcvr(const char *iname, xiomas_export *exp,
        xiomas_udp_txmtr *xutr)
    : Socket(iname, "xurx", "xurx", RCVR_BUFSIZE, UDP_READ),
      exp(exp),
      xutr(xutr)
{
  // Validate incoming packet
  // Packetize into serio_pkts
  // Log packet with a timestamp packet
  // Forward packet to tm_ip_export
}

bool xiomas_udp_rcvr::protocol_input()
{
  return false;
}

int main(int argc, char **argv)
{
  oui_init_options(argc, argv);
  {
    Loop ELoop;
    
    AppID.report_startup();
    
    xiomas_export *XEXP = new xiomas_export("XEXP");
    XEXP->connect();
    ELoop.add_child(XEXP);

    /* TCP listener on xiomas service: xiomas_tcp_svc */
    xiomas_tcp_svc *XTCP = new xiomas_tcp_svc("XTCP", XEXP);
    XTCP->connect();
    ELoop.add_child(XTCP);
    
    xiomas_udp_txmtr *XUTX = new xiomas_udp_txmtr("XUTX");
    XUTX->connect();
    ELoop.add_child(XUTX);

    /* UDP RW on xiomas service */
    xiomas_udp_rcvr *XURX = new xiomas_udp_rcvr("XURX", XEXP, XUTX);
    XURX->connect();
    ELoop.add_child(XURX);
    
    /* Data forward to tm_ip_export */
    /* Quit service from srvr */
    /* TM status to collection */
    ELoop.event_loop();
    AppID.report_shutdown();
  }
}
