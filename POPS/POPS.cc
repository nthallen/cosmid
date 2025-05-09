#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "dasio/appid.h"
#include "dasio/msg.h"
#include "dasio/quit.h"
#include "dasio/tm_data_sndr.h"
#include "nl.h"
#include "nl_assert.h"
#include "oui.h"
#include "POPS_int.h"

using namespace DAS_IO;

POPS_t POPS;
const char *pops_service = "pops";

int main(int argc, char **argv) {
  oui_init_options(argc, argv);
  DAS_IO::Loop ELoop;
  const char *name = AppID.name;

  UserPkts_UDP *Pkts = new UserPkts_UDP(pops_service);
  Pkts->connect();
  ELoop.add_child(Pkts);
  
  // Shutdown_UDP *SD = new Shutdown_UDP(pops_service);

  POPS_Cmd *Q = new POPS_Cmd(name);
  Q->connect();
  ELoop.add_child(Q);

  DAS_IO::TM_data_sndr *TM =
    new DAS_IO::TM_data_sndr("TM", 0, name, &POPS, sizeof(POPS));
  TM->connect();
  ELoop.add_child(TM);
  POPS_client *clt = new POPS_client();
  clt->connect();
  ELoop.add_child(clt);
  msg(0, "Starting: V3.0");
  ELoop.event_loop();
  msg(0, "Terminating");
  ELoop.delete_children();
  ELoop.clear_delete_queue(true);
  return 0;
}

UserPkts_UDP::UserPkts_UDP(const char *service)
    : DAS_IO::Socket("UDPr", "UDPr", service, 512, UDP_READ)
{
  // Bind(udp_port);
  // flush_input();
  setenv("TZ", "UTC0", 1); // Force UTC for mktime()
  // flags |= gflag(0);
}

