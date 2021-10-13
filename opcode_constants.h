/* DO NOT EDIT. This file has been generated by opcodes.scm */

#ifndef _OPCODE_CONSTANTS_H_
#define _OPCODE_CONSTANTS_H_

//                          opcode  /* hex; numargbytes */
#define OP_UNSAFE_LDA_               140 /* 0x8c; 1 */
#define OP_UNSAFE_LDB_               141 /* 0x8d; 1 */
#define OP_UNSAFE_LDC_               142 /* 0x8e; 1 */
#define OP_UNSAFE_LDD_               143 /* 0x8f; 1 */
#define OP_UNSAFE_LDM_INT_           144 /* 0x90; 1 */
#define OP_UNSAFE_LDN_INT_           145 /* 0x91; 1 */
#define OP_UNSAFE_STA_               150 /* 0x96; 1 */
#define OP_UNSAFE_STB_               151 /* 0x97; 1 */
#define OP_UNSAFE_STC_               152 /* 0x98; 1 */
#define OP_UNSAFE_STD_               153 /* 0x99; 1 */
#define OP_UNSAFE_STM_FIX_           154 /* 0x9a; 1 */
#define OP_UNSAFE_STN_FIX_           155 /* 0x9b; 1 */
#define OP_LOADM_IM                  1 /* 0x01; 2 */
#define OP_LOADN_IM                  2 /* 0x02; 2 */
#define OP_LOADA_IM                  5 /* 0x05; 2 */
#define OP_LOADB_IM                  6 /* 0x06; 2 */
#define OP_LOADC_IM                  125 /* 0x7d; 2 */
#define OP_LOADD_IM                  126 /* 0x7e; 2 */
#define OP_PUSHA                     7 /* 0x07; 0 */
#define OP_PUSHB                     8 /* 0x08; 0 */
#define OP_PUSHC                     128 /* 0x80; 0 */
#define OP_PUSHD                     129 /* 0x81; 0 */
#define OP_POPA                      9 /* 0x09; 0 */
#define OP_POPB                      10 /* 0x0a; 0 */
#define OP_POPC                      110 /* 0x6e; 0 */
#define OP_POPD                      111 /* 0x6f; 0 */
#define OP_TAB                       11 /* 0x0b; 0 */
#define OP_TBA                       12 /* 0x0c; 0 */
#define OP_SWAPA                     13 /* 0x0d; 0 */
#define OP_PUSHM                     19 /* 0x13; 0 */
#define OP_PUSHN                     23 /* 0x17; 0 */
#define OP_SWAPN                     24 /* 0x18; 0 */
#define OP_POPN__PUSHA               130 /* 0x82; 0 */
#define OP_PUSH_IM                   14 /* 0x0e; 2 */
#define OP_DROP1                     15 /* 0x0f; 0 */
#define OP_PICK_B                    16 /* 0x10; 1 */
#define OP_SWAP                      17 /* 0x11; 0 */
#define OP_DUP                       18 /* 0x12; 0 */
#define OP_INC                       20 /* 0x14; 0 */
#define OP_INC_                      21 /* 0x15; 1 */
#define OP_INCA                      22 /* 0x16; 0 */
#define OP_DEC                       25 /* 0x19; 0 */
#define OP_DECA                      26 /* 0x1a; 0 */
#define OP_DECN                      27 /* 0x1b; 0 */
#define OP_ADD                       30 /* 0x1e; 0 */
#define OP_ADD_IM                    31 /* 0x1f; 2 */
#define OP_ADD__                     32 /* 0x20; 2 */
#define OP_ADDA                      33 /* 0x21; 0 */
#define OP_UNSAFE_ADDA_              133 /* 0x85; 1 */
#define OP_ADDM                      34 /* 0x22; 0 */
#define OP_MUL__                     35 /* 0x23; 2 */
#define OP_BITWISE_AND               40 /* 0x28; 0 */
#define OP_UNSAFE_BITWISE_AND        41 /* 0x29; 0 */
#define OP_UNSAFE_BITWISE_OR         42 /* 0x2a; 0 */
#define OP_JMP_REL8                  80 /* 0x50; 1 */
#define OP_JMP_REL16                 81 /* 0x51; 2 */
#define OP_JSR_REL8                  85 /* 0x55; 1 */
#define OP_FRAME_JSR_REL8_1          185 /* 0xb9; 1 */
#define OP_FRAME_JSR_REL8_2          187 /* 0xbb; 1 */
#define OP_RET                       86 /* 0x56; 0 */
#define OP_UNSAFE_FRAME_RET          186 /* 0xba; 1 */
#define OP_RET_IM                    87 /* 0x57; 2 */
#define OP_RET_POP                   88 /* 0x58; 0 */
#define OP_BEQ_IM_REL16              91 /* 0x5b; 4 */
#define OP_BPOS_KEEP_REL16           92 /* 0x5c; 2 */
#define OP_BNEG0_KEEP_REL16          93 /* 0x5d; 2 */
#define OP_BNEG_KEEP_REL16           94 /* 0x5e; 2 */
#define OP_BZ_KEEP_REL16             95 /* 0x5f; 2 */
#define OP_BZ_REL16                  96 /* 0x60; 2 */
#define OP_CMPBR_KEEP_LT_IM_REL8     100 /* 0x64; 3 */
#define OP_CMPBR_A_LT_IM_REL8        101 /* 0x65; 3 */
#define OP_CMPBR_N_LT_IM_REL8        102 /* 0x66; 3 */
#define OP_DEC__DUP                  200 /* 0xc8; 0 */
#define OP_SWAP__DEC                 201 /* 0xc9; 0 */
#define OP_JSR_REL8__SWAP            202 /* 0xca; 1 */
#define OP_TRACE_ON                  252 /* 0xfc; 0 */
#define OP_TRACE_OFF                 253 /* 0xfd; 0 */
#define OP_NOP                       254 /* 0xfe; 0 */
#define OP_BREAK                     251 /* 0xfb; 0 */
#define OP_HALT                      255 /* 0xff; 0 */
#define OP_FIB                       220 /* 0xdc; 0 */
#define OP_FIB_WITH_REGISTERS        221 /* 0xdd; 0 */

#endif /* _OPCODE_CONSTANTS_H_ */
