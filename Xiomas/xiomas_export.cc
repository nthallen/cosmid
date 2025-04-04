#include "xiomas_export.h"
#include "dasio/appid.h"
#include "dasio/serio_pkt.h"
#include "dasio/quit.h"
#include "crc16modbus.h"
#include "XioHarvGatewayPacket.h"
#include "nl.h"
#include "nl_assert.h"
#include "oui.h"

using namespace DAS_IO;

xiomas_tcp_export::xiomas_tcp_export(const char *iname)
  : Client("xtx", RCVR_BUFSIZE, 0, "ipx", "tcp")
{
  set_obufsize(5*1024);
}

xiomas_udp_export::xiomas_udp_export(const char *iname)
  : Client("xux", RCVR_BUFSIZE, 0, "ipx", "udp"),
    ack_pending(false),
    packets_dropped(0)
{
  set_obufsize(5*1024);
}

xiomas_udp_export::~xiomas_udp_export()
{
  if (packets_dropped)
    msg(MSG, "%s: %d packets dropped", iname, packets_dropped);
}

bool xiomas_udp_export::app_input()
{
  while (cp < nc) {
    if (not_str("OK\n")) {
      if (cp >= nc)
        report_err("%s: Unexpected incomplete input", iname);
      consume(nc);
      ack_pending = false;
      return false;
    }
    ack_pending = false;
  }
  consume(nc);
  return false;
}

void xiomas_udp_export::forward_packet(const unsigned char *bfr, int n_bytes)
{
  if (ack_pending || !obuf_empty()) {
    ++packets_dropped;
    return;
  }
  ack_pending = true;
  struct iovec io[2];
  serio_pkt_hdr hdr;
  
  hdr.LRC = 0;
  hdr.type = pkt_type_XIO;
  hdr.length = n_bytes;
  io[0].iov_len = sizeof(serio_pkt_hdr);
  io[0].iov_base = &hdr;
  io[1].iov_len = hdr.length;
  io[1].iov_base = (void *)bfr;
  // Calculate the CRC of io[1]
  { unsigned CRC = crc16modbus_word(0,0,0);
    CRC = crc16modbus_word(CRC, io[1].iov_base, io[1].iov_len);
    hdr.CRC = CRC;
    uint8_t *hdrp = (uint8_t *)io[0].iov_base;
    for (uint32_t i = 1; i < io[0].iov_len; ++i) {
      hdr.LRC += hdrp[i];
    }
    hdr.LRC = -hdr.LRC;
  }
  iwritev(io,2);
}

const char *xiomas_tcp_rcvr::mlf_base = "XioTCP";
const char *xiomas_tcp_rcvr::mlf_config;

xiomas_tcp_rcvr::xiomas_tcp_rcvr(Socket *orig, const char *iname, int fd,
        xiomas_tcp_export *exp)
    : Socket(orig, iname, RCVR_BUFSIZE, fd),
      mlf_packet_logger(iname, mlf_base, mlf_config),
      block_count(0),
      exp(exp)
{
  set_obufsize(5*1024);
  // Should at least try to validate the remote IP address
}

bool xiomas_tcp_rcvr::connected()
{
  msg(MSG, "%s: Connection established from Xiomas", iname);
  return false;
}

bool xiomas_tcp_rcvr::protocol_input()
{
  msg(MSG, "%s: Incoming TCP packet len %d", iname, nc);
  // Validate incoming packet
  // Packetize into serio_pkts
  // Log packet with a timestamp packet
  // Forward packet to tm_ip_export
  report_ok(nc);
  return false;
}

bool xiomas_tcp_rcvr::sendFlag(uint8_t flag)
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

xiomas_tcp_svc::xiomas_tcp_svc(const char *iname, xiomas_tcp_export *exp)
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

bool xiomas_udp_txmtr::CTS()
{
  if (socket_state == Socket_disconnected) {
    msg(MSG, "%s: Delayed connect", iname);
    connect();
    return false;
  } else if (socket_state != Socket_connected) {
    msg(MSG_WARN, "%s: Socket not connected", iname);
    return false;
  }
  return obuf_empty();
}

