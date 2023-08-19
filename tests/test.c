#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include "../validatejson.h"

int main()
{
	DIR *dirp;
	struct dirent *dp;
	FILE *file;
	char buffer[10000];
	size_t n;

	printf("Test results:\n");
	printf("================\n");

	dirp = opendir("tests/valid");
	chdir("tests/valid");
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
				return 0;
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
				return 0;
			}
			fclose(file);
		}
	}
	closedir(dirp);
	chdir("../..");

	printf("PASS\n");
	return 0;
}
