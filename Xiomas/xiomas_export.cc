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

/****** xiomas_tcp_export ******/

xiomas_tcp_export::xiomas_tcp_export(const char *iname)
  : Client("xtx", RCVR_BUFSIZE, 0, "ipx", "tcp"),
    circ((uint8_t*)new_memory(CIRC_BUFFER_SIZE)),
    circ_size(CIRC_BUFFER_SIZE),
    circ_head(CIRC_BUFFER_SIZE+1),
    circ_tail(0),
    outstanding_bytes(0)
{
  set_obufsize(2000);
}

void xiomas_tcp_export::forward_packet(const uint8_t *bfr, int n_bytes)
{
  serio_pkt_hdr hdr;
  
  hdr.LRC = 0;
  hdr.type = pkt_type_SID;
  hdr.length = n_bytes;
  { unsigned CRC = crc16modbus_word(0,0,0);
    CRC = crc16modbus_word(CRC, bfr, n_bytes);
    hdr.CRC = CRC;
    uint8_t *hdrp = (uint8_t *)&hdr;
    for (uint32_t i = 1; i < sizeof(hdr); ++i) {
      hdr.LRC += hdrp[i];
    }
    hdr.LRC = -hdr.LRC;
  }
  msg(MSG_DBG(1), "%s: Committing %d+%d=%d bytes for TCP", iname,
    sizeof(hdr), n_bytes, n_bytes+sizeof(hdr));
  circ_commit(&hdr, sizeof(hdr));
  circ_commit(bfr, n_bytes);
  process_queue();
}

bool xiomas_tcp_export::app_input()
{
  bool have_hdr;
  serio_pkt_type type;
  uint16_t length;
  uint8_t *payload;
  serio_ctrl_payload *sctrl;

  while (cp < nc)
  {
    if (not_serio_pkt(have_hdr, type, length, payload))
      return false;
    switch (type)
    {
      case pkt_type_CTRL:
        if (length != sizeof(serio_ctrl_payload))
        {
          report_err("%s: Invalid ctrl packet payload length: %d",
            iname, length);
          ++cp;
          continue;
        }
        sctrl = (serio_ctrl_payload*)payload;
        switch (sctrl->subtype)
        {
          case ctrl_subtype_ACK:
            if (sctrl->length > outstanding_bytes)
            {
              report_err("%s: ACK exceeds tx: %u", iname, sctrl->length);
              outstanding_bytes = 0;
            } else outstanding_bytes -= sctrl->length;
            process_queue();
            break;
          default:
            report_err("%s: Unsupported ctrl subtype %u", iname,
              sctrl->subtype);
            break;
        }
        report_ok(serio::pkt_hdr_size + length);
        break;
      default:
        report_err("%s: Unsupported packet type %u", iname, type);
    }
  }
  return false;
}

void xiomas_tcp_export::process_queue()
{
  // I'd like to transmit a packet at a time, but
  // I'm probably overthinking, since it's going
  // down in a stream. They have to come in as
  // complete packets, though.
  uint32_t len = circ_length();
  if (CTS() && len > 0)
  {
    if (len > MAX_SPKT_DATA_PER_SERIO_PKT + sizeof(serio_pkt_hdr))
      len = MAX_SPKT_DATA_PER_SERIO_PKT + sizeof(serio_pkt_hdr);
    circ_transmit(len);
  }
}

/**
 * @returns number of bytes of data that can be added to the buffer
 */
uint32_t xiomas_tcp_export::circ_space()
{
  return circ_size - circ_length();
}

/**
 * @returns number of bytes of data currently in the buffer
 */
uint32_t xiomas_tcp_export::circ_length()
{
  return circ_head > circ_size ? 0 :
    (circ_tail > circ_head ? circ_tail-circ_head :
     circ_size-circ_head+circ_tail);
}

void xiomas_tcp_export::circ_commit(const void *data, uint32_t n_bytes)
{
  if (n_bytes > circ_space())
  {
    msg(MSG_ERROR, "%s: circular buffer overflow", iname);
    n_bytes = circ_space();
  }
  if (n_bytes)
  {
    // We know we are not full
    if (circ_head > circ_size) {
      circ_head = 0;
      circ_tail = 0;
    }
    uint32_t tail_space =
      circ_tail >= circ_head ?
        circ_size-circ_tail :
        circ_head-circ_tail;
    if (n_bytes < tail_space)
      tail_space = n_bytes;
    memcpy(&circ[circ_tail], data, tail_space);
    circ_tail += tail_space;
    nl_assert(circ_tail <= circ_size);
    if (circ_tail == circ_size)
      circ_tail = 0;
    n_bytes -= tail_space;
    data = tail_space + (uint8_t*)data; // don't to arithmetic on void*
    if (n_bytes)
    {
      nl_assert(circ_tail == 0 && n_bytes <= circ_head);
      memcpy(&circ[circ_tail], data, n_bytes);
      circ_tail += tail_space;
    }
  }
}

void xiomas_tcp_export::circ_transmit(uint32_t n_bytes)
{
  while (n_bytes)
  {
    uint32_t txmit_bytes = circ_head_bytes();
    nl_assert(txmit_bytes > 0);
    if (n_bytes < txmit_bytes)
      txmit_bytes = n_bytes;
    iwrite((char*)&circ[circ_head], txmit_bytes);
    n_bytes -= txmit_bytes;
    circ_consume(txmit_bytes);
  }
}

/****** xiomas_udp_export ******/

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
  msg(MSG_DBG(1), "%s: forward_packet(%d,%d)", iname, io[0].iov_len, io[1].iov_len);
  iwritev(io,2);
}


/****** xiomas_tcp_rcvr ******/

const char *xiomas_tcp_rcvr::mlf_base = "XioTCP";
const char *xiomas_tcp_rcvr::mlf_config;

