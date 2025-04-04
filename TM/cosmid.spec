# cosmid.spec
tmcbase = base.tmc
cmdbase = cosmid.cmd
genuibase = cosmid.genui
Module TMbase mode=ignore
Module POPS mode=server
Module POPS pfx=POPS2
Module coretemps_select
Module tm_ipx
Module Xiomas
Module Sat Prefix=cosmid Relay=$spi4
TGTDIR = /home/cosmid
SCRIPT = VERSION Experiment.config
DISTRIB = interact services
IGNORE = Makefile "*.exe"

cosmidtxsrvr :
cosmiddisp : cosmid.tbl POPS.tbl POPS2.tbl
cosmidalgo : cosmid.tma
cosmidjsonext : $extbase $genuibase
doit : cosmid.doit
