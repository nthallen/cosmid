# cosmid.spec
tmcbase = base.tmc
cmdbase = cosmid.cmd
Module TMAstat TMA=cosmid.tma TextWidth=11
swsbase = cosmid.sws
genuibase = cosmid.genui
Module TMbase mode=ignore
Module POPS mode=server
Module POPS pfx=POPS2
Module coretemps_select
Module tm_ipx
Module Xiomas
Module Nephex
#Module Sat Prefix=cosmid Relay=$spi4
Module Sat Prefix=cosmid Relay=$relay_ext Relay2=$relay_ext
TGTDIR = /home/cosmid
SCRIPT = VERSION Experiment.config
DISTRIB = interact services services.cosmid.sat services.cosmid.satrelay
IGNORE = Makefile "*.exe"
IGNORE = SWData.h SWData.tmc SWData_col.tmc SWData.cmd
OBJ = SWData.h SWData.tmc SWData_col.tmc SWData.cmd

cosmidtxsrvr :
cosmiddisp : cosmid.tbl POPS.tbl POPS2.tbl
cosmidalgo : cosmid.tma $swsbase
cosmidjsonext : $extbase $genuibase
doit : cosmid.doit
