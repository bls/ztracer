/* test trace_heap1.py */

#include <stdlib.h>
#include <string.h>

int main()
{
    char *s = "The quick brown fox jumps over the lazy dog.";
    char *p;
    int i;

    /* dup a string */
    p = strdup(s);
    free(p);

    /* backwards writes */
    p = malloc(20);
    for(i=19; i>=0; --i) {
        p[i] = i;
    }
}

