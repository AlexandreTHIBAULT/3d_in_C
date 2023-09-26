all:
	make 3d

SUFFIXES: .c .o
.c.o:
	gcc -c $< -o $@

3d: 3d.o libs/utils.o libs/maze/maze.o
	gcc 3d.o libs/utils.o libs/maze/maze.o -o 3d -lglfw  -lGL -lXrandr  -lXi -lXinerama -lX11 -lrt -ldl -lm
	rm *.o libs/*.o libs/maze/*.o

clean:
	rm 3d