bool UserPkts_UDP::protocol_input() {
  // double Time;
  float    TimeSSM;
  uint8_t  IntStatus, DataStatus;
  uint32_t Part_Num;
  uint32_t HistSum;
  float    PartCon_num_cc;
  uint32_t Baseline;
  uint32_t BLTH;
  float    STD;
  float    MaxSTD;
  float    P_mbar;
  float    MSTemp;
  float    PumpLife;
  float    WidthSTD;
  float    WidthAvg;
  float    Flow;
  float    PumpFB;
  float    LDTemp;
  float    LaserFB;
  float    LD_Mon;
  float    AirTemp;
  float    BatV;
  float    Laser_Current;
  float    Flow_Set;
  uint32_t BL_Start;
  float    TH_Mult;
  uint16_t NBins;
  float    logmin;
  float    logmax;
  int32_t  Skip_Save;
  uint32_t MinPeakPts;
  uint32_t MaxPeakPts;
  int32_t  RawPts;
  uint32_t Bin01;
  uint32_t Bin02;
  uint32_t Bin03;
  uint32_t Bin04;
  uint32_t Bin05;
  uint32_t Bin06;
  uint32_t Bin07;
  uint32_t Bin08;
  uint32_t Bin09;
  uint32_t Bin10;
  uint32_t Bin11;
  uint32_t Bin12;
  uint32_t Bin13;
  uint32_t Bin14;
  uint32_t Bin15;
  uint32_t Bin16;
  int year, month, day, hour, minute, second;
  struct tm buft;
  le_time_t ltime;

  cp = 0;
  if (not_str("POPS,") ||
      not_found(',', true) || // SN
      not_found(',', true) || // Filename
      not_ndigits(4,year) || not_ndigits(2, month) || not_ndigits(2,day) ||
      not_str("T") || not_ndigits(2,hour) || not_ndigits(2,minute) ||
      not_ndigits(2,second) || not_str(",", 1) ||
      not_nfloat(&TimeSSM) || not_str(",", 1) ||
      not_uint8(IntStatus) || not_str(",", 1) ||
      not_uint8(DataStatus) || not_str(",", 1) ||
      not_uint32(Part_Num) || not_str(",", 1) ||
      not_uint32(HistSum) || not_str(",", 1) ||
      not_nfloat(&PartCon_num_cc) || not_str(",", 1) ||
      not_uint32(Baseline) || not_str(",", 1) ||
      not_uint32(BLTH) || not_str(",", 1) ||
      not_nfloat(&STD) || not_str(",", 1) ||
      not_nfloat(&MaxSTD) || not_str(",", 1) ||
      not_nfloat(&P_mbar) || not_str(",", 1) ||
      not_nfloat(&MSTemp) || not_str(",", 1) ||
      not_nfloat(&PumpLife) || not_str(",", 1) ||
      not_nfloat(&WidthSTD) || not_str(",", 1) ||
      not_nfloat(&WidthAvg) || not_str(",", 1) ||
      not_nfloat(&Flow) || not_str(",", 1) ||
      not_nfloat(&PumpFB) || not_str(",", 1) ||
      not_nfloat(&LDTemp) || not_str(",", 1) ||
      not_nfloat(&LaserFB) || not_str(",", 1) ||
      not_nfloat(&LD_Mon) || not_str(",", 1) ||
      not_nfloat(&AirTemp) || not_str(",", 1) ||
      not_nfloat(&BatV) || not_str(",", 1) ||
      not_nfloat(&Laser_Current) || not_str(",", 1) ||
      not_nfloat(&Flow_Set) || not_str(",", 1) ||
      not_uint32(BL_Start) || not_str(",", 1) ||
      not_nfloat(&TH_Mult) || not_str(",", 1) ||
      not_uint16(NBins) || not_str(",", 1) ||
      not_nfloat(&logmin) || not_str(",", 1) ||
      not_nfloat(&logmax) || not_str(",", 1) ||
      not_int32(Skip_Save) || not_str(",", 1) ||
      not_uint32(MinPeakPts) || not_str(",", 1) ||
      not_uint32(MaxPeakPts) || not_str(",", 1) ||
      not_int32(RawPts) || not_str(",", 1) ||
      not_uint32(Bin01) || not_str(",", 1) ||
      not_uint32(Bin02) || not_str(",", 1) ||
      not_uint32(Bin03) || not_str(",", 1) ||
      not_uint32(Bin04) || not_str(",", 1) ||
      not_uint32(Bin05) || not_str(",", 1) ||
      not_uint32(Bin06) || not_str(",", 1) ||
      not_uint32(Bin07) || not_str(",", 1) ||
      not_uint32(Bin08) || not_str(",", 1) ||
      not_uint32(Bin09) || not_str(",", 1) ||
      not_uint32(Bin10) || not_str(",", 1) ||
      not_uint32(Bin11) || not_str(",", 1) ||
      not_uint32(Bin12) || not_str(",", 1) ||
      not_uint32(Bin13) || not_str(",", 1) ||
      not_uint32(Bin14) || not_str(",", 1) ||
      not_uint32(Bin15) || not_str(",", 1) ||
      not_uint32(Bin16)) {
    if (cp < nc) {
      consume(nc);
    }
    return false;
  }
  buft.tm_year = year - 1900;
  buft.tm_mon = month - 1;
  buft.tm_mday = day;
  buft.tm_hour = hour;
  buft.tm_min = minute;
  buft.tm_sec = second;
  buft.tm_isdst = 0;
  ltime = mktime(&buft);
  if (ltime == (le_time_t)(-1)) {
    report_err("%s: mktime returned error", iname);
  } else {
    double fracsec = TimeSSM - floor(TimeSSM);
    POPS.Time = ltime + fracsec;
  }
  POPS.IntStatus = IntStatus;
  POPS.DataStatus = DataStatus;
  POPS.Part_Num = Part_Num;
  POPS.HistSum = HistSum;
  POPS.PartCon_num_cc = PartCon_num_cc;
  POPS.Baseline = Baseline;
  POPS.BLTH = BLTH;
  POPS.STD = STD;
  POPS.MaxSTD = MaxSTD;
  POPS.P_mbar = P_mbar;
  POPS.MSTemp = MSTemp;
  POPS.PumpLife = PumpLife;
  POPS.WidthSTD = WidthSTD;
  POPS.WidthAvg = WidthAvg;
  POPS.Flow = Flow;
  POPS.PumpFB = PumpFB;
  POPS.LDTemp = LDTemp;
  POPS.LaserFB = LaserFB;
  POPS.LD_Mon = LD_Mon;
  POPS.AirTemp = AirTemp;
  POPS.BatV = BatV;
  POPS.BL_Start = BL_Start;
  POPS.TH_Mult = TH_Mult;
  POPS.NBins = NBins;
  POPS.logmin = logmin;
  POPS.logmax = logmax;
  POPS.Skip_Save = Skip_Save;
  POPS.MinPeakPts = MinPeakPts;
  POPS.MaxPeakPts = MaxPeakPts;
  POPS.RawPts = RawPts;
  POPS.Bin01 = Bin01;
  POPS.Bin02 = Bin02;
  POPS.Bin03 = Bin03;
  POPS.Bin04 = Bin04;
  POPS.Bin05 = Bin05;
  POPS.Bin06 = Bin06;
  POPS.Bin07 = Bin07;
  POPS.Bin08 = Bin08;
  POPS.Bin09 = Bin09;
  POPS.Bin10 = Bin10;
  POPS.Bin11 = Bin11;
  POPS.Bin12 = Bin12;
  POPS.Bin13 = Bin13;
  POPS.Bin14 = Bin14;
  POPS.Bin15 = Bin15;
  POPS.Bin16 = Bin16;
  POPS.Stale = 0;
  report_ok(nc);
  return false;
}

