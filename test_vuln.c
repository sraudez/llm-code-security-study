// test_vuln.c
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[64];
    gets(buf);
    printf(buf);
    strcpy(buf, "test");
    return 0;
}