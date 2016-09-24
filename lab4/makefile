# set this variable to the director in which you saved the common files
commondir = ../common/

all: $(commondir)LoadTGA.c SpriteLight.c lab4.c $(commondir)VectorUtils3.c $(commondir)GL_utilities.c $(commondir)Linux/MicroGlut.c
	gcc lab4.c ../common/*.c ../common/Mac/MicroGlut.m -o lab4 -framework OpenGL -framework Cocoa SpriteLight.c -I../common/Mac -I../common

clean:
	rm -f lab4

