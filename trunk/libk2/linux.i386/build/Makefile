

include Makefile.conf

.PHONY: all clean opt debug

all:    opt debug

clean:
	make -f Makefile.opt clean
	make -f Makefile.debug clean

opt:
	make -f Makefile.opt all

debug:
	make -f Makefile.debug all
