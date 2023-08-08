#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validatejson.h"

bool validateObject(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  bool colonIsValid = false;
  bool commaIsValid = false;
  bool keyIsValid = true;
  bool rightBraceIsValid = true;
  bool valueIsValid = false;
  for (; *cursor < length; (*cursor)++)
  {
    switch (jsonString[*cursor])
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        break;
      case '}':
        return rightBraceIsValid;
      case ',':
        if (!commaIsValid) return false;
        commaIsValid = false;
        keyIsValid = true;
        rightBraceIsValid = false;
        break;
      case ':':
        if (!colonIsValid) return false;
        colonIsValid = false;
        valueIsValid = true;
        break;
      case '"':
        if (!keyIsValid) break;
        if (!validateString(jsonString, cursor, length)) return false;
        colonIsValid = true;
        keyIsValid = false;
        rightBraceIsValid = false;
        break;
      default:
        if (
	  !valueIsValid ||
	  !validateJSONElement(jsonString, cursor, length)
        ) return false;
        commaIsValid = true;
        rightBraceIsValid = true;
        valueIsValid = false;
        break;
    }
  }
  return false;
}

bool validateArray(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  bool rightBracketIsValid = true;
  bool commaIsValid = false;
  for (; *cursor < length; (*cursor)++)
  {
    switch (jsonString[*cursor])
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        break;
      case ']':
       return rightBracketIsValid;
      case ',':
        if (!commaIsValid) return false;
        commaIsValid = false;
        rightBracketIsValid = false;
        break;
      default:
        if (
	  (rightBracketIsValid && commaIsValid) ||
          !validateJSONElement(jsonString, cursor, length)
	) return false;
        commaIsValid = true;
        rightBracketIsValid = true;
        break;
    }
  }
  return false;
}

bool validateBoolean(const char *jsonString, int *cursor, int length)
{
  if (
      (*cursor) < length &&
      jsonString[*cursor] == 't' &&
      jsonString[(*cursor) + 1] == 'r' &&
      jsonString[(*cursor) + 2] == 'u' &&
      jsonString[(*cursor) + 3] == 'e')
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

bool validateExponent(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  bool canReturn = false;
  bool numberIsValid = false;
  for (; *cursor < length; (*cursor)++)
  {
    switch (jsonString[*cursor])
    {
      case '+':
      case '-':
	if (numberIsValid) return false;
	numberIsValid = true;
	break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
	if (!numberIsValid) return false;
	canReturn = true;
        break;
      case '}':
      case ']':
      case ',':
      case ' ':
      case '\t':
      case '\r':
      case '\n':
      case '\0':
	(*cursor)--;
	return canReturn;
      default:
	return false;
    }
  }
}

bool validateFraction(const char *jsonString, int *cursor, int length)
{
  (*cursor)++;
  bool canReturn = false;
  bool eIsValid = false;
  for (; *cursor < length; (*cursor)++)
  {
    switch (jsonString[*cursor])
    {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
	eIsValid = true;
	canReturn = true;
        break;
      case 'e':
      case 'E':
	return eIsValid && validateExponent(jsonString, cursor, length);
      case '}':
      case ']':
      case ',':
      case ' ':
      case '\t':
      case '\r':
      case '\n':
      case '\0':
	(*cursor)--;
	return canReturn;
      default:
	return false;
    }
  }
}

bool validateNumber(const char *jsonString, int *cursor, int length)
{
  bool plusMinusIsValid, periodIsValid, eIsValid = false;
  bool numberIsValid = true;
  if (jsonString[*cursor] == '-') (*cursor)++;
  for (; *cursor < length; (*cursor)++)
  {
    switch (jsonString[*cursor])
    {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        if (!numberIsValid) return false;
        eIsValid = true;
        periodIsValid = true;
        break;
      case '.':
	return periodIsValid && validateFraction(jsonString, cursor, length);
      case 'e':
      case 'E':
	return eIsValid && validateExponent(jsonString, cursor, length);
      case '}':
      case ']':
      case ',':
      case ' ':
      case '\t':
      case '\r':
      case '\n':
      case '\0':
	(*cursor)--;
        return true;
      default:
        return false;
    }
  }

  return true;
}

bool validateJSONElement(const char *jsonString, int *cursor, int length)
{
  for (; *cursor < length; (*cursor)++)
  {
    switch (jsonString[*cursor])
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        break;
      case '"':
        return validateString(jsonString, cursor, length);
      case '[':
        return validateArray(jsonString, cursor, length);
      case '{':
        return validateObject(jsonString, cursor, length);
      case 't':
      case 'f':
        return validateBoolean(jsonString, cursor, length);
      default:
        return validateNumber(jsonString, cursor, length);
    }
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
