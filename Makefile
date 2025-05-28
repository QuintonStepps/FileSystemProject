CFLAGS += -g -Wall -Wno-pointer-sign

BINARIES = printsb printblk printbg printinode printidata isfree carve find2i custominode

all: $(BINARIES)

clean:
	$(RM) $(BINARIES) *.o

$(BINARIES): e2util.o

e2util.o: e2util.c e2util.h
