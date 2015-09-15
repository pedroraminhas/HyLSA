CC       := g++
CFLAGS   += -std=c++11 -g -w -pthread -mrtm -fpermissive
CFLAGS   += -O2
CFLAGS   += -I$(LIB) -I../../../rapl-power/ -I ../../../stms/tl2/
CPP      := g++
CPPFLAGS += $(CFLAGS)
LD       := g++
LIBS     += -lpthread

# Remove these files when doing clean
OUTPUT +=

LIB := ../lib