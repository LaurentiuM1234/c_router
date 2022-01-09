#include "fatal.h"

#include <stdio.h>
#include <stdlib.h>


void fatal_error(const char *message, char *file, int line)
{
    fprintf(stderr, "FATAL ERROR\n[FILE]: %s\n[LINE]: %d\n[DESCRIPTION]: %s\n", file, line, message);
    exit(EXIT_FAILURE);
}