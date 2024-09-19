CXXFLAGS = -Wall -g -std=c++11 -D_DEFAULT_SOURCE
LDFLAGS = -L/usr/local/lib -ldasio -lnl -lpthread
.PHONY : clean

POPS : POPS.o POPSoui.o
	$(CXX) -o POPS $(CXXFLAGS) POPS.o POPSoui.o $(LDFLAGS)
POPSoui.o : POPSoui.cc
POPSoui.cc : POPS.oui
	oui -o POPSoui.cc POPS.oui
clean :
	rm -f POPS POPSoui.cc *.o
