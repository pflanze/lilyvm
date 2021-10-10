/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <inttypes.h>

#include "chj-64lib/util.h"
#include "bytecode.h"

const stacksize_t default_stacklen = 256;
const uint16_t default_heaplen = 1024;

NORETURN help(const char* maybe_fmt, const char* arg) {
    if (maybe_fmt) {
        printf(maybe_fmt, arg);
        printf("\n");
    }
    printf("Usage: lilyvm bytecodefile(s)\n"
           "\n"
           "  Currently simply loads and runs each file individually\n"
           "  in a fresh process in sequence, and shows GC statistics\n"
           "  and the result stack.\n"
           "\n"
           "  Options: \n"
           "    -s|--stack stacklen   (in words, default %" PRIi16 ")\n"
           "    -h|--heap  heaplen    (in words, default %" PRIi16 ")\n"
           "\n"
           , default_stacklen, default_heaplen);
    exit(1);
}

bool parse_uint16(const char *str, uint16_t *resptr) {
    int r = atoi(str);
    if (r < 0) return false;
    uint16_t res = r;
#define BUFS 100
    char buf[BUFS];
    if (snprintf(buf, BUFS, "%" PRIu16, res) >= BUFS)
        return false;
#undef BUFS
    if (strcmp(buf, str) != 0)
        return false;
    *resptr = res;
    return true;
}

bool perhaps_parseopt_uint16(int argc,
                             const char **argv,
                             int *i,
                             const char *optstr,
                             uint16_t *resptr) {
    if (strcmp(argv[*i], optstr) == 0) {
        (*i)++;
        if (*i < argc) {
            if (parse_uint16(argv[*i], resptr)) {
                return true;
            } else {
                help("invalid value for %s", optstr);
            }
        } else {
            help("missing value after %s", optstr);
        }
    } else {
        return false;
    }
}

int main(int argc, const char **argv)
{
    const char **paths = (const char**)alloca(argc * sizeof(char*));
    if (argc < 2) help(NULL, NULL);
    stacksize_t stacklen = default_stacklen;
    uint16_t heaplen = default_heaplen;
    bool stop_parsing = false;
    int j = 0;
    for (int i = 1; i < argc; i++) {
        if (! stop_parsing) {
            if (strcmp(argv[i], "--help") == 0) help(NULL, NULL);
            if (perhaps_parseopt_uint16(argc, argv, &i, "-s", &stacklen))
                continue;
            if (perhaps_parseopt_uint16(argc, argv, &i, "--stack", &stacklen))
                continue;
            if (perhaps_parseopt_uint16(argc, argv, &i, "-h", &heaplen))
                continue;
            if (perhaps_parseopt_uint16(argc, argv, &i, "--heap", &heaplen))
                continue;
            if (strcmp(argv[i], "--") == 0) {
                stop_parsing = true;
                continue;
            }
        }
        paths[j++] = argv[i];
    }
    for (int i = 0; i < j; i++) {
        if (! bytecode_load_and_run(paths[i], stacklen, heaplen))
            exit(1);
    }
    exit(0);
}
