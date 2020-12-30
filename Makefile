APP_NAME := ana

ifdef PLIK
CPP_FLAGS+= -DPLIK=$(PLIK)
endif

SOURCE_FILES := analysis.cc
#SOURCE_FILES := analysisElastic.cc
USES_RFIO := no
USES_ORACLE := yes
CPP_FLAGS+= -g -I/lustre/nyx/hades/user/shower/PION/HNTUPLE -I/lustre/nyx/hades/user/shower/PION/PLUTO/CODE/src -I/lustre/nyx/hades/user/shower/PION/HFILTER

include $(HADDIR)/hades.def.mk

LD+= -g `root-config  --libs` -L/lustre/nyx/hades/user/shower/PION/HNTUPLE -L/lustre/nyx/hades/user/shower/PION/HFILTER -L/lustre/nyx/hades/user/shower/PION/PLUTO/CODE -lTuple -lHFilter

.PHONY:  default
default: build install

include $(HADDIR)/hades.app.mk


