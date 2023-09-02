# validatejson

## Description

`validatejson` checks whether the argument passed is a valid string of JSON
as defined by
[RFC 8259](https://datatracker.ietf.org/doc/html/rfc8259).
It takes a lot of cues implementation-wise from
[`jsmn`](https://github.com/zserge/jsmn), but it only validates the passed string.

## Why Another JSON Validator?

I really like the aformentioned `jsmn`'s approach to algorithmically parsing
a string. However, it initializes structs for every complete token parsed.
I wanted something that avoided using `structs` to store references
to the parsed tokens since I only want to validate the string.
I'm hoping this leads to some very fast validations at the cost of
functionality (no ability to reference previously parsed tokens).

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

To validate a string of potential json, pass it as the first argument:

```
$ ./validatejson '{ "foo": [ 1, 2, "Bar!" ] }'
PASS
```

To use the `validateJSON` function in your C code, copy the
`validatejson.h` and `validatejson.c` files into the same directory
as your C file. Here is an example C file that uses the `validateJSON`
function:

```c
#include "validatejson.h"

int main(int argc, char *argv[])
{
	if (argc == 1 && validateJSON(argv[1]))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
```

## Development

Use `make clean` to remove the compiled binary and object headers:

```
$ make clean
rm validatejson validatejson.o
```

If you find a bug, add a file containing valid or invalid JSON
to either the `tests/valid` or `tests/invalid` directories.

## Testing

To compile and run the "test suite:"

```
$ make test
gcc -c validatejson.c
gcc -o tests/test tests/test.c validatejson.o
./tests/test
Test results:
================
PASS
rm ./tests/test validatejson.o
```

## Profiling

Use `make profile` to see 
[the output from `gprof`](https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_chapter/gprof_5.html)
of running the test suite.
