#ifndef MLF_PACKET_LOGGER_H_INCLUDED
#define MLF_PACKET_LOGGER_H_INCLUDED

#include "mlf.h"

class mlf_packet_logger {
  public:
    mlf_packet_logger(const char *iname,
      const char *mlf_base, const char *mlf_config);
    void log_packet(const unsigned char *bfr, uint16_t pkt_len);
    void next_file();
  protected:
    const char *miname;
    mlf_def_t *mlf;
    int ofd;
    int Bytes_in_File;
    const static int Bytes_per_File = 4096; // for testing. 10x that for real
};

#endif
