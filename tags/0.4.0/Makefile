SBINDIR = $(DESTDIR)/sbin
ETCDIR = $(DESTDIR)/etc
VERSION = 0.4.0
OBJS = main.o diskset.o disk.o spindown.o iniparser.o dictionary.o log.o spindownd.o\
        exceptions.o
CC = g++
CFLAGS =-O1
SRC = src/
INPARSER = $(SRC)ininiparser3.0b/
SRCDIR = spindown-$(VERSION)

all: spindownd
	@echo "---"
	@echo "THE CONFIGURATION FILE HAS CHANGED SINCE V0.2.1!!!!"
	@echo "Please see the changelog and the example configuration file for more information."
	@echo "---"

clean:
	rm -f $(OBJS) spindownd

install: all
	install -D -m 755 spindownd $(SBINDIR)/spindownd
	install -D -m 755 spindown $(ETCDIR)/init.d/spindown

#make the links in rc.d
	mkdir -p $(ETCDIR)/rc0.d
	mkdir -p $(ETCDIR)/rc1.d
	mkdir -p $(ETCDIR)/rc2.d
	mkdir -p $(ETCDIR)/rc3.d
	mkdir -p $(ETCDIR)/rc4.d
	mkdir -p $(ETCDIR)/rc5.d
	mkdir -p $(ETCDIR)/rc6.d

	ln -s ../init.d/spindown $(ETCDIR)/rc0.d/K20spindown
	ln -s ../init.d/spindown $(ETCDIR)/rc1.d/K20spindown
	ln -s ../init.d/spindown $(ETCDIR)/rc6.d/K20spindown

	ln -s ../init.d/spindown $(ETCDIR)/rc2.d/S20spindown
	ln -s ../init.d/spindown $(ETCDIR)/rc3.d/S20spindown
	ln -s ../init.d/spindown $(ETCDIR)/rc4.d/S20spindown
	ln -s ../init.d/spindown $(ETCDIR)/rc5.d/S20spindown

uninstall:
	rm $(ETCDIR)/spindown.conf $(ETCDIR)/init.d/spindown $(ETCDIR)/spindownd $(ETCDIR)/rc0.d/K20spindown\
		$(ETCDIR)/rc1.d/K20spindown $(ETCDIR)/rc2.d/S20spindown $(ETCDIR)/rc3.d/S20spindown\
		$(ETCDIR)/rc4.d/S20spindown $(ETCDIR)/rc5.d/S20spindown $(ETCDIR)/rc6.d/K20spindown

dist:
	mkdir -p $(SRCDIR)/src/ininiparser3.0b
	mkdir -p $(SRCDIR)/gentoo
	cp $(SRC)general.h $(SRC)main.cpp $(SRC)diskset.h $(SRC)diskset.cpp $(SRC)disk.h\
		$(SRC)disk.cpp $(SRC)spindown.h $(SRC)spindown.cpp $(SRC)log.h $(SRC)log.cpp\
                $(SRC)spindownd.h $(SRC)spindownd.cpp $(SRC)exceptions.h $(SRC)exceptions.cpp\
                $(SRCDIR)/$(SRC)
	cp $(INPARSER)iniparser.c $(INPARSER)dictionary.c $(INPARSER)iniparser.h $(INPARSER)dictionary.h\
		$(SRCDIR)/$(INPARSER)
	cp gentoo/README gentoo/rc.spindown gentoo/init.spindown $(SRCDIR)/gentoo
	cp CHANGELOG README COPYING TODO spindown spindown.conf.example Makefile $(SRCDIR)
	tar -czf $(SRCDIR).tar.gz $(SRCDIR)/*
	rm -d -r -f $(SRCDIR)

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

spindownd.o: $(SRC)spindownd.cpp $(SRC)spindownd.h $(SRC)general.h
	g++ $(CFLAGS) -c $(SRC)spindownd.cpp

log.o: $(SRC)log.cpp $(SRC)log.h $(SRC)general.h
	g++ $(CFLAGS) -c $(SRC)log.cpp

exceptions.o: $(SRC)exceptions.cpp $(SRC)exceptions.h $(SRC)general.h
	g++ $(CFLAGS) -c $(SRC)exceptions.cpp

iniparser.o: $(INPARSER)iniparser.c
	g++ $(CFLAGS) -c $(INPARSER)iniparser.c

dictionary.o: $(INPARSER)dictionary.c
	g++ $(CFLAGS) -c $(INPARSER)dictionary.c
