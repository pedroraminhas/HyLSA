CC       := g++
CFLAGS   += -std=c++11 -g -w -pthread -mhtm -fpermissive
CFLAGS   += -O2
CFLAGS   += -I$(LIB) -I../../../rapl-power/ -I ../../../stms/tinystm/src/ -I ../../../stms/tinystm/include -I ../../../stms/tinystm/src/atomic_ops
CPP      := g++
CPPFLAGS += $(CFLAGS)
LD       := g++
LIBS     += -lpthread

# Remove these files when doing clean
OUTPUT +=

LIB := ../lib
