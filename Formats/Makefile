
include ../Makefile.inc

%.o : %.cpp
	$(COMPILE) $(COMPILEFLAGS) $(INCLUDE_FLAGS) -c $? -o $@ 


T_BSON := TestBSON.o BSON.o TrishulFormats.o Header.o
testbson: $(T_BSON) 
	$(LINK) $(T_BSON) $(LDFLAGS) -o $(BIN)/$@