xiomas_tcp_rcvr::xiomas_tcp_rcvr(Socket *orig, const char *iname, int fd,
        xiomas_tcp_export *exp)
    : Socket(orig, iname, RCVR_BUFSIZE, fd),
      mlf_packet_logger(iname, mlf_base, mlf_config),
      state(xtr_lost),
      bytes_remaining_in_packet(0),
      bytes_discarding(0),
      block_count(0),
      exp(exp)
{
  // set_obufsize(5*1024); // I don't think I need an obuf here
  // Should at least try to validate the remote IP address
}

bool xiomas_tcp_rcvr::connected()
{
  msg(MSG, "%s: Connection established from Xiomas", iname);
  return false;
}

uint32_t xiomas_tcp_rcvr::get_txmit_size()
{
  uint32_t to_send =
    (bytes_remaining_in_packet >= MAX_SPKT_DATA_PER_SERIO_PKT) ?
    MAX_SPKT_DATA_PER_SERIO_PKT : bytes_remaining_in_packet;
  return (nc-cp >= to_send) ? to_send : 0;
}

bool xiomas_tcp_rcvr::protocol_input()
{
  unsigned cp0, nc0;
  uint32_t txmit_size;
  XioHarvardHeader *hdr;

  while (cp < nc)
  {
    switch (state)
    {
      case xtr_lost:
        cp0 = cp;
        nc0 = nc;
        if (not_found('$', false))
        {
          bytes_discarding += nc0-cp0;
        } else {
          --cp;
          bytes_discarding += cp-cp0;
          state = xtr_idle;
        }
        continue;
      case xtr_idle:
        // msg(MSG_DBG(0), "%s: Incoming packet len %d", iname, nc-cp);
        // Validate incoming packet
        // Packetize into serio_pkts
        // Log packet with a timestamp packet
        // Forward packet to tm_ip_export
        if (nc-cp < sizeof(XioHarvardHeader))
        {
          consume(cp);
          return false;
        }
        hdr = (XioHarvardHeader*)&buf[cp];
        // xhgValidateHeader will check that the header matches the
        // length given here (nc), but in a streaming input, there
        // could be more or less data in the input buffer, so we
        // should use the header's length.
        bytes_remaining_in_packet = xhgGetPacketLength(hdr);
        if (!xhgValidateHeader(hdr, bytes_remaining_in_packet))
        {
          // report_err("%s: Bad packet, searching", iname);
          ++cp;
          state = xtr_lost;
          continue;
        }
        if (bytes_remaining_in_packet > MAX_SPKT_LENGTH+1000)
        {
          xhgShowHeader(hdr);
          report_err("%s: Oversized packet, assuming bad", iname);
          ++cp;
          state = xtr_lost;
          continue;
        }
        txmit_size = get_txmit_size();
        if (!txmit_size)
          return false; // Don't send short packets
        
        // Now we are committed to logging and/or transmitting something
        if (bytes_discarding)
        {
          report_err("%s: Discarded %u bytes before valid packet",
                  iname, bytes_discarding);
          bytes_discarding = 0;
        }
        if (hdr->m_uSource != xhsrcOPU /* xhsrcScanner */ )
        {
          report_err("%s: Unexpected packet source %d", iname, hdr->m_uSource);
        }

        if ((hdr->m_uFlags & xhfRTS) && CTS())
        {
          sendCTS();
        }
        switch (hdr->m_uPacketID)
        {
          case xhpidFireLayer:	// fire layer imagery frame
          case xhpidMWBand:     // MW IR band imagery frame
          case xhpidLWBand:     // LW IR band imagery frame
            break;
          default:
            msg(MSG_WARN, "%s: Unexpected packet ID %d", iname, hdr->m_uPacketID);
        }

        msg(MSG_DBG(1), "%s: Rec'd SPkt of %u bytes", iname, bytes_remaining_in_packet);
        // setup the packet
        state = xtr_mid_pkt;
        // Figure out the total size after packetization
        {
          uint32_t n_pkts =
            (bytes_remaining_in_packet+MAX_SPKT_DATA_PER_SERIO_PKT-1) /
              MAX_SPKT_DATA_PER_SERIO_PKT;
          uint32_t total_pkts_size =
            bytes_remaining_in_packet + n_pkts * sizeof(serio_pkt_hdr);
          transmitting_current_packet = total_pkts_size <= exp->circ_space();
        }

        log_packet(buf+cp, txmit_size,
          xhgGetPacketLength(hdr) > 2000 ? log_newfile : log_default);
        break;
      case xtr_mid_pkt:
        txmit_size = get_txmit_size();
        if (txmit_size)
          log_packet(buf+cp, txmit_size, log_curfile);
        break;
    }
    // We should only reach here if we have set txmit_size
    if (txmit_size)
    {
      if (transmitting_current_packet)
        exp->forward_packet(buf+cp, txmit_size);
      cp += txmit_size;
      bytes_remaining_in_packet -= txmit_size;
    }
  }

  report_ok(cp);
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

/****** xiomas_tcp_svc ******/

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

/********* xiomas_udp_txmtr ***********
 * Sends UDP Xiomas packets to Xiomas *
 **************************************/

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

/****** xiomas_udp_rcvr ******/

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
  if (hdr->m_uSource != xhsrcScanner )
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
      msg(MSG_DBG(2), "%s: Sending CTS", iname);
      break;
    case xhfNCTS:
      msg(MSG_DBG(2), "%s: Sending NCTS", iname);
      break;
    default:
      msg(MSG_DBG(2), "%s: Sending flags 0x%02X", iname, flag);
      break;
  }
  xhgShowHeader(&hdr);
  return xutr->send((char *)&hdr, sizeof(hdr));
}

/****** main() ******/

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
