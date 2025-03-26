CXXFLAGS = -Wall -g -std=c++11 -D_DEFAULT_SOURCE
LDFLAGS = -L/usr/local/lib -ldasio -lnl -lpthread
.PHONY : clean

XEXPOBJ=xiomas_export.o xiomas_exportoui.o XioHarvGatewayPacket.o
xiomas_export : $(XEXPOBJ)
	$(CXX) -o xiomas_export $(CXXFLAGS) $(XEXPOBJ) $(LDFLAGS)
xiomas_exportoui.o : xiomas_exportoui.cc
xiomas_exportoui.cc : xiomas_export.oui
	oui -o xiomas_exportoui.cc xiomas_export.oui
clean :
	rm -f xiomas_export xiomas_exportoui.cc *.o
