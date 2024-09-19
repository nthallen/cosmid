CXXFLAGS=-Wall -I. -std=c++0x -D_POSIX_C_SOURCE=199309

OBJ = interface.o socket.o server.o loop.o msg.o \
  POPS_srvr.o POPS_srvroui.o \
  timeout.o ascii_escape.o nl_error.o nl_verr.o \
  newmem.o unix_name.o not_parsed.o nlresp.o \
  nldbg.o appid.o client.o host_session.o

DESTDIR = /root/SW/bin

.PHONY : all clean install

all : POPS_srvr

POPS_srvr : $(OBJ)
	$(CXX) -o POPS_srvr $(CXXFLAGS) $(OBJ) -lpthread -lrt

install : POPS_srvr
	cp POPS_srvr POPSstow start_POPS start_POPS_srvr services $(DESTDIR)

clean :
	rm -f *.o POPS_srvr
