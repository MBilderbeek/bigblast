CC=htc
CFLAGS=-O # -DDEBUG_RENDERER
TARGET=bigblast
LIBS=-lg -lf
EXTRAS=loadgrp.o
OTHERFILES=SHIPS.COP
DOSFILEPATH=./disk
EMULATOR=openmsx
EMUDISK=-diska

OBJECTS=bigblast.o \
        renderer.o \
	ship.o \
	asteroid.o \
	bullet.o \
	object.o \
	controls.o \
	collisio.o

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

$(TARGET).o: $(TARGET).c 
object.o:	object.c object.h

