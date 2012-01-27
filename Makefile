CC=gcc
CXX=g++

ECHO=$(shell which echo)
RM=$(shell which rm)

CFLAGS=
LDFLAGS=-lxcb

MODULES=mamewm
OBJS=$(addsuffix .o,$(MODULES))
TGT=mamewm.exe

override VERBOSE:=$(if $(VERBOSE),,@)

.PHONY: all
all: $(TGT)
	@$(ECHO) "Build complete!"

mamewm.exe: $(OBJS)
	@$(ECHO) "Linking $@..."
	$(eval override LINKER:=$(if $(IS_CPP),$(CXX),$(CC)))
	$(VERBOSE) $(LINKER) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c %.h
	@$(ECHO) "Compiling $*..."
	$(VERBOSE) $(CC) -c -o $@ $(CFLAGS) $<
%.o: %.cpp %.h
	$(eval IS_CPP=T)
	@$(ECHO) "Compiling $*..."
	$(VERBOSE) $(CXX) -c -o $@ $(CFLAGS) $<

.PHONY: clean
clean:
	@$(ECHO) "Removing: $(OBJS) $(TGT)"
	$(VERBOSE) $(RM) -rf $(OBJS) $(TGT)
