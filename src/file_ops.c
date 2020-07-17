#include <stdio.h>
#include <stdlib.h>

char *file_to_str(const char *file_path)
{
	FILE *fp;
	unsigned int length;
	char *buffer;

	fp = fopen(file_path, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		buffer = malloc(length + 1);
		fseek(fp, 0, SEEK_SET);
		fread(buffer, length, 1, fp);
		fclose(fp);

		buffer[length] = '\0';

		return buffer;
	} else return 0;
}
