# surf - simple browser
# See LICENSE file for copyright and license details.
.POSIX:

include config.mk

SRC = src/surf.c
CSRC = src/common.c
WEBEXTSRC = src/libsurf-webext.c
OBJ = $(patsubst src/%.c,build/%.o,$(SRC))
COBJ = $(patsubst src/%.c,build/%.o,$(CSRC))
WEBEXTOBJ = $(patsubst src/%.c,build/%.o,$(WEBEXTSRC))

all: options build/libsurf-webext.so build/surf

options:
	@echo surf build options:
	@echo "CC            = $(CC)"
	@echo "CFLAGS        = $(SURFCFLAGS) $(CFLAGS)"
	@echo "WEBEXTCFLAGS  = $(WEBEXTCFLAGS) $(CFLAGS)"
	@echo "LDFLAGS       = $(LDFLAGS)"

build/%.o: src/%.c
	$(CC) $(SURFCFLAGS) $(CFLAGS) -c $< -o $@

src/config.h: config.def.h
	cp config.def.h $@

$(OBJ): src/config.h src/common.h config.mk
$(COBJ): src/config.h src/common.h config.mk
$(WEBEXTOBJ): src/config.h src/common.h config.mk

$(WEBEXTOBJ): $(WEBEXTSRC)
	$(CC) $(WEBEXTCFLAGS) $(CFLAGS) -c $(WEBEXTSRC) -o $@

build/libsurf-webext.so: $(WEBEXTOBJ) $(COBJ)
	$(CC) -shared -Wl,-soname,$@ $(LDFLAGS) -o $@ \
	    $(WEBEXTOBJ) $(COBJ) $(WEBEXTLIBS)

build/surf: $(OBJ) $(COBJ)
	$(CC) $(SURFLDFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(COBJ) $(LIBS)

clean:
	rm -f build/*

distclean: clean
	rm -f src/config.h build/surf-$(VERSION).tar.gz

dist: distclean
	mkdir -p surf-$(VERSION)
	cp -R LICENSE Makefile config.mk config.def.h README \
	    surf-open.sh src TODO.md surf.png \
	    surf.1 surf-$(VERSION)
	tar -cf surf-$(VERSION).tar surf-$(VERSION)
	gzip surf-$(VERSION).tar
	rm -rf surf-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f build/surf $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/surf
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp -f build/libsurf-webext.so $(DESTDIR)$(LIBDIR)
	chmod 644 $(DESTDIR)$(LIBDIR)/libsurf-webext.so
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < surf.1 > $(DESTDIR)$(MANPREFIX)/man1/surf.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/surf.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/surf
	rm -f $(DESTDIR)$(MANPREFIX)/man1/surf.1
	rm -f $(DESTDIR)$(LIBDIR)/libsurf-webext.so
	- rmdir $(DESTDIR)$(LIBDIR)

.SUFFIXES: .so .o .c
.PHONY: all options clean-dist clean dist install uninstall
