export

PREFIX ?= /usr/share

BASEDIR := $(shell pwd)
BINDIR := $(BASEDIR)/bin
BINARY_NAME := emu68k
SOURCES := $(shell find ./src -name '*.c')
OBJECTS := $(foreach tmp, $(SOURCES:%.c=%.o), $(BINDIR)/$(tmp))

CC = gcc
AR = ar
LD = ld

CCFLAGS := -w -rdynamic -no-pie $(shell pkg-config --cflags --libs gtk+-3.0 vte-2.91) -O3
LDFLAGS := -w -rdynamic -no-pie $(shell pkg-config --libs gtk+-3.0 vte-2.91)

.PHONY: all clean run install uninstall
all: $(OBJECTS) $(BINARY_NAME)
	cp $(BASEDIR)/src/GTK.glade $(BINDIR)

clean:
	mkdir -p $(BINDIR)
	rm $(BINDIR)/* -vfr

run: all
	cd $(BINDIR) && ./$(BINARY_NAME) $(BASEDIR)/gdos.bin && cd $(BASEDIR)

install:
	sudo mkdir -vp $(PREFIX)/emu68k/
	sudo cp $(BINDIR)/$(BINARY_NAME) $(BINDIR)/GTK.glade $(PREFIX)/emu68k
	sudo ln -s $(PREFIX)/emu68k/$(BINARY_NAME) /usr/bin/

uninstall:
	sudo rm -rf $(PREFIX)/emu68k
	sudo rm -f /usr/bin/$(BINARY_NAME)

.SECONDEXPANSION :
$(BINARY_NAME): $(OBJECTS)
	@echo "[LD] Creating final binary"
	@$(CC) $(shell find $(BINDIR) -name '*.o') $(LDFLAGS) -o $(BINDIR)/$@ -pthread

$(OBJECTS): $$(patsubst $$(BINDIR)%.o, $$(BASEDIR)%.c, $$@)
	@echo "[CC] -c $(shell realpath -m --relative-to=$(PWD) $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.c))) -o $(shell realpath -m --relative-to=$(PWD) $(@))"
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -c $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.c)) -o $@ -pthread
