#include "dasio/quit.h"
#include "nephex_export.h"
#include "nl.h"
#include "oui.h"

nephex_tcp_export(const char *iname)
    : Client(iname, BUFSZ, 0, "ipx", "tcp")
{
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
  set_obufsize(5*1024);
  // Should at least try to validate the remote IP address
}

bool nephex_tcp_rcvr::connected()
{
  msg(MSG, "%s: Connection established to Nephex", iname);
  return false;
}

bool nephex_tcp_rcvr::protocol_input()
{
  msg(MSG, "%s: Incoming TCP packet len %d", iname, nc);
  // Validate incoming packet
  // Packetize into serio_pkts
  // Log packet with a timestamp packet
  // Forward packet to tm_ip_export
  report_ok(nc);
  return false;
}

bool nephex_tcp_rcvr::sendFlag(uint8_t flag)
{
  XioHarvardHeader hdr;
  xhgFillHeader(&hdr, xhpidGWControl, xhsrcGateway, flag, 0, ++block_count);
  switch (flag) {
    case xhfCTS:
      msg(MSG_DBG(1), "%s: Sending CTS", iname);
      break;
    case xhfNCTS:
      msg(MSG_DBG(1), "%s: Sending NCTS", iname);
      break;
    default:
      msg(MSG_DBG(1), "%s: Sending flags 0x%02X", iname, flag);
      break;
  }
  xhgShowHeader(&hdr);
  return iwrite((char *)&hdr, sizeof(hdr));
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
