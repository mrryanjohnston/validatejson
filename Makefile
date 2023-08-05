all: validatejson.o
	gcc -o validatejson main.c validatejson.o
test: validatejson.o
	gcc -o tests/test tests/test.c validatejson.o
	./tests/test
	rm ./tests/test
validatejson.o:
	gcc -c validatejson.c
clean:
	rm validatejson.o
	rm validatejson
