/**
 * @file utilities.c
 * Utilites
 */

#include <stdio.h>

#include "utilities.h"

/**
 * Wipes the debug log.
 */
void clear_log() {
	FILE* f = fopen(DEBUG_LOG_FILE, "w");
	fclose(f);
}

/**
 * Appends a message to the log file.
 */
void log(char* err) {
	FILE* f = fopen(DEBUG_LOG_FILE, "a");
	fprintf(f, err);
	fclose(f);
}
