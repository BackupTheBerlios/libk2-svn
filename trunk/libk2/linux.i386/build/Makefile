
CPPFLAGS_OPT    = -DK2_BUILD -I../../ -I../depend/include/ -Wall -Weffc++
CPPFLAGS_DEBUG  = -g -DK2_DEBUG -DK2_BUILD -I../../ -I../depend/include/ -Wall -Weffc++
LIBK2_ROOT      = ../..
SRC_ROOT        = $(LIBK2_ROOT)/source
BIN_OPT         = $(LIBK2_ROOT)/linux.i386/bin.opt
BIN_DEBUG       = $(LIBK2_ROOT)/linux.i386/bin.opt

SRCS	= \
	$(SRC_ROOT)/assert.cpp \
	$(SRC_ROOT)/atomic.cpp \
	$(SRC_ROOT)/ipv4.cpp \
	$(SRC_ROOT)/process.cpp \
	$(SRC_ROOT)/runtime.cpp \
	$(SRC_ROOT)/socket_error.cpp \
	$(SRC_ROOT)/threading.cpp \
	$(SRC_ROOT)/timing.cpp

RULES_OPT   = $(SRCS:$(SRC_ROOT)/%.cpp=$(BIN_OPT)/%.rule)
RULES_DEBUG = $(SRCS:$(SRC_ROOT)/%.cpp=$(BIN_DEBUG)/%.rule)
OBJS_OPT    = $(SRCS:$(SRC_ROOT)/%.cpp=$(BIN_OPT)/%.o)
OBJS_DEBUG  = $(SRCS:$(SRC_ROOT)/%.cpp=$(BIN_DEBUG)/%.o)

#   Object files depend on rule files
$%.o: %.rule
	@make -f $<

#   Optimized rule files depend on source files
$(BIN_OPT)/%.rule: $(SRC_ROOT)/%.cpp
	@set -e; rm -f $@; \
	g++ -MM $(CPPFLAGS_OPT) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;\
	echo 'g++ -o $< $(CPPFLAGS_OPT)' >> $@

#   So do debug rule files depend on source files, but not neccessary the same,
#   because might has directive to include different headers.
$(BIN_DEBUG)/%.rule: $(SRC_ROOT)/%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS_DEBUG) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;\
	echo 'g++ -o $< $(CPPFLAGS_DEBUG)' >> $@

clean:  CLEAN
.PHONY: clean

CLEAN:
    @rm $(RULES);\
    rm $(OBJS)
