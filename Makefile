# $@	full target name
# $^	all dependacnies
# $?	out of date dependancies
# $<
# $*	target without suffix

%.o	: %.cc	;	$(CXX) -c $(CXXFLAGS) $<
%.o	: %.c	;	$(CC)  -c $(CFLAGS) $<


DEBUGFLAGS+=-g #-pg -DDBX_DEBUGGING_INFO -DXCOFF_DEBUGGING_INFO #-DDEBUG
CFLAGS=$(DEBUGFLAGS) -I/home/ray/tools/include -DNEED_UCHAR_UINT_T
CXXFLAGS=$(DEBUGFLAGS) -I/home/ray/tools -I/home/ray/tools/include -DNEED_UCHAR_UINT_T -g
LDFLAGS=$(DEBUGFLAGS)

TARGETS=dvdpacking

all:	objs $(TARGETS)
objs:	Item.o Bin.o Packer.o dvdpacking.o


Item.o:		Item.cc Item.h
Bin.o:		Bin.cc Bin.h Item.h Items.h
Packer.o:	Packer.cc Packer.h Bin.h Bins.h Item.h Items.h
dvdpacking.o:	dvdpacking.cc dvdpacking.h Packer.h Bins.h Bin.h Item.h Items.h

dvdpacking:	dvdpacking.o Packer.o Bin.o Item.o
	$(CXX)  $^ $(LDFLAGS) -o $@

clean:
	rm -fr *.o core.* $(TARGETS) 
