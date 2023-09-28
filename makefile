CC = gcc
OBJ = 3d.o libs/utils.o libs/maze/maze.o libs/render/render.o libs/color/color.o libs/object/object.o
LDFLAGS = -lglfw -lGL -lXrandr -lXi -lXinerama -lX11 -lrt -ldl -lm

all:
	make 3d

SUFFIXES: .c .o
.c.o:
	$(CC) -c $< -o $@

3d: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm 3d $(OBJ)