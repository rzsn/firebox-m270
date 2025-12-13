# TODO:
#  - add a build directory with .o files
#  - add a output directory with executables

CC=gcc
LDIR=libs
ARCH=`uname -m`
DEBUG=-ggdb -O0
INCLUDES=
#-I$(LDIR)/libsomething/
CFLAGS= $(DEBUG) -Wall -std=gnu99
CFLAGS= -Ofast -Wall -std=gnu99
LFLAGS= -L$(LDIR)/$(ARCH)/
LIBS=
#LIBS=-lm -lsomething
LDFLAGS= $(LIBS)
SOURCES=afpc.c
OBJECTS= $(SOURCES:.c=.o)
EXECUTABLE=afpc
BUILDDIR := .
INSTALLDIR= /usr/sbin

.PHONY: clean target

all: target strip

target: $(EXECUTABLE)

strip: $(EXECUTABLE)
	strip $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXECUTABLE) $(OBJECTS) $(LFLAGS) $(LIBS)
# -Wl,-rpath=./$(LDIR)/$(ARCH)/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f *.o
	rm -f $(EXECUTABLE)

help: $(EXECUTABLE)
	./$(EXECUTABLE) --help

install_copy:
	cp $(EXECUTABLE) $(INSTALLDIR)/$(EXECUTABLE)

install_link:
# https://unix.stackexchange.com/questions/458861/how-to-create-a-link-ln-s-with-an-absolute-path
	ln -s "$(realpath $(EXECUTABLE))" $(INSTALLDIR)/$(EXECUTABLE)

uninstall:
	rm $(INSTALLDIR)/$(EXECUTABLE)

