.SUFFIXES:
.SUFFIXES: .o .c .as

.as.o:
	$(CC) -c $(CFLAGS) $<
.c.o:
	$(CC) -c $(CFLAGS) $<

CC=htc
CFLAGS=-O  -DUSE_FUNCTIONS # -DDEBUG_FPS # -DDEBUG_RENDERER
TARGET=bigblast
LIBS=-lg -lf
EXTRAS=
#OTHERFILES=./disk/ships.sr5 ./disk/ships.pl5 ./disk/intro5.pl5 ./disk/intro5.sr5
OTHERFILES=./disk/ships.sr5 ./disk/ships.pl5 ./disk/intro.sr5 ./disk/intro.pl5
DOSFILEPATH=./disk/
EMULATOR=openmsx -machine Philips_NMS_8250
EMUDISK=-diska

OBJECTS=bigblast.o \
        renderer.o \
	ship.o \
	ufo.o \
	asteroid.o \
	bullet.o \
	explosio.o \
	object.o \
	controls.o \
	collisio.o \
	font.o \
	menu.o \
	gs2.o

all: emutest

emutest: $(TARGET) dsk
	$(EMULATOR) $(EMUDISK) $(TARGET).dsk

dsk: $(TARGET)
	rm -f $(TARGET).dsk autoexec.bat
	echo $(TARGET) > autoexec.bat
	wrdsk $(TARGET).dsk $(DOSFILEPATH)/msxdos.sys $(DOSFILEPATH)/command.com $(TARGET).com autoexec.bat $(OTHERFILES)


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET).com $(OBJECTS) $(EXTRAS) $(LIBS) 

clean:
	rm -f $(TARGET).com $(OBJECTS) $(TARGET).dsk autoexec.bat

#$(TARGET).o: $(TARGET).c
#object.o:	object.c object.h

