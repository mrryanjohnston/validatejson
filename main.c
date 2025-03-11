#include <stdio.h>
#include "validatejson.h"

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    printf("USAGE\nvalidatejson checks whether the argument passed is a valid string of JSON\n\nExample:\n\tvalidatejson '{ \"foo\": [ 1, 2, \"Bar!\" ] }'\n");
    return 0;
  }
  else if (argc == 2)
  {
    const char *current = argv[1];
    if (!validateJSON(&current))
    {
      int line = 1;
      int character = 0;
      printf("FAIL\nInvalid JSON found near: %s\n\n", current);
      while (current != argv[1] && *current != '\n') { character++; current--; }
      printf("Check json near character number %d", character);
      while (current != argv[1]) { *current == '\n' && line++; current--; }
      printf(" on line %d\n", line);
      return -1;
    }
    printf("PASS\n");
    return 0;
  }
  return -1;
}
