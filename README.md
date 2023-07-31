# validatejson

## Description

`validatejson` checks whether the argument passed is a valid string of JSON

## Usage

Running `./validatejson` with no arguments will
display the USAGE text:

```
$ ./validatejson 
USAGE
validatejson checks whether the argument passed is a valid string of JSON

Example:
        validatejson '{ "foo": [ 1, 2, "Bar!" ] }'
```

To validate a string of potention json, pass it as the first argument:

```
$ make
gcc -c -o validatejson.o validatejson.c
gcc -o validatejson main.c validatejson.o

$ ./validatejson '{ "foo": [ 1, 2, "Bar!" ] }'
PASS
```

## Development

Use `make clean` to remove the compiled binary and object headers:

```
$ make clean
rm validatejson validatejson.o
```

## Testing

To compile and run the "test suite:"

```
$ make test
gcc -c -o validatejson.o validatejson.c
gcc -o tests/test tests/test.c validatejson.o
./tests/test
Test results:
================
PASS
rm ./tests/test
```

Add examples of valid or invalid JSON
by creating a file in either the `tests/valid`
or `tests/invalid` directories.
