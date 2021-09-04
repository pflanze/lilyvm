/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/

#ifndef _VM_H
#define _VM_H

#include "chj-64lib/util.h"

#include "vm_mem.h"
#include "vm_process.h"

void vm_process_run(struct vm_process *process, uint8_t *code);

void vm_process_stack_writeln(struct vm_process *process);

void vm_process_stack_clear(struct vm_process *process);

#endif
