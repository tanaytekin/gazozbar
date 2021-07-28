PREFIX ?= /usr/local


gazozbar: gazozbar.o gazozbar.c
	gcc -pedantic -std=gnu90 -Wall gazozbar.c -o gazozbar -lX11

clean:
	rm -rf gazozbar

install: gazozbar
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 gazozbar $(DESTDIR)$(PREFIX)/bin

uninstall: 
	rm -rf $(DESTDIR)$(PREFIX)/bin/gazozbar

