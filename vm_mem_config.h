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
typedef uint16_t word_t;
typedef int16_t signed_word_t; // *should* always be same as fixaddint_t ? XX
typedef uint32_t dword_t; // double word
typedef int32_t signed_dword_t; // *should* always be same as fixmulint_t ?

/* a tagged value (immediate or allocated) */
typedef word_t val;

/* An integer large enough to represent the result of fixnum + fixnum */
typedef int16_t fixaddint_t;
/* An integer large enough to represent the result of fixnum * fixnum;
   also see dword_t for unsigned variant. */
typedef int32_t fixmulint_t;

/*
  integer type for the number of words contained in the body of an
  allocated object
*/
typedef unsigned char numwords_t;



// Settings; XX move to make process

#define DEBUG_MEM_SET 1
#define DEBUG_MEM_DEBUG 0
#define DEBUG_MEM_VERIFY 1


#endif /* VM_MEM_CONFIG_H_ */
