# cosmid.spec
tmcbase = base.tmc
genuibase = cosmid.genui
Module TMbase mode=ignore
Module POPS mode=server
Module POPS pfx=POPS2
Module coretemps_select

TGTDIR = /home/cosmid
SCRIPT = VERSION Experiment.config
DISTRIB = interact services
IGNORE = Makefile "*.exe"

cosmiddisp : cosmid.tbl POPS.tbl POPS2.tbl
cosmidalgo : cosmid.tma
cosmidjsonext : $extbase $genuibase
doit : cosmid.doit
