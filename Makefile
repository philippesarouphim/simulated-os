mysh: shell.c interpreter.c shellmemory.c
	gcc -DVARIABLE_STORE_SIZE=$(varmemsize) -DFRAME_STORE_SIZE=$(framesize) -c -g shell.c interpreter.c shellmemory.c
	gcc -o mysh shell.o interpreter.o shellmemory.o

clean: 
	rm mysh; rm *.o
