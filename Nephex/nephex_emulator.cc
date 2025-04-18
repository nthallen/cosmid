#include <fcntl.h>
#include "nephex_emulator.h"
#include "dasio/quit.h"
#include "dasio/loop.h"
#include "dasio/appid.h"
#include "oui.h"
#include "nl.h"
#include "nl_assert.h"

const char *NE_ingest::nephex_input_file =
  "/home/cosmid/src/Nephex/nephelometer_data.txt";

NE_ingest::NE_ingest()
    : Interface("ingest", 1000),
      packet_sn(0)
{
  fd = open(NE_ingest::nephex_input_file, O_RDONLY);
  flags &= ~Fl_Read;
}

bool NE_ingest::set_client(nephex_tcp *client)
{
  if (this->client)
  {
    return true;
  }
  else
  {
    this->client = client;
    flags |= Fl_Read;
  }
  return false;
}

void NE_ingest::reset_client()
{
  TO.Clear();
  flags &= ~Fl_Read;
  client = 0;
}

bool NE_ingest::protocol_input()
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
    cp += 4;
    uint16_t Len = buf[cp] + (((uint16_t)buf[cp+1])<<8);
    cp += 2;
    if (nc < cp+Len+1) // don't have the whole packet
      return false;
    // Check the CRC
    // Get the time
    uint32_t pkt_secs =
      ((uint32_t)buf[cp]) +
      (((uint32_t)buf[cp+1])<<8) +
      (((uint32_t)buf[cp+2])<<16) +
      (((uint32_t)buf[cp+3])<<24);
    uint16_t pkt_msecs =
      ((uint16_t)buf[cp+4]) +
      (((uint16_t)buf[cp+5])<<8);
    if (!TO.Set())
    {
      msg(MSG_DBG(1), "%s: Initializing time base SN:%u %u:%u", iname,
        packet_sn, pkt_secs, pkt_msecs);
      last_secs = pkt_secs;
      last_msecs = pkt_msecs;
      TO.Set(0, 0);
    }
    if (pkt_secs > last_secs ||
        (pkt_secs == last_secs && pkt_msecs > last_msecs)) {
      uint32_t d_msecs, d_secs;
      if (pkt_msecs < last_msecs)
      {
        nl_assert(pkt_msecs+1000U > last_msecs);
        d_msecs = pkt_msecs+1000U-last_msecs;
        d_secs = pkt_secs-last_secs-1;
      } else {
        d_msecs = pkt_msecs - last_msecs;
        d_secs = pkt_secs - last_secs;
      }
      last_secs = pkt_secs;
      last_msecs = pkt_msecs;
      msg(MSG_DBG(1), "%s: TO update SN:%u %u:%u (%u:%u)", iname,
        packet_sn, pkt_secs, pkt_msecs, d_secs, d_msecs);
      TO.Add(d_secs, d_msecs);
    }
    if (TO.Expired() && client)
    {
      // Forward the packet
      client->forward_packet(&buf[cp-8], Len+9);
      // Look at the next one
      cp += Len + 1;
      ++packet_sn;
    } else {
      cp -= 8;
      break;
    }
  }
  consume(cp);
  return false;
}

bool NE_ingest::protocol_timeout()
{
  return false;
}

nephex_tcp::nephex_tcp(Socket *orig, const char *iname, int fd, NE_ingest *NE)
    : Socket(orig, iname, TCP_BUFSIZE, fd),
      NE(NE)
{
}

nephex_tcp::~nephex_tcp()
{
  if (NE)
  {
    NE->reset_client();
    NE = 0;
  }
}

bool nephex_tcp::forward_packet(uint8_t *pkt, uint16_t len)
{
  if (obuf_empty())
  {
    iwrite((char*)pkt, len);
    return false;
  }
  return true;
}

bool nephex_tcp::connected()
{
  msg(MSG_DBG(0), "%s: connected", iname);
  nl_assert(NE && ELoop);
  if (NE->set_client(this)) // more than one client!
    ELoop->delete_child(this);
  return false;
}

nephex_tcp_svc::nephex_tcp_svc(const char *iname, NE_ingest *NE)
    : Socket(iname, "ntcp", Socket_TCP),
      NE(NE)
{
}

Socket *nephex_tcp_svc::new_client(const char *iname, int fd)
{
  Socket *rv = new nephex_tcp(this, iname, fd, NE);
  if (ELoop) ELoop->add_child(rv);
  return rv;
}

int main(int argc, char **argv)
{
  oui_init_options(argc, argv);
  {
    Loop ELoop;
    
    AppID.report_startup();
    NE_ingest *NE = new NE_ingest();
    ELoop.add_child(NE);

    /* TCP listener on nephex service: nephex_tcp_svc */
    nephex_tcp_svc *NTCP = new nephex_tcp_svc("NTCP", NE);
    NTCP->connect();
    ELoop.add_child(NTCP);
    
    Quit *Q = new Quit();
    Q->connect();
    ELoop.add_child(Q);
    
    ELoop.event_loop();
  }
  AppID.report_shutdown();
}
