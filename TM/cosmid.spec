# cosmid.spec
tmcbase = base.tmc
Module TMbase mode=ignore
Module POPS

TGTDIR = /home/cosmid
SCRIPT = VERSION Experiment.config
DISTRIB = interact services
IGNORE = Makefile "*.exe"

cosmiddisp : cosmid.tbl POPS.tbl
cosmidalgo : cosmid.tma
doit : cosmid.doit
