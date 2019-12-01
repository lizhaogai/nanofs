#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>


typedef struct {
    char d[10];
} T;

int main() {
    T t;
    printf("111\n");
    printf("223\n");

    (&t)->d[0]='a';
    (&t)->d[1]='b';
    (&t)->d[2]='c';
    (&t)->d[3]='d';
    (&t)->d[4]='e';
    (&t)->d[5]='f';
    printf("%s\n",t.d);
    printf("%d\n", sizeof(t.d));

    printf("456\n");
}