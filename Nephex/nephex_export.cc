#include "dasio/appid.h"
#include "dasio/quit.h"
#include "nephex_export.h"
#include "nl.h"
#include "nl_assert.h"
#include "oui.h"
#include "crc16modbus.h"

APid *APid::APid_defs;

APid::APid()
{
  nl_assert(APid_defs == 0);

  for (int i = 0; i <= MAX_APID; ++i)
  {
    def("UNDEF", i, 0, 0, 0);
  }
  def("ACCEL", 1, 28, 0.1, 20);
  def("SCI", 2, 36, 1, 10);
  def("STATUS", 3, 88, 1, 20);
  def("HSK", 4, 20, 5, 20);
  def("HTR", 5, 20, 5, 4);
  def("CFG", 6, 96, 0, 0);
  def("VER", 7, 36, 0, 0);
  def("TASK", 8, 80, 5, 20);
  def("NEPH_VER", 132, 36, 0, 0);
  def("NEPH_STATUS", 133, 12, 1, 20);
  def("NEPH_AD4684_REG", 134, 4, 0, 0);
  def("NEPH_AD4684_ADC", 135, 8, 1, 20);
  def("NEPH_AD7490", 136, 32, 1, 20);
  def("NEPH_DAC", 137, 12, 1, 20);
  def("NEPH_GPIO", 138, 8, 0, 0);
  def("NEPH_MEM_READ", 139, 8, 0, 0);
  def("NEPH_MEM_DUMP", 140, 208, 0, 0);
  def("NEPH_TEC_PID", 141, 72, 0, 0);
  def("NEPH_TEC_PID_PARAM", 142, 24, 0, 0);
  def("NEPH_LASER_POWER", 143, 4, 5, 1);
  def("NEPH_LASER_PWR_CTRL", 144, 12, 0, 0);
  def("NEPH_STATS_1", 145, 44, 0, 0);
  def("NEPH_STATS_2", 146, 44, 0, 0);
  def("NEPH_AC_1", 147, 80, 0, 0);
  def("NEPH_AC_2", 148, 80, 0, 0);
  def("NEPH_PULSE", 149, 208, 0.3571, 4);
  def("NEPH_DARK_1", 150, 204, 5, 4);
  def("NEPH_DARK_2", 151, 204, 5, 4);
}

void APid::def(const char *mnc, int APID, uint16_t size, float per, uint16_t dec)
{
  nl_assert(APID <= MAX_APID);
  defs[APID].mnc = mnc;
  defs[APID].size = size;
  defs[APID].period = per;
  defs[APID].decimation_rate = dec;
  defs[APID].n_to_skip = dec ? 1 : 0;
  defs[APID].reported = 0;
}

uint16_t APid::get_size(int APID)
{
  if (APID > MAX_APID)
    return 0;
  return defs[APID].size;
}

bool APid::OK_to_transmit(int APID)
{
  if (APID > MAX_APID)
    return false;
  if (defs[APID].size == 0 || defs[APID].decimation_rate == 0)
    return false;
  if (--defs[APID].n_to_skip == 0)
  {
    defs[APID].n_to_skip = defs[APID].decimation_rate;
    return true;
  }
  return false;
}

bool APid::not_reported(int APID)
{
  if (defs[APID].reported) return false;
  defs[APID].reported = 1;
  return true;
}

nephex_tcp_export::nephex_tcp_export(const char *iname)
    : Client(iname, IBUFSZ, 0, "ipx", "tcp"),
      outstanding_bytes(0)
{
  set_obufsize(5*1024);
};

void nephex_tcp_export::forward_packet(const uint8_t *bfr, uint16_t n_bytes)
{
  nl_assert(CTS());
  struct iovec io[2];
  serio_pkt_hdr hdr;

  hdr.LRC = 0;
  hdr.type = pkt_type_NPH;
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
  io[0].iov_len = serio::pkt_hdr_size;
  io[0].iov_base = &hdr;
  io[1].iov_len = n_bytes;
  io[1].iov_base = (void *)bfr;
  iwritev(io, 2);
  outstanding_bytes += serio::pkt_hdr_size + n_bytes;
}

bool nephex_tcp_export::app_input()
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
              report_err("%s: ACK (%u) exceeds outstanding (%u)", iname,
                  sctrl->length, outstanding_bytes);
              outstanding_bytes = 0;
            } else outstanding_bytes -= sctrl->length;
            msg(MSG_DBG(1), "%s: Rec'd ACK %u outstanding: %u", iname,
              sctrl->length, outstanding_bytes);
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

const char *nephex_tcp_rcvr::mlf_base = "Nephex";
const char *nephex_tcp_rcvr::mlf_config;

nephex_tcp_rcvr::nephex_tcp_rcvr(const char *iname,
          nephex_tcp_export *exp)
    : Socket(iname, "Nephex", "ntcp", RCVR_BUFSIZE),
      mlf_packet_logger(iname, mlf_base, mlf_config),
      exp(exp)
{
  // reduce the maximum delay for testing
  set_retries(-1, 5, 10);
  set_bytes_per_file(32*1024);
}

bool nephex_tcp_rcvr::connected()
{
  msg(MSG, "%s: Connection established to Nephex", iname);
  return false;
}

bool nephex_tcp_rcvr::protocol_input()
{
  while (cp < nc)
  {
    if (not_found('\xaa'))
      return false;
    if (cp < nc && buf[cp] != (uint8_t)'\xaa') {
      continue;
    }
    cp++; // pointing past sync word
    if (nc < cp+6) // enough to ready the Primary Hdr
      return false;
    uint16_t APword = buf[cp] + (((uint16_t)buf[cp+1])<<8);
    uint16_t APid = APword & 0x3FF;
    cp += 4;
    uint16_t Len = buf[cp] + (((uint16_t)buf[cp+1])<<8);
    cp += 2;
    if (nc < cp+Len+1) // don't have the whole packet
      return false;
    // Check the CRC
    // Get the time
    // uint32_t pkt_secs =
    //   ((uint32_t)buf[cp]) +
    //   (((uint32_t)buf[cp+1])<<8) +
    //   (((uint32_t)buf[cp+2])<<16) +
    //   (((uint32_t)buf[cp+3])<<24);
    // uint16_t pkt_msecs =
    //   ((uint16_t)buf[cp+4]) +
    //   (((uint16_t)buf[cp+5])<<8);
    // Check the size
    uint16_t apid_len = APid::APid_defs->get_size(APid);
    if (apid_len+6 != Len && APid::APid_defs->not_reported(APid))
      report_err("%s: Incorrect packet length APID:%u expected:%u Len:%u",
        iname, APid, apid_len+6, Len);

    log_packet(&buf[cp-8], Len+9);
    if (exp->CTS() && APid::APid_defs->OK_to_transmit(APid))
    {
      // Forward the packet
      msg(MSG_DBG(1), "%s: Fwd APID:%u %u+%u=%u", iname, APid, Len+9,
          serio::pkt_hdr_size, serio::pkt_hdr_size+Len+9);
      exp->forward_packet(&buf[cp-8], Len+9);
      // Look at the next one
    }
    cp += Len + 1;
  }
  report_ok(cp);
  return false;
}

int main(int argc, char **argv)
{
  oui_init_options(argc, argv);
  {
    Loop ELoop;

    AppID.report_startup();

    APid::APid_defs = new APid();

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
