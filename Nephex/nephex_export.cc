#include "dasio/appid.h"
#include "dasio/quit.h"
#include "nephex_export.h"
#include "nl.h"
#include "oui.h"

nephex_tcp_export::nephex_tcp_export(const char *iname)
    : Client(iname, BUFSZ, 0, "ipx", "tcp"),
      txfr_pending(false)
{
  set_obufsize(5*1024);
};

void nephex_tcp_export::forward_packet(const char *pkt)
{
  nl_assert(CTS());
}

const char *nephex_tcp_rcvr::mlf_base = "Nephex";
const char *nephex_tcp_rcvr::mlf_config;

nephex_tcp_rcvr::nephex_tcp_rcvr(const char *iname,
          nephex_tcp_export *exp)
    : Socket(iname, "Nephex", "ntcp", RCVR_BUFSIZE),
      mlf_packet_logger(iname, mlf_base, mlf_config),
      block_count(0),
      exp(exp)
{
  // reduce the maximum delay for testing
  set_retries(-1, 5, 10);
}

bool nephex_tcp_rcvr::connected()
{
  msg(MSG, "%s: Connection established to Nephex", iname);
  return false;
}

bool nephex_tcp_rcvr::protocol_input()
{
  msg(MSG, "%s: Incoming TCP packet nc %d", iname, nc);
  // Validate incoming packet
  // Packetize into serio_pkts
  // Log packet with a timestamp packet
  // Forward packets to tm_ip_export
  if (exp->CTS())
  {
    exp->forward_packet(buf);
  } else if (nc-cp < 1000)
  {
    flags &= ~Fl_Read;
  }
  return false;
}

int main(int argc, char **argv)
{
  oui_init_options(argc, argv);
  {
    Loop ELoop;
    
    AppID.report_startup();

    nephex_tcp_export *NTEXP = new nephex_tcp_export("NTEXP");
    NTEXP->connect();
    ELoop.add_child(NTEXP);

    /* TCP listener on nephex service: nephex_tcp_svc */
    nephex_tcp_rcvr *NTCP =
      new nephex_tcp_rcvr("NTCP", NTEXP);
    NTCP->connect();
    ELoop.add_child(NTCP);
    
    Quit *Q = new Quit();
    Q->connect();
    ELoop.add_child(Q);
    
    /* Data forward to tm_ip_export */
    /* Quit service from srvr */
    /* TM status to collection */
    ELoop.event_loop();
    AppID.report_shutdown();
  }
}
