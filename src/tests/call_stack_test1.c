#include <stdio.h>

void blorf()
{
    printf("Done.");
}

void baz()
{
    blorf();
}

void bar()
{
    baz();
}

void foo()
{
    bar();
}

int main()
{
    foo();
}
