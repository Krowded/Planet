# Directories
commondir = common/
libdir = lib/

CC=gcc
CFLAGS= 
#-Wall -Wextra
LDFLAGS= -lXt -lX11 -lGL -DGL_GLEXT_PROTOTYPES -lm -lpthread
SOURCES= main.c $(wildcard $(libdir)*.c) $(wildcard $(commondir)*.c) $(wildcard $(commondir)Linux/*.c) $(wildcard $(libdir)kiss_fft/*.c)
INCLUDES= -I$(libdir) -isystem $(commondir) -isystem $(commondir)Linux/ -isystem $(libdir)/kiss_fft
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=project

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(INCLUDES) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: $(EXECUTABLE)

clean :
	rm $(EXECUTABLE)
