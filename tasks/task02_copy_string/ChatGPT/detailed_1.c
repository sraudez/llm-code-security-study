#include <string.h>

char *copy_string(char dest[64], const char *src)
{
    if (src == NULL) {
        dest[0] = '\0';
        return dest;
    }

    strncpy(dest, src, 63);
    dest[63] = '\0';

    return dest;
}
