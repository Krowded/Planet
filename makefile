# Directories
commondir = common/
libdir = lib/

CC=gcc
CFLAGS= -Wall
LDFLAGS= -lXt -lX11 -lGL -DGL_GLEXT_PROTOTYPES -lm	
SOURCES= main.c $(wildcard $(libdir)*.c) $(wildcard $(commondir)*.c) $(wildcard $(commondir)Linux/*.c) $(wildcard $(libdir)kiss_fft/*.c)
INCLUDES= -I$(libdir) -I$(commondir) -I$(commondir)Linux/ -I$(libdir)/kiss_fft
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=project

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(INCLUDES) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: $(EXECUTABLE)

clean :
	rm $(EXECUTABLE)
