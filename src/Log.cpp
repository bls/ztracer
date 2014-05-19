#include "Log.h"
#include <stdarg.h>

const char *
log_format(const char *fmt, ...)
{
	va_list va;
	static char formatted[1024];
	va_start(va, fmt);
	vsnprintf(formatted, 1024, fmt, va);
	va_end(va);
	return formatted;
}

