#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../validatejson.h"

long buffer_len(FILE *file)
{
	long len;

	// determine file length
	if (fseek(file, 0, SEEK_END) != 0) {
		perror("fseek");
		return -1;
	}
	len = ftell(file);
	if (len < 0) {
		perror("ftell");
		return -1;
	}
	rewind(file);

	return len;
}

int main()
{
	DIR *dirp;
	struct dirent *dp;
	FILE *file;
	size_t n;
	long len;

	printf("Test results:\n");
	printf("================\n");

	if (!(dirp = opendir("tests/valid")))
	{
		perror("opendir");
		return 1;
	}
	if (chdir("tests/valid") != 0)
	{
		perror("chdir");
		closedir(dirp);
		return 1;
	}
	while ((dp = readdir(dirp)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && (file = fopen(dp->d_name, "r")) != NULL)
		{
			if ((len = buffer_len(file)) == -1) {
				fprintf(stderr, "Exited earlier on file %s\n", dp->d_name);
				return 1;
			}
			char buffer[len];
			n = fread(buffer, sizeof(char), len, file);
			buffer[n] = '\0';
			const char *_buffer = buffer;
			if (!validateJSON(&_buffer))
			{
				fprintf(stderr, "ERROR: %s should be valid!\n", dp->d_name);
				fclose(file);
				return 1;
			}
			fclose(file);
		}
	}
	closedir(dirp);

	if (chdir("..") != 0)
	{
		perror("chdir");
		return 1;
	}
	if (!(dirp = opendir("invalid")))
	{
		perror("opendir");
		return 1;
	}
	if (chdir("invalid") != 0)
	{
		perror("chdir");
		closedir(dirp);
		return 1;
	}
	while ((dp = readdir(dirp)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && (file = fopen(dp->d_name, "r")) != NULL)
		{
			if ((len = buffer_len(file)) == -1) {
				fprintf(stderr, "Exited earlier on file %s\n", dp->d_name);
				return 1;
			}
			char buffer[len];
			n = fread(buffer, sizeof(char), len, file);
			buffer[n] = '\0';
			const char *_buffer = buffer;
			if (validateJSON(&_buffer))
			{
				fprintf(stderr, "ERROR: %s should be invalid!\n", dp->d_name);
				fclose(file);
				return 1;
			}
			fclose(file);
		}
	}
	closedir(dirp);
	if (chdir("../..") != 0)
	{
		perror("chdir");
		return 1;
	}

	printf("PASS\n");
	return 0;
}
