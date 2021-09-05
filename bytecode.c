/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

#include "chj-64lib/util.h"
#include "bytecode.h"
#include "vm.h"


bool bytecode_write_file(const uint8_t *program,
                         size_t programlen,
                         const char *path) {
    int fd = open(path, O_CREAT|O_WRONLY, 438 /* 0o666 */);
    if (fd < 0) {
        fprintf(stderr, "open('%s') for writing: %s\n", path, strerror(errno));
        return false;
    }
    if (write(fd, (void*)program, programlen) < 0) {
        fprintf(stderr, "write('%s'): %s\n", path, strerror(errno));
        close(fd);
        return false;
    }
    if (close(fd) < 0) {
        fprintf(stderr, "close('%s'): %s\n", path, strerror(errno));
        return false;
    }
    return true;
}

bool bytecode_load_and_run(const char* path,
                           uint16_t stacklen,
                           uint16_t heaplen) {
    printf("Loading and running '%s'...\n", path);
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "open('%s'): %s\n", path, strerror(errno));
        return false;
    }
    struct stat st;
    if (fstat(fd, &st) < 0) {
        fprintf(stderr, "stat('%s'): %s\n", path, strerror(errno));
        close(fd);
        return false;
    }
    uint8_t *mapping = (uint8_t *)mmap(
        NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (! mapping) {
        fprintf(stderr, "mmap('%s'): %s\n", path, strerror(errno));
        close(fd);
        return false;
    }

    LET_NEW_VM_PROCESS(process, stacklen, heaplen);
    vm_process_run(process, mapping);
    printf("gc_count = %" PRIu32 ", gc_moves = %" PRIu64 "\n",
           process->gc_count, process->gc_moves);
    printf("stack = ");
    vm_process_stack_writeln(process);
    FREE_VM_PROCESS(process);
    
    munmap(mapping, st.st_size);
    return true;
}
