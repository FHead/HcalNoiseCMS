OFILES = HistogramManager.o NoiseTreeData.o HBHEChannelMap.o HcalDetId.o \
         HcalHPDRBXMap.o HBHEChannelGeometry.o fitHcalEnergies.o \
         HcalPulseShape.o HcalPulseShapes.o HcalShapeIntegrator.o \
         HcalTimeSlew.o HcalPulseContainmentAlgo.o MixedChargeInfo.o \
         HcalPulseContainmentCorrection.o skipComments.o

PROGRAMS = exampleTreeAnalysis.ana runNoiseTreeAnalysis.ana \
           runMixedChargeAnalysis.ana

ROOTCONFIG   := root-config

ARCH         := $(shell $(ROOTCONFIG) --arch)
PLATFORM     := $(shell $(ROOTCONFIG) --platform)

CXX           =  g++

ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)
HASTHREAD    := $(shell $(ROOTCONFIG) --has-thread)

CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)

NPSTAT_DIR = $(HOME)/local
NPSTAT_LIB = $(NPSTAT_DIR)/lib
NPSTAT_INC = $(NPSTAT_DIR)/include

LIBS = $(ROOTLIBS) -L$(NPSTAT_LIB) -L/usr/lib64 -lnpstat -llapack -lblas \
        -lfftw3 -lgeners -lbz2 -lz -ldl -lm

CXXFLAGS = -fPIC -Wall -g -std=c++0x $(ROOTCFLAGS) -I$(NPSTAT_INC) -I.
LINKFLAGS = -fPIC -g -std=c++0x

%.o : %.C
	$(CXX) -c $(CXXFLAGS) -MD $< -o $@
	@sed -i 's,\($*\.o\)[:]*\(.*\),$@: $$\(wildcard\2\)\n\1:\2,g' $*.d

%.C : %.ana
	rm -f $@
	sed "s/ANALYSIS_HEADER_FILE/$</g" analysisExecutableTemplate.C > $@

BINARIES = $(PROGRAMS:.ana=)

all: $(BINARIES) tools

$(BINARIES): % : %.o $(OFILES); g++ $(LINKFLAGS) -fPIC -o $@ $^ $(LIBS)

tools:
	make -f Makefile.tools

clean:
	rm -f $(BINARIES) $(PROGRAMS:.ana=.C) core.* *.o *.d *~
	make -f Makefile.tools clean 

-include $(OFILES:.o=.d)
-include $(PROGRAMS:.ana=.d)
