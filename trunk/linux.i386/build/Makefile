
include Makefile.param

OBJS = $(SRCS:$(SRC_ROOT)/%.cpp=%.o)

all:    $(OBJS)
.PHONY: all

clean:
	@rm -f $(OBJS)

%.o:      $(SRC_ROOT)/%.cpp
	$(CC) -c $(CPPFLAGS) -o $@ $<



