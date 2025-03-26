CXXFLAGS = -Wall -g -std=c++11 -D_DEFAULT_SOURCE
LDFLAGS = -L/usr/local/lib -ldasio -lnl -lpthread
.PHONY : clean

xiomas_export : xiomas_export.o xiomas_exportoui.o
	$(CXX) -o xiomas_export $(CXXFLAGS) xiomas_export.o xiomas_exportoui.o $(LDFLAGS)
xiomas_exportoui.o : xiomas_exportoui.cc
xiomas_exportoui.cc : xiomas_export.oui
	oui -o xiomas_exportoui.cc xiomas_export.oui
clean :
	rm -f xiomas_export xiomas_exportoui.cc *.o
