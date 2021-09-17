/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/

#ifndef _VM_H
#define _VM_H

#include "chj-64lib/util.h"

#include "vm_mem.h"
#include "vm_process.h"

#ifdef VM_TRACE
INLINE static
void vm_process_trace_on(struct vm_process *process, bool yes) {
    process->trace_on = yes;
}
#endif

void vm_process_run(struct vm_process *process, uint8_t *code);

void vm_process_stack_write(struct vm_process *process);
void vm_process_registers_write(struct vm_process *process,
                                const char* separator);

void vm_process_stack_clear(struct vm_process *process);

#endif
