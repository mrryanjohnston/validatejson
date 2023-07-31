all: validatejson
	gcc -o validatejson main.c validatejson.o
validatejson:
	gcc -c -o validatejson.o validatejson.c
test: validatejson
	gcc -o tests/test tests/test.c validatejson.o
	./tests/test
	rm ./tests/test
clean:
	rm validatejson validatejson.o
