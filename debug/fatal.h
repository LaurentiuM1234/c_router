#ifndef __FATAL__
#define __FATAL__


#define FATAL_ERROR(message) fatal_error(message, __FILE__, __LINE__)

/*
 * Print error message and exit program with code EXIT_FAILURE.
 * @params: message -> the error message
 *          file -> file in which the error took place
 *          line -> line at which the error took place
 * @returns: no return
 */
void fatal_error(const char *message, char *file, int line);

#endif
