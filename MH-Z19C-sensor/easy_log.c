#include <stdio.h>
#include <string.h>
#include <stdarg.h>


void MyTrace(const char *fileName, int line, const char *msg, ...) {
    va_list args;
    char buffer[256] = { 0 };
    char* buffer_ptr = &buffer[0];
    unsigned char n = 0;
    
    // add file and line
    n = snprintf(buffer, sizeof(buffer), "%s:%d\t", fileName, line);
    buffer_ptr += n;

    // add format string and argument
    va_start(args, msg);
    vsnprintf(buffer_ptr, sizeof(buffer)-n, msg, args);
    va_end(args);
    
//     std::cout << buffer << std::endl;
    printf(buffer);
    printf("\n");
}


size_t snprintfcat(char* buf, size_t bufSize, char const* fmt, ...) {
    size_t result;
    va_list args;
    size_t len = strnlen( buf, bufSize);

    va_start(args, fmt);
    result = vsnprintf( buf + len, bufSize - len, fmt, args);
    va_end(args);

    return result + len;
}
