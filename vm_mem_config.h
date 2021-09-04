#ifndef VM_MEM_CONFIG_H_
#define VM_MEM_CONFIG_H_

/*
  Base definitions, i.e. parameters for the rest of the memory system.
*/

#include <stdint.h>

/*
  For now, use a 16-bit val on all platforms (restricting memory usage
  to 64KiB). Will change later. On non-C64 platforms, the 16-bit
  pointers are an index into the memory area; on C64, it's an actual
  address.
*/
typedef uint16_t word;
typedef int16_t signed_word_t; // *should* always be same as fixaddint_t ? XX
typedef uint32_t dword_t; // double word; also see fixmulint_t for signed

/* a tagged value (immediate or allocated) */
typedef word val;

/*
  integer type for the number of words contained in the body of an
  allocated object
*/
typedef unsigned char numwords_t;



// Settings; XX move to make process

#define DEBUG_MEM_SET 1
#define DEBUG_MEM_DEBUG 0
#define DEBUG_MEM_VERIFY 1

//#define VM_TRACE


#endif /* VM_MEM_CONFIG_H_ */
