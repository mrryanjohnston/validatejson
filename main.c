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
    if (!validateJSON(argv[1]))
    {
      printf("ERROR: %s is invalid!\n", argv[1]);
      return -1;
    }
    printf("PASS\n");
    return 0;
  }
  return -1;
}
