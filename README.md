# validatejson

## Description

`validatejson` checks whether the argument passed is a valid string of JSON
as defined by
[RFC 8259](https://datatracker.ietf.org/doc/html/rfc8259).
It takes a lot of cues implementation-wise from
[`jsmn`](https://github.com/zserge/jsmn), but it only validates the passed string.

## Why Another JSON Validator?

I really like the aformentioned `jsmn`'s approach to algorithmically parsing
a string. I wanted something that avoided using `structs` to store references
to the parsed tokens. I'm hoping this leads to some very fast validations.

## Usage

Use `make` to compile the `validatejson` binary:

```
$ make
gcc -c validatejson.c
gcc -o validatejson main.c validatejson.o
```

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
