#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <stdio.h>
#include <stdlib.h>

char *file_to_str(const char *file_path);

inline void file_to_str_free(char *buffer) {
	free(buffer);
}

#endif
