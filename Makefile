make:
	gcc -o mono-os mono-os.c -fPIE -L./ -larchsim -Wall -Wextra -pedantic -g 

clean:
	rm mono-os *.o *~
