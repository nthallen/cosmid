%INTERFACE <POPS>

&command
  : POPS Start * { if_POPS.Turf("B\n"); }
# : POPS Shutdown * { if_POPS.Turf("S\n"); }
  : POPS Shutdown * { if_POPS.Turf("E\n"); }
  : POPS Shutdown Force * { if_POPS.Turf("F\n"); }
# : POPS Server Shutdown * { if_POPS.Turf("E\n"); }
# : POPS Server Shutdown Force * { if_POPS.Turf("F\n"); }
  : POPS Driver Quit * { if_POPS.Turf("Q\n"); }
  ;
