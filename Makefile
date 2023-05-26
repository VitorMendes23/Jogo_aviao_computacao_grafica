executar: compilar
	./programa

compilar:
	gcc SOIL/*.c -c 
	gcc *.c -c -g
	gcc *.o -o programa -lm -lopengl32 -lfreeglut -lglew32 -lglu32

limpar:
	del -rf programa *.o
	del -rf programa *.exe
