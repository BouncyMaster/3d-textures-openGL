#include "file_ops.h"

char *
file_to_str(const char *file_path)
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
		if (fread(buffer, 1, length, fp) != length)
			exit(1);
		fclose(fp);

		buffer[length] = '\0';

		return buffer;
	} else return 0;
}
