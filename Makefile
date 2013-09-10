CC="gcc"
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=draw.c klicknmenu.c types.h
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) klicknmenu

klicknmenu: $(OBJECTS)
	$(CC) $(LDFLAGS) -lX11 $(OBJECTS) -o $@
.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm *.o klicknmenu
install:
	install -m 755 klicknmenu /usr/bin/klicknmenu
uninstall:
	rm /usr/bin/klicknmenu
