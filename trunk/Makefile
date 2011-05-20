CC = g++
CFLAGS = -Wall
LIBS = -lglut -lGLU -lGL -lm -lglui
OBJS = main.o mesh.o camera.o vector3f.o matrix4x4f.o close2gl.o

all: ctgsl

ctgsl: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^
