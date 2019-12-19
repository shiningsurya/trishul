# One make to rule them all
# For trishul
include Makefile.inc

VPATH=./:./Dedispersion:./Plotting:./Formats:./Application:./ob

INCLUDES+= -I$(BOOST_INC) -L$(BOOST_LIB)

./ob/%.o : %.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./ob/BSON.o : BSON.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) $(JSONINC) -c $< -o $@

./ob/CandidateProfilePlot.o :  CandidateProfilePlot.cpp
		$(CXX) $(CFLAGS) $(INCLUDES)  \
		-c $< -o $@

./ob/CandidatePlot.o :  CandidatePlot.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(DEDISP_INC) -I$(PGPLOT_INC) -L$(PGPLOT_LIB) \
		-L$(PGFORTRAN_LIB) -c $< -o $@

./ob/BowtiePlot.o :  BowtiePlot.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(DEDISP_INC) -I$(PGPLOT_INC) -L$(PGPLOT_LIB) \
		-L$(PGFORTRAN_LIB) -c $< -o $@

./ob/TestCandidatePlot.o : TestCandidatePlot.cpp
		$(CXX) $(CFLAGS) $(INCLUDES)   $(JSONINC) -c $< -o $@

./ob/TestCPlot.o : TestCPlot.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(DEDISP_INC) $(JSONINC) -c $< -o $@

./ob/Dedisp.o : Dedisp.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(DEDISP_INC) -c $< -o $@

./ob/TestDedisp.o : TestDedisp.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(DEDISP_INC) -c $< -o $@

./ob/TestFilterbankDedisp.o : TestFilterbankDedisp.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(DEDISP_INC) -c $< -o $@

testfcp : $(addprefix ./ob/, TestCPlot.o CandidatePlot.o TrishulPlotting.o \
								Header.o BSON.o TrishulFormats.o \
								PackUnpack.o Incoherent.o Timer.o Globals.o\
								TrishulDedisperser.o Dedisp.o)

		$(LINK) $+ -L$(BOOST_LIB) -L$(DEDISP_LIB) -Wl,-rpath=$(BOOST_LIB) $(LD_FLAGS) $(DEDISP_LD) $(BOOST_LD) -o $@  

testcp : $(addprefix ./ob/, TestCandidatePlot.o CandidateProfilePlot.o TrishulPlotting.o \
								Header.o BSON.o TrishulFormats.o \
								PackUnpack.o \
								Incoherent.o TrishulDedisperser.o )

		$(LINK) $+ $(LD_FLAGS) -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB) $(PGPLOT_LD) $(BOOST_LD) -o $@  

testdd : $(addprefix ./ob/, TestDedisp.o \
								Header.o BSON.o TrishulFormats.o \
								PackUnpack.o \
								Dedisp.o TrishulDedisperser.o )
		$(LINK) $+ -L$(DEDISP_LIB)  -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB) $(DEDISP_LD) $(BOOST_LD) -o $@  

testfb : $(addprefix ./ob/, TestFilterbank.o Filterbank.o TrishulFormats.o \
								Header.o \
								PackUnpack.o )

		$(LINK) $+ -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB)  $(BOOST_LD) -o $@  

testfbt : $(addprefix ./ob/, TestFBT.o FilterbankBowtiePlot.o TrishulPlotting.o \
								Header.o Filterbank.o TrishulFormats.o \
								PackUnpack.o \
								Incoherent.o TrishulDedisperser.o )

		$(LINK) $+ -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB) $(PGPLOT_LD) $(BOOST_LD) -o $@  

testfbd : $(addprefix ./ob/, TestFilterbankDedisp.o FilterbankBowtiePlot.o TrishulPlotting.o \
								Header.o Filterbank.o TrishulFormats.o \
								PackUnpack.o \
								Dedisp.o TrishulDedisperser.o )

		$(LINK) $+ -L$(DEDISP_LIB)  -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB) $(DEDISP_LD) $(BOOST_LD) $(LD_FLAGS) -o $@  

testmglfb : $(addprefix ./ob/, TestMGL_FB.o \
								Header.o Filterbank.o TrishulFormats.o \
								PackUnpack.o \
								Incoherent.o TrishulDedisperser.o )

		$(LINK) $+ -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB) $(PGPLOT_LD) $(BOOST_LD) -o $@  
clean:
		rm -f ./ob/*.o

all : tscandplot

tscandplot : $(addprefix ./ob/, tscandplot.o CandidatePlot.o TrishulPlotting.o \
								Header.o BSON.o TrishulFormats.o \
								PackUnpack.o Incoherent.o Timer.o Globals.o\
								TrishulDedisperser.o Dedisp.o)

		$(LINK) $+ -L$(BOOST_LIB) -L$(DEDISP_LIB) -Wl,-rpath=$(BOOST_LIB) $(LD_FLAGS) $(DEDISP_LD) $(BOOST_LD) -o $@  

tsbtplot : $(addprefix ./ob/, tsbtplot.o BowtiePlot.o TrishulPlotting.o \
								Header.o BSON.o TrishulFormats.o \
								PackUnpack.o Timer.o Globals.o\
								TrishulDedisperser.o BTIncoherent.o Incoherent.o)

		$(LINK) $+ -L$(BOOST_LIB)  -Wl,-rpath=$(BOOST_LIB) $(LD_FLAGS)  $(BOOST_LD) -o $@  
.PHONY:
		clean
