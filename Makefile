mysh: shell.c interpreter.c shellmemory.c framestore.c pagetable.c
	gcc -DVARIABLE_STORE_SIZE=$(varmemsize) -DFRAME_STORE_SIZE=$(framesize) -c -g shell.c interpreter.c shellmemory.c framestore.c pagetable.c
	gcc -o mysh shell.o interpreter.o shellmemory.o framestore.o pagetable.o

clean: 
	rm mysh; rm *.o
