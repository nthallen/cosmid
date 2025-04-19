CXXFLAGS = -Wall -g -std=c++11 -D_DEFAULT_SOURCE
LDFLAGS = -L/usr/local/lib -ldasio -lnl -lpthread
.PHONY : clean all

all : nephex_emulator nephex_export

NEXPOBJ=nephex_export.o nephex_exportoui.o
NEXPCLEAN=nephex_export $(NEXPOBJ) nephex_exportoui.cc
NEMOBJ=nephex_emulator.o nephex_emulatoroui.o
NEMCLEAN=nephex_emulator $(NEMOBJ) nephex_emulatoroui.cc

nephex_export : $(NEXPOBJ)
	$(CXX) -o nephex_export $(CXXFLAGS) $(NEXPOBJ) $(LDFLAGS)
nephex_exportoui.o : nephex_exportoui.cc
nephex_exportoui.cc : nephex_export.oui
	oui -o nephex_exportoui.cc nephex_export.oui

nephex_emulator : $(NEMOBJ)
	$(CXX) -o nephex_emulator $(CXXFLAGS) $(NEMOBJ) $(LDFLAGS)
nephex_emulatoroui.o : nephex_emulatoroui.cc
nephex_emulatoroui.cc : nephex_emulator.oui
	oui -o nephex_emulatoroui.cc nephex_emulator.oui

clean :
	rm -f nephex_export nephex_exportoui.cc *.o
