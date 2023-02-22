/**
 * @file logUtil.c
 * @author Gustavo Henrique da Silva Barbosa (ghsb19) e Calebe Pompeo Helpa (cph19)
 * @brief 
 * 
 */

#include "logUtil.h"

void logError( const char* format, ... ) {
	va_list args;
	fprintf( stderr, RED "[ERRO]: " );
	va_start( args, format );
	vfprintf( stderr, format, args);
	va_end( args );
	fprintf( stderr, "\n" RESET);
}

void logInfo( const char* format, ... ) {
	va_list args;
	fprintf( stdout, BLU "[INFO]: " );
	va_start( args, format );
	vfprintf( stdout, format, args);
	va_end( args );
	fprintf( stdout, "\n" RESET);
}

void logWarning( const char* format, ... ) {
	va_list args;
	fprintf( stdout, YEL "[WARNING]: " );
	va_start( args, format );
	vfprintf( stdout, format, args);
	va_end( args );
	fprintf( stdout, "\n" RESET);
}