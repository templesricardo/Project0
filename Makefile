all:
	gcc -o tucat tucat.c -Wall -Werror

clean:
	rm -f tucat