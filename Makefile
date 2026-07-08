all:
	gcc -g src/main.c -o server

run:
	./server

clean:
	rm -f server
