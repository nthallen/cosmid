CXXFLAGS = -Wall
LDFLAGS = -L/usr/local/lib -ldasio -lnl -lpthread
.PHONY : all clean

all : POPS_srvr

OBJ = POPS_srvr.o POPS_srvroui.o

POPS_srvr : $(OBJ)
	$(CXX) -o POPS_srvr $(CXXFLAGS) $(OBJ) $(LDFLAGS)
POPS_srvroui.o : POPS_srvroui.cc
POPS_srvroui.cc : POPS_srvr.oui
	oui -o POPS_srvroui.cc POPS_srvr.oui

clean :
	rm -rf $(OBJ) POPS_srvroui.cc POPS_srvr
