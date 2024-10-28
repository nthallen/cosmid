CXXFLAGS=-Wall -I. -std=c++0x -D_POSIX_C_SOURCE=199309

OBJ = interface.o socket.o server.o loop.o msg.o \
  POPS_srvr.o POPS_srvroui.o \
  timeout.o ascii_escape.o nl_error.o nl_verr.o \
  newmem.o unix_name.o not_parsed.o nlresp.o \
  nldbg.o appid.o client.o host_session.o

DESTDIR = /root/SW/bin

.PHONY : all clean diffs install install-service install-config

all : POPS_srvr

POPS_srvr : $(OBJ)
	$(CXX) -o POPS_srvr $(CXXFLAGS) $(OBJ) -lpthread -lrt

install : POPS_srvr
	mkdir -p $(DESTDIR)
	cp POPS_srvr POPSstow start_POPS services $(DESTDIR)

install-service : pops_server.service
	cp pops_server.service /lib/systemd/system/
	systemctl daemon-reload
	systemctl start pops_server
	systemctl enable pops_server

install-config : interfaces POPS_BBB.cfg
	cp interfaces /etc/network/
	cp POPS_BBB.cfg /media/uSD/

diffs :
	diff /etc/network/interfaces interfaces
	diff /media/uSD/POPS_BBB.cfg POPS_BBB.cfg
	diff /lib/systemd/system/pops_server.service pops_server.service

clean :
	rm -f *.o POPS_srvr
