#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#ifdef USE64BIT
 typedef int64_t int_t;
 #define PRIi PRIi64
#else
 typedef int32_t int_t;
 #define PRIi PRIi32
#endif 

static int_t fib(int_t n) {
    return n < 2 ? 1 : fib(n-1) + fib(n-2);
}

int main(int argc, const char**argv) {
    assert(argc == 2);
    long n = atol(argv[1]);
    int_t res = fib(n);
    printf("fib(%li) = %" PRIi "\n", n, res);
    return 0;
}
