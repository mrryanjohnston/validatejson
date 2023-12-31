#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validatejson.h"

bool validateCharAndAdvanceCursor(const char *jsonString, int *cursor, char c)
{
  return jsonString[*cursor] == c &&
         ++(*cursor);
}

bool skipWhitespace(const char *jsonString, int *cursor, int length)
{
  while (
    *cursor < length && (
      validateCharAndAdvanceCursor(jsonString, cursor, ' ')  ||
      validateCharAndAdvanceCursor(jsonString, cursor, '\t') ||
      validateCharAndAdvanceCursor(jsonString, cursor, '\r') ||
      validateCharAndAdvanceCursor(jsonString, cursor, '\n')
    )
  );
  return true;
}

bool validateEndOfObject(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  skipWhitespace(jsonString, cursor, length);
  return jsonString[*cursor] == '}' ||
         validateCharAndAdvanceCursor(jsonString, cursor, ',') &&
         skipWhitespace(jsonString, cursor, length) &&
         jsonString[*cursor] == '"' &&
         validateString(jsonString, cursor, length) &&
         (*cursor)++ &&
         skipWhitespace(jsonString, cursor, length) &&
         validateCharAndAdvanceCursor(jsonString, cursor, ':') &&
         validateJSONElement(jsonString, cursor, length) &&
         validateEndOfObject(jsonString, cursor, length);
}

bool validateObject(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  skipWhitespace(jsonString, cursor, length);
  return jsonString[*cursor] == '}' ||
         jsonString[*cursor] == '"' &&
         validateString(jsonString, cursor, length) &&
         (*cursor)++ &&
         skipWhitespace(jsonString, cursor, length) &&
         validateCharAndAdvanceCursor(jsonString, cursor, ':') &&
         validateJSONElement(jsonString, cursor, length) &&
         validateEndOfObject(jsonString, cursor, length);
}

bool validateEndOfArray(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  skipWhitespace(jsonString, cursor, length);
  return jsonString[*cursor] == ']' ||
         validateCharAndAdvanceCursor(jsonString, cursor, ',') &&
         validateJSONElement(jsonString, cursor, length) &&
         validateEndOfArray(jsonString, cursor, length);
}

bool validateArray(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  skipWhitespace(jsonString, cursor, length);
  return jsonString[*cursor] == ']' ||
         validateJSONElement(jsonString, cursor, length) &&
         validateEndOfArray(jsonString, cursor, length);
}

bool validateBoolean(const char *jsonString, int *cursor, int length)
{
  return (
      strncmp(jsonString + (*cursor), "true", 4) == 0 ||
      strncmp(jsonString + (*cursor), "null", 4) == 0
    ) &&
    (*cursor = (*cursor) + 3) ||
    strncmp(jsonString + (*cursor), "false", 5) == 0 &&
    (*cursor = (*cursor) + 4);
}

bool validateString(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  while (
    *cursor < length &&
    jsonString[*cursor] != '"'
  )
  {
    if (jsonString[*cursor] == '\\')
    {
      (*cursor)++;
      if (jsonString[*cursor] == 'u')
      {
        if ((*cursor) + 4 > length) return false;
        // From https://github.com/zserge/jsmn/blob/25647e692c7906b96ffd2b05ca54c097948e879c/jsmn.h#L241-L251
        for (int x = 0; x < 4; (*cursor)++ && x++)
        {
          int c = jsonString[(*cursor) + 1];
          if (!(
               (c >= 48 && c <= 57) || /* 0-9 */
               (c >= 65 && c <= 70) || /* A-F */
               (c >= 97 && c <= 102)   /* a-f */
          )) return false;
        }
      }
    }
    (*cursor)++;
  }
  return jsonString[*cursor] == '"';
}

bool validateAtLeastOneInteger(const char *jsonString, int *cursor, int length)
{
  if (
    jsonString[*cursor] < 48 ||
    jsonString[*cursor] > 57
  ) return false;
  do (*cursor)++;
  while (
    *cursor < length &&
    jsonString[*cursor] >= 48 &&
    jsonString[*cursor] <= 57
  );
  return true;
}

bool validateExponent(const char *jsonString, int *cursor, int length)
{
  return (
           jsonString[*cursor] != 'e' &&
           jsonString[*cursor] != 'E'
         ) ||
         (*cursor)++ &&
         (
           validateCharAndAdvanceCursor(jsonString, cursor, '-') ||
           validateCharAndAdvanceCursor(jsonString, cursor, '+') ||
           true
         ) &&
         validateAtLeastOneInteger(jsonString, cursor, length);
}

bool validateFraction(const char *jsonString, int *cursor, int length)
{
  return jsonString[*cursor] != '.' ||
         (*cursor)++ &&
         validateAtLeastOneInteger(jsonString, cursor, length);
}

bool validateNumber(const char *jsonString, int *cursor, int length)
{
  return validateAtLeastOneInteger(jsonString, cursor, length) &&
         validateFraction(jsonString, cursor, length) &&
         validateExponent(jsonString, cursor, length) &&
         (
           jsonString[*cursor] == '}' ||
           jsonString[*cursor] == ']' ||
           jsonString[*cursor] == ',' ||
           jsonString[*cursor] == ' ' ||
           jsonString[*cursor] == '\t' ||
           jsonString[*cursor] == '\r' ||
           jsonString[*cursor] == '\n' ||
           jsonString[*cursor] == '\0'
         ) &&
         (*cursor)--;
}

bool validateJSONElement(const char *jsonString, int *cursor, int length)
{
  skipWhitespace(jsonString, cursor, length);
  switch (jsonString[*cursor])
  {
    case '"':
      return validateString(jsonString, cursor, length);
    case '[':
      return validateArray(jsonString, cursor, length);
    case '{':
      return validateObject(jsonString, cursor, length);
    case 't':
    case 'f':
    case 'n':
      return validateBoolean(jsonString, cursor, length);
    case '-':
      (*cursor)++;
    default:
      return validateNumber(jsonString, cursor, length);
  }
}

bool validateJSONString(const char *jsonString, int *cursor, int length)
{
  return validateJSONElement(jsonString, cursor, length) &&
         (
           (*cursor) == length ||
           ++(*cursor) &&
           skipWhitespace(jsonString, cursor, length) &&
           (*cursor) == length
         );
}

bool validateJSON(const char *jsonString) {
  int cursor = 0;

  return validateJSONString(jsonString, &cursor, strlen(jsonString));
}
