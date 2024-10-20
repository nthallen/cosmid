# cosmid.spec
tmcbase = base.tmc
genuibase = cosmid.genui
Module TMbase mode=ignore
Module POPS
Module coretemps

TGTDIR = /home/cosmid
SCRIPT = VERSION Experiment.config
DISTRIB = interact services
IGNORE = Makefile "*.exe"

cosmidcol : -lsensors
cosmiddisp : cosmid.tbl POPS.tbl
cosmidalgo : cosmid.tma
cosmidjsonext : $extbase $genuibase
doit : cosmid.doit
