#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "validatejson.h"

bool validateObject(const char *jsonString, int length, int *start, int *end)
{
  int innerStart, innerEnd;
  bool colonIsValid = false;
  bool commaIsValid = false;
  bool keyIsValid = true;
  bool rightBraceIsValid = true;
  bool valueIsValid = false;
  for (; *end < length; (*end)++)
  {
    switch (jsonString[*end])
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        break;
      case '}':
        if (rightBraceIsValid)
        {
          return true;
        }
        return false;
      case ',':
        if (!commaIsValid)
        {
          return false;
        }
        commaIsValid = false;
        keyIsValid = true;
        rightBraceIsValid = false;
        break;
      case ':':
        if (!colonIsValid) {
          return false;
        }
        colonIsValid = false;
        valueIsValid = true;
        break;
      case '"':
        if (keyIsValid) {
          innerStart = innerEnd = (*end) + 1;
          if (!validateString(jsonString, length, &innerStart, &innerEnd))
          {
            return false;
          }
          *end = innerEnd;
          colonIsValid = true;
          keyIsValid = false;
          break;
        }
      default:
        if (!valueIsValid)
        {
          return false;
        }
        innerStart = *end;
        innerEnd = *end;
        if (!validateJSONElement(jsonString, length, &innerStart, &innerEnd))
        {
          return false;
        }
        *end = innerEnd;
        commaIsValid = true;
        rightBraceIsValid = true;
        valueIsValid = false;
        break;
    }
  }
  return false;
}

bool validateArray(const char *jsonString, int length, int *start, int *end)
{
  int innerStart, innerEnd;
  bool rightBracketIsValid = true;
  bool commaIsValid = false;
  for (; *end < length; (*end)++)
  {
    switch (jsonString[*end])
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        break;
      case ']':
        if (rightBracketIsValid)
        {
          return true;
        }
        return false;
      case ',':
        if (!commaIsValid)
        {
          return false;
        }
        commaIsValid = false;
        rightBracketIsValid = false;
        break;
      default:
        if (rightBracketIsValid && commaIsValid)
        {
          return false;
        }
        innerStart = *end;
        innerEnd = *end;
        if (!validateJSONElement(jsonString, length, &innerStart, &innerEnd))
        {
          return false;
        }
        *end = innerEnd;
        commaIsValid = true;
        rightBracketIsValid = true;
        break;
    }
  }
  return false;
}

bool validateBoolean(const char *jsonString, int length, int *start, int *end)
{
  if (
      (*end) < length &&
      jsonString[*start] == 't' &&
      jsonString[(*start) + 1] == 'r' &&
      jsonString[(*start) + 2] == 'u' &&
      jsonString[(*start) + 3] == 'e')
  {
    return true;
  } else if (
      ((*end) + 1) < length &&
      jsonString[*start] == 'f' &&
      jsonString[(*start) + 1] == 'a' &&
      jsonString[(*start) + 2] == 'l' &&
      jsonString[(*start) + 3] == 's' &&
      jsonString[(*start) + 4] == 'e')
  {
    (*end)++;
    return true;
  }
  return false;
}

bool validateString(const char *jsonString, int length, int *start, int *end)
{
  bool controlCharacterIsValid = false;
  for (; *end < length; (*end)++)
  {
    switch (jsonString[*end])
    {
      case '"':
        return true;
      case '\\':
        (*end)++;
        if (jsonString[*end] == 'u')
        {
          if ((*end) + 4 > length)
          {
            return false;
          }
          // From https://github.com/zserge/jsmn/blob/25647e692c7906b96ffd2b05ca54c097948e879c/jsmn.h#L241-L251
          for (int x = 0; x < 4; (*end)++ && x++)
          {
            if (!(
                 (jsonString[(*end) + 1] >= 48 && jsonString[(*end) + 1] <= 57) || /* 0-9 */
                 (jsonString[(*end) + 1] >= 65 && jsonString[(*end) + 1] <= 70) || /* A-F */
                 (jsonString[(*end) + 1] >= 97 && jsonString[(*end) + 1] <= 102)   /* a-f */
               ))
            {   
              return false;
            }
          }
        }
      case '\t':
      case '\n':
      case '\r':
      default:
        break;
    }
  }
  return false;
}