bool UserPkts_UDP::process_eof() {
  msg(0, "%s: process_eof(): Resetting UDP port", iname);
  return reset();
}

#if 0
Shutdown_UDP::Shutdown_UDP(const char *service)
    : Socket("UDPw", "POPS", service, 0, UDP_WRITE)
{}

void Shutdown_UDP::send_shutdown()
{
  iwrite("8");
}
#endif

bool POPS_Cmd::app_input() {
  bool rv = false;
  if (nc > 0) {
    switch (buf[0]) {
      case 'B': // Send POPS start to POPS_srvr
      case 'E': // Send Shutdown to POPS_srvr
      case 'F': // Send forced shutdown to POPS_srvr
        nl_assert(POPS_client::instance);
        POPS_client::instance->forward(buf);
        break;
#if 0
      case 'S': // Send Shutdown to POPS
        SD->send_shutdown();
        break;
#endif
      case 'Q':
        rv = true;
        break;
      default:
        msg(2, "%s: Invalid command letter: '%c' (0x%X)",
          iname, buf[0], buf[0]);
        break;
    }
    report_ok(nc);
  }
  return rv;
}

#if 0
void POPS_Cmd::send_shutdown() {
  int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_sock == -1) {
    msg(MSG_ERROR, "%s: Unable to create UDP socket for shutdown: %s",
      iname, strerror(errno));
    return;
  }
  struct addrinfo hints, *res;
  hints.ai_flags = AI_NUMERICHOST;
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_addrlen = 0;
  hints.ai_canonname = 0;
  hints.ai_addr = 0;
  hints.ai_next = 0;
  const char *hostname, *port;
  hostname = hs_registry::query_host("POPS"));
  
  if (getaddrinfo("10.11.97.50", "7079", &hints, &res)) {
    msg(MSG_ERROR, "%s: getaddrinfo for shutdown failed: %s",
      iname, strerror(errno));
    return;
  }
  nl_assert(res->ai_next == 0);
  nl_assert(res->ai_addr != 0);

  struct sockaddr_in s;
  socklen_t addrlen;
  nl_assert(((unsigned)res->ai_addrlen) <= sizeof(s));
  memcpy(&s, res->ai_addr, res->ai_addrlen);
  addrlen = res->ai_addrlen;
  freeaddrinfo(res);
  
  const char *buf = "8";
  int msglen = 1;
  int nb = sendto(udp_sock, buf, msglen, 0, (sockaddr*)&s, addrlen);
  if (nb < msglen) {
    msg(MSG_ERROR, "%s: sendto() expected %d, returned %d",
      iname, msglen, nb);
  }
  ::close(udp_sock);
}
#endif

