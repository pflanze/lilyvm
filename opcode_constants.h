/* DO NOT EDIT. This file has been generated by opcodes.scm */

#ifndef _OPCODE_CONSTANTS_H_
#define _OPCODE_CONSTANTS_H_

//                        opcode  numargbytes
#define OP_PUSH_IM                   10 /* 2 */
#define OP_DROP1                     11 /* 0 */
#define OP_PICK_B                    12 /* 1 */
#define OP_SWAP                      13 /* 0 */
#define OP_DUP                       14 /* 0 */
#define OP_INC                       20 /* 0 */
#define OP_INC_                      21 /* 1 */
#define OP_DEC                       25 /* 0 */
#define OP_ADD                       30 /* 0 */
#define OP_ADD_IM                    31 /* 2 */
#define OP_ADD__                     32 /* 2 */
#define OP_MUL__                     33 /* 2 */
#define OP_BITWISE_AND               40 /* 0 */
#define OP_UNSAFE_BITWISE_AND        41 /* 0 */
#define OP_UNSAFE_BITWISE_OR         42 /* 0 */
#define OP_JMP_REL8                  80 /* 1 */
#define OP_JMP_REL16                 81 /* 2 */
#define OP_JSR_REL8                  85 /* 1 */
#define OP_RET                       86 /* 0 */
#define OP_RET_IM                    87 /* 2 */
#define OP_RET_POP                   88 /* 0 */
#define OP_BEQ_IM_REL16              91 /* 4 */
#define OP_BPOS_KEEP_REL16           92 /* 2 */
#define OP_BNEG0_KEEP_REL16          93 /* 2 */
#define OP_BNEG_KEEP_REL16           94 /* 2 */
#define OP_BZ_KEEP_REL16             95 /* 2 */
#define OP_BZ_REL16                  96 /* 2 */
#define OP_CMPBR_KEEP_LT_IM_REL8     100 /* 3 */
#define OP_DEC__DUP                  200 /* 0 */
#define OP_SWAP__DEC                 201 /* 0 */
#define OP_FIB                       220 /* 0 */
#define OP_FIB_WITH_REGISTERS        221 /* 0 */
#define OP_NOP                       254 /* 0 */
#define OP_HALT                      255 /* 0 */

#endif /* _OPCODE_CONSTANTS_H_ */
