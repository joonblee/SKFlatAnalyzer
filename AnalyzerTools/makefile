LIBRARY = AnalyzerTools
OBJDIR = obj
DEPDIR = $(OBJDIR)/dep
SRCDIR = src
INCDIR = include
ADDITIONAL_ROOTMAPLIBRARY= -rml libDataFormats.so -rml libHist.so

include $(SKFlat_WD)/makefile.common

INCLUDES += -I$(SKFlat_WD)/DataFormats/include/
#INCLUDES += -I/cvmfs/cms.cern.ch/slc6_amd64_gcc630/external/lhapdf/6.2.1-fmblme/include/

INCLUDES += -I$(CORRECTIONLIB_INCLUDE)

EXTRA_LIBS += -L$(CORRECTIONLIB_LIBDIR)
EXTRA_LIBS += -Wl,-rpath,$(CORRECTIONLIB_LIBDIR)
EXTRA_LIBS += -lcorrectionlib
