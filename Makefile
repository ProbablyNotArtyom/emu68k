#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#
#	emu68k v0.7
#	NotArtyom
#	27/03/21
#
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

export

PREFIX ?= /usr/share

CC = g++
AR = ar
LD = ld

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

BASEDIR := $(shell pwd)
BINDIR := $(BASEDIR)/bin
OBJDIR := $(BASEDIR)/obj
SRCDIR := $(BASEDIR)/src
RESDIR := $(BASEDIR)/res
BINARY_NAME := emu68k

SOURCES := $(shell find ./src -name '*.c')
SOURCES_CXX := $(shell find ./src -name '*.cpp')
OBJECTS := $(foreach tmp, $(SOURCES:%.c=%.o), $(OBJDIR)/$(tmp))
OBJECTS_CXX := $(foreach tmp, $(SOURCES_CXX:%.cpp=%.o), $(OBJDIR)/$(tmp))

CFLAGS := -rdynamic -fPIE -w $(shell pkg-config --cflags --libs glibmm-2.4 gtk+-3.0 vte-2.91 pango)
CXXFLAGS := -xc++ -rdynamic -fPIE -w $(shell pkg-config --cflags-only-I --libs glibmm-2.4 gtk+-3.0 vte-2.91 pango)
LDFLAGS := -w -rdynamic -fwhole-program $(shell pkg-config --libs glibmm-2.4 gtk+-3.0 vte-2.91 pango)

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

.PHONY: all clean test install uninstall
all: $(OBJECTS_CXX) $(OBJECTS) $(BINARY_NAME)
	@echo "[--] Copying resources"
	@cp -R $(RESDIR) $(BINDIR)
	@echo "[--] cp -R $(RESDIR) $(BINDIR)"

.SECONDEXPANSION :
$(BINARY_NAME): $(OBJECTS_CXX) $(OBJECTS)
	@echo "[LD] Creating final binary"
	@mkdir -p $(BINDIR)
	@$(CC) $(shell find $(OBJDIR) -name '*.o') $(LDFLAGS) -o $(BINDIR)/$@ -lstdc++ -pthread

$(OBJECTS): $$(patsubst $$(OBJDIR)%.o, $$(BASEDIR)%.c, $$@)
	@echo "[CC] -c $(shell realpath -m --relative-to=$(PWD) $(patsubst $(OBJDIR)%, $(BASEDIR)%, $(@:%.o=%.c))) -o $(shell realpath -m --relative-to=$(PWD) $(@))"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $(patsubst $(OBJDIR)%, $(BASEDIR)%, $(@:%.o=%.c)) -o $@ -pthread

$(OBJECTS_CXX): $$(patsubst $$(OBJDIR)%.o, $$(BASEDIR)%.cpp, $$@)
	@echo "[CPP] -c $(shell realpath -m --relative-to=$(PWD) $(patsubst $(OBJDIR)%, $(BASEDIR)%, $(@:%.o=%.cpp))) -o $(shell realpath -m --relative-to=$(PWD) $(@))"
	@mkdir -p $(dir $@)
	@$(CC) $(CXXFLAGS) -c $(patsubst $(OBJDIR)%, $(BASEDIR)%, $(@:%.o=%.cpp)) -o $@ -pthread

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

clean:
	rm $(OBJDIR) -vfr
	rm $(BINDIR) -vfr

test: $(BINARY_NAME)
	$(BINDIR)/$(BINARY_NAME) -d -t 1000000 -r $(BASEDIR)/test/gdos.bin

install:
	mkdir -vp $(PREFIX)/emu68k/
	cp -R $(BINDIR)/* $(PREFIX)/emu68k
	ln -s $(PREFIX)/emu68k/$(BINARY_NAME) /usr/bin/$(BINARY_NAME)

uninstall:
	rm -rf $(PREFIX)/emu68k
	rm -f /usr/bin/$(BINARY_NAME)