bool validateNumber(const char *jsonString, int length, int *start, int *end)
{
  bool periodIsValid = jsonString[*start] != '-';
  bool eIsValid = jsonString[*start] != '-';
  bool plusMinusIsValid = false;
  for (; *end < length; (*end)++)
  {
    switch (jsonString[*end])
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
        if (plusMinusIsValid)
        {
          plusMinusIsValid = false;
        }
        if (jsonString[*start] == '-' && *end == ((*start) + 1)) {
          eIsValid = true;
          periodIsValid = true;
        }
        break;
      case '.':
        if (!periodIsValid)
        {
          return false;
        }
        periodIsValid = false;
        break;
      case 'e':
      case 'E':
        if (!eIsValid)
        {
          return false;
        }
        periodIsValid = false;
        eIsValid = false;
        plusMinusIsValid = true;
        break;
      case '+':
      case '-':
        if (!plusMinusIsValid)
        {
          return false;
        }
        break;
      case '}':
      case ']':
      case ',':
      case ' ':
      case '\t':
      case '\r':
      case '\n':
      case '\0':
        if (jsonString[*start] == '-' && *end == ((*start) + 1)) {
          return false;
        }
        (*end)--;
        return true;
      default:
        return false;
    }
  }

  return true;
}

bool validateJSONElement(const char *jsonString, int length, int *start, int *end)
{
  bool toReturn;
  int innerStart;
  int innerEnd;
  for (; *end < length; (*end)++)
  {
    switch (jsonString[*end])
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        break;
      case '{':
        innerStart = innerEnd = (*end) + 1;
        toReturn = validateObject(jsonString, length, &innerStart, &innerEnd);
        *start = *end = innerEnd;
        return toReturn;
      case '[':
        innerStart = innerEnd = (*end) + 1;
        toReturn = validateArray(jsonString, length, &innerStart, &innerEnd);
        *start = *end = innerEnd;
        return toReturn;
      case '"':
        innerStart = innerEnd = (*end) + 1;
        toReturn = validateString(jsonString, length, &innerStart, &innerEnd);
        *start = *end = innerEnd;
        return toReturn;
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
      case '-':
        innerStart = *end;
        innerEnd = (*end) + 1;
        toReturn = validateNumber(jsonString, length, &innerStart, &innerEnd);
        *start = *end = innerEnd;
        return toReturn;
      case 't':
      case 'f':
        innerStart = *end;
        innerEnd = (*end) + 3;
        toReturn = validateBoolean(jsonString, length, &innerStart, &innerEnd);
        *start = *end = innerEnd;
        return toReturn;
      default:
        return false;
    }
  }
}

bool validateJSON(const char *jsonString) {
  int length = strlen(jsonString);
  int start = 0;
  int end = 0;

  return validateJSONElement(jsonString, length, &start, &end);
}

int main(int argc, char *argv[])
{
  if (argc == 2) {
    if (!validateJSON(argv[1]))
    {
      printf("ERROR: %s should be valid!\n", argv[1]);
      return -1;
    }
    printf("PASS\n");
    return 0;
  } else if (argc == 1) {
    DIR *dirp;
    struct dirent *dp;
    FILE *file;
    char buffer[10000];
    size_t n;

    printf("Test results:\n");
    printf("================\n");

    dirp = opendir("valid");
    chdir("valid");
    while ((dp = readdir(dirp)) != NULL)
    {
      if (dp->d_name[0] != '.' && (file = fopen(dp->d_name, "r")) != NULL)
      {
        n = fread(buffer, sizeof(char), 10000, file);
        buffer[n] = '\0';
        if (!validateJSON(buffer))
        {
          printf("ERROR: %s should be valid!\n", buffer);
          fclose(file);
          return -1;
        }
        fclose(file);
      }
    }
    closedir(dirp);

    chdir("..");
    dirp = opendir("invalid");
    chdir("invalid");
    while ((dp = readdir(dirp)) != NULL)
    {
      if (dp->d_name[0] != '.' && (file = fopen(dp->d_name, "r")) != NULL)
      {
        n = fread(buffer, sizeof(char), 10000, file);
        buffer[n] = '\0';
        if (validateJSON(buffer))
        {
          printf("ERROR: %s should be invalid!\n", buffer);
          fclose(file);
          return -1;
        }
        fclose(file);
      }
    }
    closedir(dirp);

    printf("PASS\n");
    return 0;
  }
  return -1;
}
