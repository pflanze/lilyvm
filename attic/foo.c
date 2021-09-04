#ifdef __C64__
# include <conio.h>
#endif
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "chj-64lib/assert.h"
#include "chj-ctest/test.h"
#include "chj-ctest/run_tests.h"
#include "chj-64lib/util.h"
#include "chj-64lib/benchmark.h"
#include "vm.h"

#define BORDERCOLOR BYTE(0xD020)
#define BGCOLOR BYTE(0xD021)

#ifdef __C64__

void fastcall fastfill4pages(unsigned char fillvalue);
extern volatile uint8_t fastfill_base;

static void colorram_fill (unsigned char color) {
    // memset(PTR(0xD800), color, 0xDBE8-0xD800);
    fastfill_base = 0xD8; fastfill4pages(color);
    /* memset(PTR(0xD800), color, 0xDC00-0xD800); */
}

static void hires_set_page(unsigned char pageno) {
    // https://www.c64-wiki.com/wiki/Page_208-211
    // $D018
    // Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
    // VM13 	VM12 	VM11 	VM10 	CB13 	CB12 	CB11 	-
    // _______________________________
    POKE(0xd018, (PEEK(0xd018) & (255>>4)) | (pageno << 4));
}

static void hires() {
    // $d011=$3b, $d016=8
    BYTE(0xd011) = 0x3b;
    BYTE(0xd016) = 8;
    hires_set_page(7);
    
}

#endif

// Benchmark timing:


// ------------------------------------------------------------------

#ifdef __C64__
static void bgcolors () {
    while (1) {
        unsigned char new = ++(*((unsigned char*)0xD021));
        char buf[100];
        int r = snprintf(buf, 99, "%3i\r\n", new & 15);
        cputsxy(0, 0, buf);
        cgetc();
    }
}
#endif

// ------------------------------------------------------------------

#ifdef __C64__

static void printintsize() {
    printf("sizeof(int) = %lu\r\n", sizeof(int));
}

// ------------------------------------------------------------------

static void _colorcycle_1(void*_ctx) {
    int i;
    for (i=0; i<16; i++) {
        colorram_fill(i);
    }
}

static void colorcycle() {
    {
        int i;
        for (i=0; i<18; i++) {
            printf("Hello World %i!\r\n", i);
        }
    }
    time_this(&_colorcycle_1, NULL, 100);
}

// ------------------------------------------------------------------

static void _fillscreen (void* as) {
    unsigned int a = ((unsigned char*)as)[0];
    int i;
    for (i=0; i<1000; i++) {
        POKE(1024+i,a);
    }
}

static void fillscreen (unsigned char a) {
    unsigned char as[1];
    as[0]= a;
    time_this(&_fillscreen, (void*)as,  100);
}

static void _fillscreen_fast (void* as) {
    unsigned int a = ((unsigned char*)as)[0];
    fastfill_base = 0x04; fastfill4pages(a);
}

static void fillscreen_fast (unsigned char a) {
    unsigned char as[1];
    as[0]= a;
    time_this(&_fillscreen_fast, (void*)as,  100);
}

#endif

// ------------------------------------------------------------------

/*
static void show_var(const char* msg, void*p) {
    int missinglen = 10 - strlen(msg);
    char padding[100];
    unsigned char i;
    ASSERT(missinglen < 100);
    for (i=0; i<missinglen; i++) {
        padding[i]= ' ';
    }
    padding[i] = '\0';
    printf("%s%sis at 0x%04"PRIxPTR" / %3"PRIuPTR"\r\n",
           msg, padding, (uintptr_t)p, (uintptr_t)p);
}
*/

#ifdef __C64__

#define GET_VARZP(x) (__asm__ ("lda #" #x), (void*)__A__ )

#define SHOW_VARZP(x) show_var(#x, GET_VARZP(x))

static void show_vars () {
    SHOW_VARZP(tmp1);
    SHOW_VARZP(tmp2);
    SHOW_VARZP(tmp3);
    SHOW_VARZP(tmp4);
    SHOW_VARZP(ptr1);
    SHOW_VARZP(ptr2);
    SHOW_VARZP(ptr3);
    SHOW_VARZP(ptr4);
    SHOW_VARZP(sp);
    SHOW_VARZP(sreg);
    SHOW_VARZP(regsave);
    SHOW_VARZP(regbank);
}

#endif

// ------------------------------------------------------------------

#ifdef __C64__
void main(void)
#else
int main(UNUSED int _argc, UNUSED char **_argv)
#endif
{
    benchmark_init();
    if (1) {
        run_tests();
    }
    if (1) {
        // colorcycle();
        //fillscreen('C');
        //fillscreen_fast('F');
        // bgcolors();
        // hires();
        //show_vars();
    }
}
