
include Makefile.conf

all:        opt debug
.PHONY:     all

include $(RULES_OPT) $(RULES_DEBUG)

clean:
	@rm -f $(OBJS_OPT);\
	rm -f $(DL_OPT);\
	rm -f $(DL_OPT:%.dll=%.a);\
	rm -f $(OBJS_DEBUG);\
	rm -f $(DL_DEBUG);\
	rm -f $(DL_DEBUG:%.dll=%.a)

DL_OPT      = $(BIN_OPT)/libk2.dll
DL_DEBUG    = $(BIN_DEBUG)/libk2.dll

#$(BIN_OPT)/%.o:    $(SRC_ROOT)/%.cpp
#	g++ -c $(CPPFLAGS_OPT) -o $@ $<

$(BIN_DEBUG)/%.o:  $(SRC_ROOT)/%.cpp
	g++ -c $(CPPFLAGS_DEBUG) -o $@ $<


opt:        $(OBJS_OPT)
	g++ -s -shared -lc -lstdc++ -lpthread -o $(DL_OPT) $? -Wl,--entry=_k2_cygwin_main@12;\
	dlltool --dllname $(DL_OPT) --output-lib $(DL_OPT:%.dll=%.a)

#	g++ -s -shared -Wl,--base-file,libk2.base -lc -lstdc++ -lpthread -o libk2.dll $? -Wl,--entry=_k2_cygwin_main@12
#	dlltool --base-file libk2.base --output-exp libk2.exp --dllname libk2.dll
#	dlltool --dllname libk2.dll --output-lib libk2.a

debug:      $(OBJS_DEBUG)
	g++ -g -s -shared -lc -lstdc++ -lpthread -o $(DL_DEBUG) $? -Wl,--entry=_k2_cygwin_main@12;\
	dlltool --dllname $(DL_DEBUG) --output-lib $(DL_DEBUG:%.dll=%.a)

