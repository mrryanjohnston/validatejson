#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validatejson.h"

bool skipWhitespace(const char *jsonString, int *cursor, int length)
{
  while (
    *cursor < length && (
      jsonString[*cursor] == ' ' ||
      jsonString[*cursor] == '\t' ||
      jsonString[*cursor] == '\r' ||
      jsonString[*cursor] == '\n'
    )
  )
  {
    (*cursor)++;
  }
  return true;
}

bool validateEndOfObject(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  skipWhitespace(jsonString, cursor, length);
  return jsonString[*cursor] == '}' ||
         jsonString[*cursor] == ',' &&
         (*cursor)++ &&
         skipWhitespace(jsonString, cursor, length) &&
         validateString(jsonString, cursor, length) &&
         (*cursor)++ &&
         skipWhitespace(jsonString, cursor, length) &&
         jsonString[*cursor] == ':' &&
         (*cursor)++ &&
         validateJSONElement(jsonString, cursor, length) &&
         validateEndOfObject(jsonString, cursor, length);
}

bool validateObject(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  skipWhitespace(jsonString, cursor, length);
  return jsonString[*cursor] == '}' ||
         validateString(jsonString, cursor, length) &&
         (*cursor)++ &&
         skipWhitespace(jsonString, cursor, length) &&
         jsonString[*cursor] == ':' &&
         (*cursor)++ &&
         validateJSONElement(jsonString, cursor, length) &&
         validateEndOfObject(jsonString, cursor, length);
}

bool validateEndOfArray(const char *jsonString, int *cursor, int length)
{
  (*cursor)++ &&
  skipWhitespace(jsonString, cursor, length);
  return jsonString[*cursor] == ']' ||
         jsonString[*cursor] == ',' &&
         (*cursor)++ &&
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
  if (
       (*cursor) < length &&
       (
         jsonString[*cursor] == 't' &&
         jsonString[(*cursor) + 1] == 'r' &&
         jsonString[(*cursor) + 2] == 'u' &&
         jsonString[(*cursor) + 3] == 'e') ||
       (
         jsonString[*cursor] == 'n' &&
         jsonString[(*cursor) + 1] == 'u' &&
         jsonString[(*cursor) + 2] == 'l' &&
         jsonString[(*cursor) + 3] == 'l')
     )
  {
    *cursor = (*cursor) + 3;
    return true;
  } else if (
      ((*cursor) + 1) < length &&
      jsonString[*cursor] == 'f' &&
      jsonString[(*cursor) + 1] == 'a' &&
      jsonString[(*cursor) + 2] == 'l' &&
      jsonString[(*cursor) + 3] == 's' &&
      jsonString[(*cursor) + 4] == 'e')
  {
    *cursor = (*cursor) + 4;
    return true;
  }
  return false;
}

bool validateString(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  for (; *cursor < length; (*cursor)++)
  {
    switch (jsonString[*cursor])
    {
      case '"':
        return true;
      case '\\':
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
      default:
        break;
    }
  }
  return false;
}

void skipInteger(const char *jsonString, int *cursor, int length)
{
  while (
    *cursor < length &&
    jsonString[*cursor] >= 48 &&
    jsonString[*cursor] <= 57
  )
  {
    (*cursor)++;
  }
}

bool validateEndOfNumber(const char *jsonString, int *cursor, int length)
{
  return (
    jsonString[*cursor] == '}' ||
    jsonString[*cursor] == ']' ||
    jsonString[*cursor] == ',' ||
    jsonString[*cursor] == ' ' ||
    jsonString[*cursor] == '\t' ||
    jsonString[*cursor] == '\r' ||
    jsonString[*cursor] == '\n' ||
    jsonString[*cursor] == '\0') &&
    (*cursor)--;
}

bool validateExponent(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  if (!(jsonString[*cursor] == '-' || jsonString[*cursor] == '+')) return false;
  (*cursor)++;
  if (!(jsonString[*cursor] >= 48 && jsonString[*cursor] <= 57)) return false;
  skipInteger(jsonString, cursor, length);
  return validateEndOfNumber(jsonString, cursor, length);
}

bool validateFraction(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  if (!(jsonString[*cursor] >= 48 && jsonString[*cursor] <= 57)) return false;
  skipInteger(jsonString, cursor, length);
  if (jsonString[*cursor] == 'e' || jsonString[*cursor] == 'E')
    return validateExponent(jsonString, cursor, length);
  return validateEndOfNumber(jsonString, cursor, length);
}

bool validateNumber(const char *jsonString, int *cursor, int length)
{
  if (jsonString[*cursor] == '-') (*cursor)++;
  skipInteger(jsonString, cursor, length);
  if (jsonString[*cursor] == '.')
    return validateFraction(jsonString, cursor, length);
  if (jsonString[*cursor] == 'e' || jsonString[*cursor] == 'E')
    return validateExponent(jsonString, cursor, length);
  return validateEndOfNumber(jsonString, cursor, length);
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
    default:
      return validateNumber(jsonString, cursor, length);
  }
}

bool validateJSONString(const char *jsonString, int *cursor, int length)
{
  bool elementIsValid = true;
  for (; *cursor < length; (*cursor)++)
  {
    switch (jsonString[*cursor])
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        break;
      default:
       if (
         !elementIsValid ||
         !validateJSONElement(jsonString, cursor, length)
       ) return false;
       elementIsValid = false;
       break;
    }
  }
  return true;  
}

bool validateJSON(const char *jsonString) {
  int cursor = 0;

  return validateJSONString(jsonString, &cursor, strlen(jsonString));
}
