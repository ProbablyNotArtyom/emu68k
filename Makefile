export

BASEDIR := $(PWD)
BINDIR := $(BASEDIR)/bin
BINARY_NAME := emu68k
SOURCES := $(shell find ./src -name '*.c')
OBJECTS := $(foreach tmp, $(SOURCES:%.c=%.o), $(BINDIR)/$(tmp))

CC = gcc
AR = ar
LD = ld

CCFLAGS := -w -rdynamic -no-pie `pkg-config --cflags --libs gtk+-3.0 vte-2.91`
LDFLAGS := -w -rdynamic -no-pie `pkg-config --libs gtk+-3.0 vte-2.91`

.PHONY: all clean run
all: $(OBJECTS) $(BINARY_NAME)
	cp $(BASEDIR)/src/GTK.glade $(BINDIR)

clean:
	mkdir -p $(BINDIR)
	rm $(BINDIR)/* -vfr

run: all
	cd $(BINDIR) && ./$(BINARY_NAME) $(BASEDIR)/gdos.bin && cd $(BASEDIR)

.SECONDEXPANSION :
$(BINARY_NAME): $(OBJECTS)
	@echo "[LD] Creating final binary"
	@$(CC) $(shell find $(BINDIR) -name '*.o') $(LDFLAGS) -o $(BINDIR)/$@

$(OBJECTS): $$(patsubst $$(BINDIR)%.o, $$(BASEDIR)%.c, $$@)
	@echo "[CC] -c $(shell realpath -m --relative-to=$(PWD) $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.c))) -o $(shell realpath -m --relative-to=$(PWD) $(@))"
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -c $(patsubst $(BINDIR)%, $(BASEDIR)%, $(@:%.o=%.c)) -o $@
