# cosmid.spec
tmcbase = base.tmc
Module TMbase mode=ignore

TGTDIR = /home/cosmid
SCRIPT = VERSION Experiment.config
DISTRIB = interact services
IGNORE = Makefile "*.exe"

cosmiddisp : cosmid.tbl
doit : cosmid.doit
