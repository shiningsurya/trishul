# Beginning of a long night

ROOT:=/home/shining/work/trishul
BIN:=/home/shining/work/bin

CXX:= g++ -std=c++17
COMPILE:= $(CXX)
LINK:= $(CXX)

COMPILEFLAGS:= -fPIC -g  -frtti

LDFLAGS:= 

INCLUDE_FLAGS:= -I/home/shining/work
INCLUDE_FLAGS+= -I$(ROOT)/Application
INCLUDE_FLAGS+= -I$(ROOT)/Application/trishul
INCLUDE_FLAGS+= -I$(ROOT)/Plotting
INCLUDE_FLAGS+= -I$(ROOT)/Dedispersion
INCLUDE_FLAGS+= -I$(ROOT)/Formats

LDPGPLOT := -lcpgplot -lpng -lX11 -lpgplot -lgfortran
