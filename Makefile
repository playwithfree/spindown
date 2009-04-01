sbindir = $(DESTDIR)/sbin
etcdir = $(DESTDIR)/etc
VERSION = 0.3.1
OBJS = main.o diskset.o disk.o spindown.o iniparser.o dictionary.o log.o
CC = g++
CFLAGS =-O1
SRC = src/
INPARSER = $(SRC)ininiparser3.0b/
srcdir = spindown-$(VERSION)

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
	mkdir -p $(srcdir)/src/ininiparser3.0b
	mkdir -p $(srcdir)/gentoo
	cp $(SRC)general.h $(SRC)main.cpp $(SRC)diskset.h $(SRC)diskset.cpp $(SRC)disk.h\
		$(SRC)disk.cpp $(SRC)spindown.h $(SRC)spindown.cpp $(SRC)log.h $(SRC)log.cpp $(srcdir)/$(SRC)
	cp $(INPARSER)iniparser.c $(INPARSER)dictionary.c $(INPARSER)iniparser.h $(INPARSER)dictionary.h\
		$(srcdir)/$(INPARSER)
	cp gentoo/README gentoo/rc.spindown gentoo/init.spindown $(srcdir)/gentoo
	cp CHANGELOG README COPYING TODO spindown spindown.conf.example Makefile $(srcdir)
	tar -czf $(srcdir).tar.gz $(srcdir)/*
	rm -d -r -f $(srcdir)

spindownd: $(OBJS)
	g++ $(CFLAGS) -o spindownd $(OBJS)

main.o: $(SRC)main.cpp $(SRC)general.h
	g++ $(CFLAGS) -c $(SRC)main.cpp

diskset.o: $(SRC)diskset.cpp $(SRC)diskset.h $(SRC)general.h
	g++ $(CFLAGS) -c $(SRC)diskset.cpp

disk.o: $(SRC)disk.cpp $(SRC)disk.h $(SRC)general.h
	g++ $(CFLAGS) -c $(SRC)disk.cpp

spindown.o: $(SRC)spindown.cpp $(SRC)spindown.h $(SRC)general.h
	g++ $(CFLAGS) -c $(SRC)spindown.cpp

log.o: $(SRC)log.cpp $(SRC)log.h $(SRC)general.h
	g++ $(CFLAGS) -c $(SRC)log.cpp

iniparser.o: $(INPARSER)iniparser.c
	g++ $(CFLAGS) -c $(INPARSER)iniparser.c

dictionary.o: $(INPARSER)dictionary.c
	g++ $(CFLAGS) -c $(INPARSER)dictionary.c
