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
BINARY_NAME := emu68k
SOURCES := $(shell find ./src -name '*.c')
SOURCES_CXX := $(shell find ./src -name '*.cpp')
OBJECTS := $(foreach tmp, $(SOURCES:%.c=%.o), $(BINDIR)/$(tmp))
OBJECTS_CXX := $(foreach tmp, $(SOURCES_CXX:%.cpp=%.o), $(BINDIR)/$(tmp))

CFLAGS := -rdynamic -fPIE -w $(shell pkg-config --cflags --libs glibmm-2.4 gtk+-3.0 vte-2.91)
CXXFLAGS := -xc++ -rdynamic -fPIE -w $(shell pkg-config --cflags-only-I --libs glibmm-2.4 gtk+-3.0 vte-2.91)
LDFLAGS := -w -rdynamic -fwhole-program $(shell pkg-config --libs glibmm-2.4 gtk+-3.0 vte-2.91)

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

.PHONY: all clean test install uninstall
all: $(OBJECTS_CXX) $(OBJECTS) $(BINARY_NAME)
	cp $(BASEDIR)/src/GTK.glade $(BINDIR)

.SECONDEXPANSION :
$(BINARY_NAME): $(OBJECTS_CXX) $(OBJECTS)
	@echo "[LD] Creating final binary"
	@$(CC) $(shell find $(BINDIR) -name '*.o') $(LDFLAGS) -o $(BINDIR)/$@ -lstdc++ -pthread

$(OBJECTS): $$(patsubst $$(BINDIR)%.o, $$(BASEDIR)%.c, $$@)
	@echo "[CC] -c $(shell realpath -m --relative-to=$(PWD) $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.c))) -o $(shell realpath -m --relative-to=$(PWD) $(@))"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.c)) -o $@ -pthread

$(OBJECTS_CXX): $$(patsubst $$(BINDIR)%.o, $$(BASEDIR)%.cpp, $$@)
	@echo "[CPP] -c $(shell realpath -m --relative-to=$(PWD) $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.cpp))) -o $(shell realpath -m --relative-to=$(PWD) $(@))"
	@mkdir -p $(dir $@)
	@$(CC) $(CXXFLAGS) -c $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.cpp)) -o $@ -pthread

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

clean:
	mkdir -p $(BINDIR)
	rm $(BINDIR)/* -vfr

test: $(BINARY_NAME)
	cd $(BINDIR) && ./$(BINARY_NAME) -t 1000000 -r $(BASEDIR)/test/gdos.bin && cd $(BASEDIR)

install:
	sudo mkdir -vp $(PREFIX)/emu68k/
	sudo cp $(BINDIR)/$(BINARY_NAME) $(BINDIR)/GTK.glade $(PREFIX)/emu68k
	sudo ln -s $(PREFIX)/emu68k/$(BINARY_NAME) /usr/bin/

uninstall:
	sudo rm -rf $(PREFIX)/emu68k
	sudo rm -f /usr/bin/$(BINARY_NAME)
