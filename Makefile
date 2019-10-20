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
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(PGPLOT_INC) -L$(PGPLOT_LIB) \
		-L$(PGFORTRAN_LIB) -c $< -o $@

./ob/TestCandidatePlot.o : TestCandidatePlot.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) $(JSONINC) -c $< -o $@

./ob/Dedisp.o : Dedisp.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(DEDISP_INC) -c $< -o $@

./ob/TestDedisp.o : TestDedisp.cpp
		$(CXX) $(CFLAGS) $(INCLUDES) -I$(DEDISP_INC) -c $< -o $@

testcp : $(addprefix ./ob/, TestCandidatePlot.o CandidateProfilePlot.o TrishulPlotting.o \
								Header.o BSON.o TrishulFormats.o \
								PackUnpack.o \
								Incoherent.o TrishulDedisperser.o )

		$(LINK) $+ -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB) $(PGPLOT_LD) $(BOOST_LD) -o $@  

testdd : $(addprefix ./ob/, TestDedisp.o \
								Header.o BSON.o TrishulFormats.o \
								PackUnpack.o \
								Dedisp.o TrishulDedisperser.o )
		$(LINK) $+ -L$(DEDISP_LIB)  -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB) $(DEDISP_LD) $(BOOST_LD) -o $@  
clean:
		rm -f ./ob/*.o

.PHONY:
		clean
