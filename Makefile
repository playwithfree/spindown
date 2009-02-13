sbindir = $(DESTDIR)/sbin
etcdir = $(DESTDIR)/etc
VERSION = 0.3.0
OBJS = main.o diskset.o disk.o spindown.o iniparser.o dictionary.o log.o
CC = g++
CFLAGS =-O1 -pthread
SRC = src/
INPARSER = $(SRC)ininiparser3.0b/

all: spindownd
	@echo "---"
	@echo "THE CONFIGURATION FILE HAS CHANGED SINCE V0.2.1!!!!"
	@echo "Please see the changelog and the example configuration file for more information."
	@echo "---"

clean:
	rm -f $(OBJS) spindownd

install: all
	install -D -m 755 spindownd $(sbindir)/spindownd
	install -D -m 755 spindown $(etcdir)/init.d/spindown

#make the links in rc.d
	mkdir -p $(etcdir)/rc0.d
	mkdir -p $(etcdir)/rc1.d
	mkdir -p $(etcdir)/rc2.d
	mkdir -p $(etcdir)/rc3.d
	mkdir -p $(etcdir)/rc4.d
	mkdir -p $(etcdir)/rc5.d
	mkdir -p $(etcdir)/rc6.d

	ln -s ../init.d/spindown $(etcdir)/rc0.d/K20spindown
	ln -s ../init.d/spindown $(etcdir)/rc1.d/K20spindown
	ln -s ../init.d/spindown $(etcdir)/rc6.d/K20spindown

	ln -s ../init.d/spindown $(etcdir)/rc2.d/S20spindown
	ln -s ../init.d/spindown $(etcdir)/rc3.d/S20spindown
	ln -s ../init.d/spindown $(etcdir)/rc4.d/S20spindown
	ln -s ../init.d/spindown $(etcdir)/rc5.d/S20spindown

uninstall:
	rm $(etcdir)/spindown.conf $(etcdir)/init.d/spindown $(etcdir)/spindownd $(etcdir)/rc0.d/K20spindown\
		$(etcdir)/rc1.d/K20spindown $(etcdir)/rc2.d/S20spindown $(etcdir)/rc3.d/S20spindown\
		$(etcdir)/rc4.d/S20spindown $(etcdir)/rc5.d/S20spindown $(etcdir)/rc6.d/K20spindown

dist:
	DESTDIR=spindown-$(VERSION) make install
	tar -czf spindown-$(VERSION).tar.gz -C spindown-$(VERSION) .
	rm -d -r -f spindown-$(VERSION)

spindownd: $(OBJS)
	g++ $(CFLAGS) -o spindownd $(OBJS)

main.o: $(SRC)main.cpp
	g++ $(CFLAGS) -c $(SRC)main.cpp

diskset.o: $(SRC)diskset.cpp
	g++ $(CFLAGS) -c $(SRC)diskset.cpp

disk.o: $(SRC)disk.cpp
	g++ $(CFLAGS) -c $(SRC)disk.cpp

spindown.o: $(SRC)spindown.cpp
	g++ $(CFLAGS) -c $(SRC)spindown.cpp

iniparser.o: $(INPARSER)iniparser.c
	g++ $(CFLAGS) -c $(INPARSER)iniparser.c

dictionary.o: $(INPARSER)dictionary.c
	g++ $(CFLAGS) -c $(INPARSER)dictionary.c

log.o: $(SRC)log.cpp
	g++ $(CFLAGS) -c $(SRC)log.cpp
