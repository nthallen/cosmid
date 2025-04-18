#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "mlf_packet_logger.h"
#include "nl.h"
#include "nl_assert.h"

mlf_packet_logger::mlf_packet_logger(const char *iname,
      const char *mlf_base, const char *mlf_config)
    : miname(iname),
      mlf(0),
      ofd(-1),
      Bytes_in_File(0)
{
  mlf = mlf_init(3, 60, 1, mlf_base, "dat", mlf_config);
}

void mlf_packet_logger::log_packet(const unsigned char *bfr, uint16_t pkt_len)
{
  if (ofd < 0 || 
      ((ofd >= 0) && Bytes_in_File + (int)pkt_len > Bytes_per_File))
    next_file();
  nl_assert(ofd >= 0);
  int rv = write(ofd, bfr, pkt_len);
  if (rv < 0)
    msg(MSG_ERROR, "%s: error %d writing to file: strerror(errno)",
      miname, errno, strerror(errno));
  else if (rv < pkt_len)
    msg(MSG_ERROR, "%s: short write to file: %d/%d", miname, rv, pkt_len);
  else
  {
    Bytes_in_File += pkt_len;
    return;
  }
  // Try moving on to another file:
  ::close(ofd);
  ofd = -1;
}

void mlf_packet_logger::next_file()
{
  if ((ofd >= 0) && ::close(ofd)) {
    msg(MSG_ERROR, "%s: Error closing file: %s", miname, strerror(errno));
  }
  ofd = mlf_next_fd(mlf);
  if (ofd < 0)
    msg(MSG_FATAL, "%s: Unable to open output file", miname);
  Bytes_in_File = 0;
  // write_tstamp();
}
