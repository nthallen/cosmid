CXXFLAGS = -Wall -g -std=c++11 -D_DEFAULT_SOURCE
LDFLAGS = -L/usr/local/lib -ldasio -lnl -lpthread
.PHONY : clean

NEXPOBJ=nephex_export.o nephex_exportoui.o mlf_packet_logger.o
nephex_export : $(NEXPOBJ)
	$(CXX) -o nephex_export $(CXXFLAGS) $(NEXPOBJ) $(LDFLAGS)
nephex_exportoui.o : nephex_exportoui.cc
nephex_exportoui.cc : nephex_export.oui
	oui -o nephex_exportoui.cc nephex_export.oui
clean :
	rm -f nephex_export nephex_exportoui.cc *.o
