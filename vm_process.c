/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/
#include <stdlib.h>
#include <string.h>
#include "chj-64lib/assert.h"
#include "chj-64lib/util.h"
#include "vm_process.h"
#include "vm_mem.h" /* for UNINITIALIZED, strictly speaking circular dep */

struct vm_process *malloc_process(stacksize_t stacklen,
                                  uint16_t numheapwords) {
    WARN_("malloc_process(%i, %i)", stacklen, numheapwords);
    // GC'd memory
    const uintptr_t sz0 = numheapwords * sizeof(val);
    const uintptr_t sz = sz0 * VM_PROCESS_NUM_ALLOC_AREAS;
    uintptr_t ph = (uintptr_t)malloc(sz + 1);
    if (!ph) {
        WARN_("malloc: could not allocate %i * %i words",
              VM_PROCESS_NUM_ALLOC_AREAS, numheapwords);
        return NULL;
    }
    // align on word boundary, XX assumes 16-bit word_t
    if (IS_ODD(ph)) {
        ph++;
    }

    {
        // Main object and stack memory
        struct vm_process *p= (struct vm_process *)malloc(
            sizeof(struct vm_process)
            // inlined stack:
            + (stacklen - 1) * sizeof(val));
        if (!p) {
            WARN("malloc: could not allocate stack");
            free((void*)ph);
            return NULL;
        }
        p->stack.sp = 0;
        p->stack.len = stacklen;

        p->A = UNINITIALIZED;
        p->B = UNINITIALIZED;
        p->C = UNINITIALIZED;
        p->D = UNINITIALIZED;
        p->alloc_area_base = (word_t*)ph;
        p->alloc_size = sz0;
        p->alloc_area = (word_t*)ph;
        p->alloc_ptr = add_word_and_bytes((word_t*)ph, sz0);
        p->alloc_area_fresh = add_word_and_bytes((word_t*)ph, sz0);
        p->gc_count = 0;
        p->gc_moves = 0;
        p->num_val_roots = 0;
        p->num_ptr_roots = 0;
#ifdef VM_TRACE
        p->trace_on = false;
#endif
#if DEBUG_MEM_SET
        memset(p->alloc_area, 0xAA, p->alloc_size);
        memset(p->alloc_area_fresh, 0xBB, p->alloc_size);
        memset(&p->stack.vals[0], 0xCC, stacklen * sizeof(val));
#endif
        return p;
    }
}

void vm_process_free(struct vm_process *p) {
    ASSERT(p->alloc_area);
    free(p->alloc_area_base);
    p->alloc_area = 0;
    free(p);
}


NORETURN die_out_of_val_roots() {
    DIE("out of val roots");
}
NORETURN die_out_of_ptr_roots() {
    DIE("out of ptr roots");
}

NORETURN die_no_val_roots_left_to_drop() {
    DIE("no val roots left to drop");
}
NORETURN die_no_ptr_roots_left_to_drop() {
    DIE("no ptr roots left to drop");
}
