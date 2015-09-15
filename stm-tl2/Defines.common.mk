STM := ../../../stms/tl2

CC       := g++
CFLAGS   += -std=c++11 -g -w -pthread -fpermissive
CFLAGS   += -O2
CFLAGS   += -I$(LIB) -I../../../rapl-power/ -I$(STM)
CPP      := g++
CPPFLAGS += $(CFLAGS)
LD       := g++
LIBS     += -lpthread

LIB := ../lib