bool xiomas_udp_txmtr::send(const char *p, unsigned nc)
{
  return CTS() && iwrite(p, nc);
}

const char *xiomas_udp_rcvr::mlf_base = "XioUDP";
const char *xiomas_udp_rcvr::mlf_config;

xiomas_udp_rcvr::xiomas_udp_rcvr(const char *iname, xiomas_udp_export *exp,
        xiomas_udp_txmtr *xutr)
    : Socket(iname, "xurx", "xurx", RCVR_BUFSIZE, UDP_READ),
      mlf_packet_logger(iname, mlf_base, mlf_config),
      exp(exp),
      xutr(xutr),
      block_count(0),
      pkts_recd(0),
      connect_requested(false)
{
  mlf = mlf_init(3, 60, 1, mlf_base, "dat", mlf_config);
  // Validate incoming packet
  // Packetize into serio_pkts
  // Log packet with a timestamp packet
  // Forward packet to tm_ip_export
}

bool xiomas_udp_rcvr::protocol_input()
{
  msg(MSG_DBG(1), "%s: Incoming UDP packet len %d", iname, nc);
  if (!connect_requested) {
    CTS();
    connect_requested = true;
  }
  if (nc < sizeof(XioHarvardHeader))
  {
    report_err("%s: Short UDP packet header received", iname);
    consume(nc);
    return false;
  }
  XioHarvardHeader *hdr = (XioHarvardHeader*)buf;
  // xhgValidateHeader will check that the header matches the
  // length given here (nc), but in a streaming input, there
  // could be more or less data in the input buffer, so we
  // should use the header's length.
  if (!xhgValidateHeader(hdr, nc))
  {
    report_err("%s: Bad packet", iname);
    consume(nc);
    return false;
  }
  xhgShowHeader(hdr);
  if (hdr->m_uSource != xhsrcScanner)
  {
    report_err("%s: Unexpected packet source %d", iname, hdr->m_uSource);
    consume(nc);
    return false;
  }

  if ((hdr->m_uFlags & xhfRTS) && CTS())
  {
    if (pkts_recd > 0)
    {
      --pkts_recd;
    }
    else
    {
      sendCTS();
    }
  }

  switch (hdr->m_uPacketID)
  {
    case xhpidGWControl:
      break;
    case xhpidXioStatus:
      if (++pkts_recd > 10)
      {
        sendNCTS();
      }
      log_packet(buf, nc);
      exp->forward_packet(buf, nc);
      break;
    default:
      msg(MSG_WARN, "%s: Unexpected packet ID %d", iname, hdr->m_uPacketID);
  }

  report_ok(nc);
  return false;
}

bool xiomas_udp_rcvr::sendFlag(uint8_t flag)
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
  return xutr->send((char *)&hdr, sizeof(hdr));
}

int main(int argc, char **argv)
{
  oui_init_options(argc, argv);
  {
    Loop ELoop;
    
    AppID.report_startup();
    
    xiomas_tcp_export *XTEXP = new xiomas_tcp_export("XTEXP");
    XTEXP->connect();
    ELoop.add_child(XTEXP);
    
    xiomas_udp_export *XUEXP = new xiomas_udp_export("XUEXP");
    XUEXP->connect();
    ELoop.add_child(XUEXP);

    /* TCP listener on xiomas service: xiomas_tcp_svc */
    xiomas_tcp_svc *XTCP = new xiomas_tcp_svc("XTCP", XTEXP);
    XTCP->connect();
    ELoop.add_child(XTCP);
    
    /* UDP write on xiomas service */
    xiomas_udp_txmtr *XUTX = new xiomas_udp_txmtr("XUTX");
    XUTX->connect();
    ELoop.add_child(XUTX);

    /* UDP read on xiomas service */
    xiomas_udp_rcvr *XURX = new xiomas_udp_rcvr("XURX", XUEXP, XUTX);
    XURX->connect();
    ELoop.add_child(XURX);
    
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
