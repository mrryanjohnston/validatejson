#include <stdbool.h>

bool validateJSON(const char *);
bool validateJSONElement(const char *, int, int *, int *);
bool validateArray(const char *, int, int *, int *);
bool validateBoolean(const char *, int, int *, int *);
bool validateNumber(const char *, int, int *, int *);
bool validateObject(const char *, int, int *, int *);
bool validateString(const char *, int, int *, int *);
