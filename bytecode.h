/*
  Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>
  See the LICENSE file that comes bundled with this file.
*/
#ifndef BYTECODE_H_
#define BYTECODE_H_

#include <stdint.h>
#include "opcode_constants.h"

#define OP(op)                                  \
    pc[0]= OP_##op;                             \
    pc++;
#define OP_B(op,a1)                             \
    pc[0]= OP_##op;                             \
    pc[1]= a1;                                  \
    pc+=2;
#define OP_B_B(op,a1,a2)                        \
    pc[0]= OP_##op;                             \
    pc[1]= a1;                                  \
    pc[2]= a2;                                  \
    pc+=3;
#define OP_IM(op,v)                             \
    pc[0]= OP_##op;                             \
    pc[1]= (v)&255;                             \
    pc[2]= (v)>>8;                              \
    pc+=3;
#define OP_IM_IM(op,v1,v2)                      \
    pc[0]= OP_##op;                             \
    pc[1]= (v1)&255;                            \
    pc[2]= (v1)>>8;                             \
    pc[3]= (v2)&255;                            \
    pc[4]= (v2)>>8;                             \
    pc+=5;
#define OP_IM_B(op,im,b)                        \
    pc[0]= OP_##op;                             \
    pc[1]= (im)&255;                            \
    pc[2]= (im)>>8;                             \
    pc[3]= b;                                   \
    pc+=4;

bool bytecode_write_file(const uint8_t *program,
                         size_t programlen,
                         const char *path);
bool bytecode_load_and_run(const char* path,
                           uint16_t stacklen,
                           uint16_t heaplen);

#endif /* BYTECODE_H_ */
