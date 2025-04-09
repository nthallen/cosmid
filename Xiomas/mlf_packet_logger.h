#ifndef MLF_PACKET_LOGGER_H_INCLUDED
#define MLF_PACKET_LOGGER_H_INCLUDED

#include "mlf.h"

class mlf_packet_logger {
  public:
    mlf_packet_logger(const char *iname,
      const char *mlf_base, const char *mlf_config);
    ~mlf_packet_logger();
    enum log_mode { log_default, log_newfile, log_curfile };
    void log_packet(const uint8_t *bfr, uint32_t pkt_len,
          log_mode mode=log_default);
    void next_file();
  protected:
    const char *miname;
    mlf_def_t *mlf;
    int ofd;
    int Bytes_in_File;
    const static int Bytes_per_File = 4096; // for testing. 10x that for real
};

#endif
