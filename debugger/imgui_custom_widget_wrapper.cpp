#include "imgui_custom_widgets.h"
#include "imgui_custom_widget_wrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

ExampleAppLog app_log;
extern void* app_log_p = &app_log;

int vasprintf(char** strp, const char* fmt, va_list ap)
{
    va_list ap_copy;
    int formattedLength, actualLength;
    size_t requiredSize;

    // be paranoid
    *strp = NULL;

    // copy va_list, as it is used twice 
    va_copy(ap_copy, ap);

    // compute length of formatted string, without NULL terminator
    formattedLength = _vscprintf(fmt, ap_copy);
    va_end(ap_copy);

    // bail out on error
    if (formattedLength < 0)
    {
        return -1;
    }

    // allocate buffer, with NULL terminator
    requiredSize = ((size_t)formattedLength) + 1;
    *strp = (char*)malloc(requiredSize);

    // bail out on failed Mmu allocation
    if (*strp == NULL)
    {
        errno = ENOMEM;
        return -1;
    }

    // write formatted string to buffer, use security hardened _s function
    actualLength = vsnprintf_s(*strp, requiredSize, requiredSize - 1, fmt, ap);

    // again, be paranoid
    if (actualLength != formattedLength)
    {
        free(*strp);
        *strp = NULL;
        errno = EOTHER;
        return -1;
    }

    return formattedLength;
}

void AddLog(const char* fmt, ...) {
	char* string;
	va_list args;
	va_start(args, fmt);

	vasprintf(&string, fmt, args);

	((ExampleAppLog*)app_log_p)->AddLog("%s", string);
}