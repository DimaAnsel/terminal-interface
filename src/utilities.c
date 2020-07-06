/**
 * @file utilities.c
 * Utilites
 */

#include <stdio.h>

#include "utilities.h"

void clear_log() {
	FILE* f = fopen(DEBUG_LOG_FILE, "w");
	fclose(f);
}

void log(char* err) {
	FILE* f = fopen(DEBUG_LOG_FILE, "a");
	fprintf(f, err);
	fclose(f);
}
