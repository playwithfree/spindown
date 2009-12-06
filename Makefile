CXX = g++
LDFLAGS = -O1
CXXFLAGS = -O1 -c

TARGET =

SRC = src/
INPARSER = $(SRC)iniparser3.0b/
SRCDIR = spindown-$(VERSION)

SBINDIR = $(DESTDIR)/sbin
ETCDIR = $(DESTDIR)/etc

VERSION = 0.5.0
OBJS = $(TARGET)main.o $(TARGET)disk.o $(TARGET)spindown.o $(TARGET)iniparser.o\
		$(TARGET)dictionary.o $(TARGET)log.o $(TARGET)spindownd.o $(TARGET)exceptions.o

all: $(TARGET)spindownd
	@echo "---"
	@echo "THE CONFIGURATION FILE HAS CHANGED SINCE V0.4.0!!!!"
	@echo "Please see the changelog and the example configuration file for more information."
	@echo "---"

debug:
	mkdir -p Debug
	$(MAKE) all TARGET="Debug/" LDFLAGS="$(LDFLAGS) -g" CXXFLAGS="$(CXXFLAGS) -g"

debug-clean:
	$(MAKE) clean TARGET="Debug/"

release:
	mkdir -p Release
	$(MAKE) all TARGET="Release/"

release-clean:
	$(MAKE) clean TARGET="Release/"

clean:
	rm -f $(OBJS) $(TARGET)spindownd

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
	cp $(SRC)general.h $(SRC)main.cpp $(SRC)disk.h\
		$(SRC)disk.cpp $(SRC)spindown.h $(SRC)spindown.cpp $(SRC)log.h $(SRC)log.cpp\
                $(SRC)spindownd.h $(SRC)spindownd.cpp $(SRC)exceptions.h $(SRC)exceptions.cpp\
                $(SRCDIR)/$(SRC)
	cp $(INPARSER)iniparser.c $(INPARSER)dictionary.c $(INPARSER)iniparser.h $(INPARSER)dictionary.h\
		$(SRCDIR)/$(INPARSER)
	cp gentoo/README gentoo/rc.spindown gentoo/init.spindown $(SRCDIR)/gentoo
	cp CHANGELOG README COPYING TODO spindown spindown.conf.example Makefile $(SRCDIR)
	tar -czf $(SRCDIR).tar.gz $(SRCDIR)/*
	rm -d -r -f $(SRCDIR)


$(TARGET)spindownd: $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET)spindownd $(OBJS)

$(TARGET)main.o: $(SRC)main.cpp
	$(CXX) $(CXXFLAGS) $(SRC)main.cpp -o $(TARGET)main.o

$(TARGET)disk.o: $(SRC)disk.cpp $(SRC)disk.h
	$(CXX) $(CXXFLAGS) $(SRC)disk.cpp -o $(TARGET)disk.o

$(TARGET)spindown.o: $(SRC)spindown.cpp $(SRC)spindown.h
	$(CXX) $(CXXFLAGS) $(SRC)spindown.cpp -o $(TARGET)spindown.o

$(TARGET)spindownd.o: $(SRC)spindownd.cpp $(SRC)spindownd.h
	$(CXX) $(CXXFLAGS) $(SRC)spindownd.cpp -o $(TARGET)spindownd.o

$(TARGET)log.o: $(SRC)log.cpp $(SRC)log.h
	$(CXX) $(CXXFLAGS) $(SRC)log.cpp -o $(TARGET)log.o

$(TARGET)exceptions.o: $(SRC)exceptions.cpp $(SRC)exceptions.h
	$(CXX) $(CXXFLAGS) $(SRC)exceptions.cpp -o $(TARGET)exceptions.o

$(TARGET)iniparser.o: $(INPARSER)iniparser.c
	$(CXX) $(CXXFLAGS) $(INPARSER)iniparser.c -o $(TARGET)iniparser.o

$(TARGET)dictionary.o: $(INPARSER)dictionary.c
	$(CXX) $(CXXFLAGS) $(INPARSER)dictionary.c -o $(TARGET)dictionary.o
