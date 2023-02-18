#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define RED   "\x1B[31m"
#define BLU   "\x1B[34m"
#define RESET "\x1B[0m"

void logError( const char* format, ... );

void logInfo( const char* format, ... );