POPS_client::POPS_client() :
      DAS_IO::Client("pops", "POPS", "pops", 0, 80),
      srvr_Stale(0) {
  POPS.Srvr = 0;
  nl_assert(POPS_client::instance == 0);
  POPS_client::instance = this;
  set_retries(-1, 5, 5);
  set_connect_timeout(5,0);
  flags |= gflag(0);
}

bool POPS_client::app_connected() {
  forward("V\n");
  ++srvr_Stale;
  return false;
}

bool POPS_client::app_input() {
  cp = 0;
  while (cp < nc) {
    if (isdigit(buf[cp])) {
      uint8_t old_Srvr = POPS.Srvr;
      if (not_uint8(POPS.Srvr) || not_str("\n")) {
        report_err("%s: poorly formed status response", iname);
      }
      if (POPS.Srvr != old_Srvr)
        msg(MSG_DBG(0), "%s: POPS.Srvr = %d", iname, POPS.Srvr);
    } else {
      while (cp < nc && buf[cp] != '\n') ++cp;
      if (cp < nc) {
        // buf[cp] must be == '\n'
        buf[cp++] = '\0';
        msg(MSG_ERROR, "%s: %s", iname, buf);
      } else {
        msg(MSG_ERROR, "%s: missing nl: %s", iname, buf);
      }
    }
  }
  TO.Clear();
  flags &= ~Fl_Timeout;
  srvr_Stale = 0;
  report_ok(nc);
  return false;
}

bool POPS_client::forward(const uint8_t *cmd) {
  if (is_negotiated()) {
    msg(MSG_DBG(1), "%s: Forwarding command %c", iname, cmd[0]);
    iwrite((const char *)cmd);
    TO.Set(3,0);
    flags |= Fl_Timeout;
  } else {
    msg(MSG_ERROR, "%s: command issued before comms established", iname);
  }
  return false;
}

/**
 * We should only get here if socket_state == Socket_connected.
 * Timeouts while connecting are handled by Socket::ProcessData().
 * This timeout occurs if we timeout during a transaction with the
 * server. Our response as of now will be to immediately tear down
 * the connection, reset the reported server state to Init and
 * begin the cycle of attempting a new connection.
 */
bool POPS_client::protocol_timeout() {
  TO.Clear();
  POPS.Srvr = 0;
  msg(MSG_ERROR, "%s: Timeout (resetting)", iname);
  return reset();
}

bool POPS_client::app_process_eof() {
  msg(MSG_ERROR, "%s: Lost contact with POPS, retrying", iname);
  POPS.Srvr = 0;
  connect_later(5,0);
  return false;
}

bool POPS_client::tm_sync() {
  msg(MSG_DBG(1), "%s: POPS_client::tm_sync()", iname);
  if (POPS.Stale < 255) ++POPS.Stale;
  if (POPS.Stale > 2 && socket_state == Socket_connected) {
    if (srvr_Stale == 0)
      return app_connected();
    else
      if (srvr_Stale < 255) ++srvr_Stale;
    if (srvr_Stale == 5)
      msg(2, "%s: srvr_Stale=%d while connected", iname, srvr_Stale);
  }
  return false;
}

bool POPS_client::connect_failed() {
  msg(MSG_DBG(2), "%s: connect_failed()", iname);
  return false;
}

POPS_client *POPS_client::instance = 0;
