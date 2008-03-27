sbindir = $(DESTDIR)/sbin
etcdir = $(DESTDIR)/etc
OBJS = disk.o thread.o spindown.o iniparser.o dictionary.o
CC = g++
CFLAGS =-O1 -pthread
SRC = src/
INPARSER = $(SRC)ininiparser3.0b/

all: $(OBJS)
	g++ $(CFLAGS) -o spindownd $(SRC)main.cpp $(OBJS)

clean:
	rm $(OBJS) spindownd

install: all
	install -D -m 755 spindownd $(sbindir)/spindownd
#install --backup -D -m 644 spindown.conf $(etcdir)/spindown.conf
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

disk.o: $(SRC)disk.cpp
	g++ $(CFLAGS) -c $(SRC)disk.cpp
	
spindown.o: $(SRC)spindown.cpp
	g++ $(CFLAGS) -c $(SRC)spindown.cpp
	
iniparser.o: $(INPARSER)iniparser.c
	g++ $(CFLAGS) -c $(INPARSER)iniparser.c
	
dictionary.o: $(INPARSER)dictionary.c
	g++ $(CFLAGS) -c $(INPARSER)dictionary.c

thread.o: $(SRC)thread.cpp
	g++ $(CFLAGS) -c $(SRC)thread.cpp