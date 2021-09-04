/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/
#ifndef VM_PROCESS_H_
#define VM_PROCESS_H_

/*
  The guest language process abstraction.

  This module only contains bare object operations.  Actual "meaty"
  operations on them are defined in vm_mem.c or vm.c.

*/


#include "vm_mem_config.h"


/*
  This is for the course grained memory handling.
  For fine-grained, see vm_mem.*
*/

// NOTE: these need a definition of ON_FAIL in their context!

#define FAIL1(failvar, msg) do {                 \
        failvar.message = msg;                   \
        failvar.file = __FILE__;                 \
        failvar.line = __LINE__;                 \
        ON_FAIL;                                 \
    } while (0)

#define FAIL2(failvar, ctxmsg, errmsg) do {      \
        failvar.context_message = ctxmsg;        \
        failvar.error_message = errmsg;          \
        failvar.file = __FILE__;                 \
        failvar.line = __LINE__;                 \
        ON_FAIL;                                 \
    } while (0)

struct vm_stack_failure {
    const char *context_message;
    const char *error_message;
    const char *file;
    int line;
};

// Just a linear fixed stack for now.
struct vm_stack {
    uint16_t sp; // number of elements currently on the stack
    uint16_t len;
    struct vm_stack_failure failure;
    val vals[1];
};

// Currently using simple full copying GC (Cheney).

#define VM_PROCESS_NUM_ALLOC_AREAS 2

#define VM_NUM_VAL_ROOTS 4
#define VM_NUM_PTR_ROOTS 4

struct vm_process {
    // GC'd memory:
    /* const */ word* alloc_area_base; // start of concatenated allocation areas
    /* const */ uintptr_t alloc_size; // size of one area in bytes
    word* alloc_area; // area currently being allocated from
    word* alloc_area_fresh; // area to copy to during gc
    word* alloc_ptr; // moving downwards, pointing to the *last* allocation
    // GC statistics:
    uint32_t gc_count; // number of gc runs that occurred (wrapping)
    uint64_t gc_moves; // number of object moves in total (wrapping)
    // stacks of roots
    uint8_t num_val_roots;
    uint8_t num_ptr_roots;
    val* val_roots[VM_NUM_VAL_ROOTS]; // pointers to val variables
    word** ptr_roots[VM_NUM_PTR_ROOTS]; // pointers to *body* pointer variables
    // Execution stack:
    struct vm_stack stack;
};

struct vm_process *malloc_process(uint16_t stacklen,
                                  uint16_t numheapwords);
void vm_process_free(struct vm_process *p);

INLINE static
word *add_word_and_bytes(word *a, uintptr_t b) {
    return (word*)((uintptr_t)a + b);
}

#define LET_NEW_VM_PROCESS(var, stacklen, heaplen)                      \
    struct vm_process *process = malloc_process(stacklen, heaplen);     \
    if (! process)                                                      \
        DIE("out of memory allocating process");

#define FREE_VM_PROCESS(var) do { vm_process_free(var); var = 0; } while(0)


NORETURN die_out_of_val_roots();
NORETURN die_out_of_ptr_roots();

NORETURN die_no_val_roots_left_to_drop();
NORETURN die_no_ptr_roots_left_to_drop();

UNUSED static
void vm_process_register_val_root(
    struct vm_process *process, val* _var) {
    uint8_t n = process->num_val_roots;
    if (n < VM_NUM_VAL_ROOTS-1) {
        process->val_roots[n] = _var;
        process->num_val_roots = n + 1;
    } else {
        die_out_of_val_roots();
    }
}

UNUSED static
void vm_process_register_ptr_root(
    struct vm_process *process, word** _var) {
    uint8_t n = process->num_ptr_roots;
    if (n < VM_NUM_PTR_ROOTS-1) {
        process->ptr_roots[n] = _var;
        process->num_ptr_roots = n + 1;
    } else {
        die_out_of_ptr_roots();
    }
}

INLINE static
void vm_process_unregister_val_roots(
    struct vm_process *process, uint8_t n) {
    if (n <= process->num_val_roots) {
        process->num_val_roots -= n;
    } else {
        die_no_val_roots_left_to_drop();
    }
}

INLINE static
void vm_process_unregister_ptr_roots(
    struct vm_process *process, uint8_t n) {
    if (n <= process->num_ptr_roots) {
        process->num_ptr_roots -= n;
    } else {
        die_no_ptr_roots_left_to_drop();
    }
}

#endif /* VM_PROCESS_H_ */
