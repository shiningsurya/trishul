# One make to rule them all
# For trishul
include Makefile.inc

VPATH=./Dedispersion:./Plotting:./Formats:./Application

%.o : %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $? -o $@

testcp : TestCandidatePlot.o CandidateProfilePlot.o TrishulPlotting.o \
	Header.o BSON.o TrishulFormats.o \
	PackUnpack.o \
	Incoherent.o TrishulDedisperser.o

	$(LINK) $^ -o $@  $(LDPGPLOT)
