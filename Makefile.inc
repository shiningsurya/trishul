# Beginning of a long night

ROOT:=/home/shining/work/trishul
BIN:=/home/shining/work/bin

CXX:= g++ -std=c++17
LINK:= g++ 

CFLAGS:= -fPIC -g  -frtti


INCLUDE_FLAGS:= -I/home/shining/work
INCLUDE_FLAGS+= -I$(ROOT)/Application
INCLUDE_FLAGS+= -I$(ROOT)
INCLUDE_FLAGS+= -I$(ROOT)/Plotting
INCLUDE_FLAGS+= -I$(ROOT)/Dedispersion
INCLUDE_FLAGS+= -I$(ROOT)/Formats

LDPGPLOT := -lcpgplot -lpng -lX11 -lpgplot -lgfortran
INCLUDES := $(INCLUDE_FLAGS)
