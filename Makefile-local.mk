ifdef CPLUSPLUS
COMPILER=g++
else
COMPILER=gcc
# gcc -pedantic-errors conflicts with computed goto
endif

LTO=-flto

#PROFILE=-fprofile-generate=.profile
#PROFILE=-fprofile-use=.profile
PROFILE=

ifdef PROF
GPROF=-p
else
GPROF=
endif

DEFS=

ifdef RELEASE
SAN=
DEFS+=-DRELEASE
ifdef SMALL
OPTIM=-Os
DEFS+=-DSMALL
else
OPTIM=-O3
# Note: gcc 10.2.1-6 from Debian currently has an issue with my
# fib_with_registers code in -O2 or -O3 (probably because of goto out
# of nested scopes). Also, -O1 is sometimes faster than -O2 in the
# interpreter (while -O3 is yet a tiny bit faster); seems to depend a
# lot on random inlining/optimization trigger patterns.
endif
else
SAN=-fsanitize=undefined -fsanitize=address -fPIE -fno-omit-frame-pointer
#OPTIM=-Og
OPTIM=-O0
endif

ifdef FIXNUM_UNSAFE
DEFS+=-DFIXNUM_UNSAFE
endif

CFLAGS=-fdiagnostics-color -Wall -Wextra -gdwarf-4 -g3 -fverbose-asm -I.. $(OPTIM) $(PROFILE) $(GPROF) $(LTO) $(DEFS)
# How does one get macros in symbols from clang? -ggdb3 -fdebug-macro
# does not help either.

AFLAGS=$(PROFILE) $(GPROF) $(LTO)
LDFLAGS=$(PROFILE) $(GPROF) $(LTO)

CC=$(COMPILER) $(SAN) -S
AS=$(COMPILER) -c
LD=$(COMPILER) $(SAN)

OBJS=target-local/tod.o target-local/benchmark.o target-local/test.o target-local/vm_process.o target-local/vm_mem.o target-local/vm.o target-local/bytecode.o

all: .deps target-local run

run: target-local/vmtest
	target-local/vmtest

gdb: target-local/vmtest
	ASAN_OPTIONS=abort_on_error=1 gdbrun target-local/vmtest


.deps:
	../chj-ctest/bin/gendeps --run-tests -I . -I .. -I ../chj-ctest -I ../chj-64lib $(OBJS) target-local/run_tests.o target-local/vmtest.o > .deps.tmp
	mv .deps.tmp .deps

include .deps

clean:
	rm -rf target-local

target-local:
	mkdir target-local

target-local/vmtest: .deps target-local $(OBJS) target-local/run_tests.o target-local/vmtest.o
	$(LD) $(LDFLAGS) -o target-local/vmtest $(OBJS) target-local/run_tests.o target-local/vmtest.o

target-local/lilyvm: .deps target-local $(OBJS) target-local/lilyvm.o
	$(LD) $(LDFLAGS) -o target-local/lilyvm $(OBJS) target-local/lilyvm.o

target-local/%.o: target-local/%.s
	$(AS) $(AFLAGS) -o $@ $<

target-local/%.o: %.s
	$(AS) $(AFLAGS) -o $@ $<

target-local/%.s: %.c
	$(CC) $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<` -o $@ $<

# HACK
target-local/%.s: ../chj-ctest/%.c
	$(CC) $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<`  -o $@ $<
# HACK
target-local/%.o: ../chj-64lib/%.s
	$(AS) $(AFLAGS) -o $@ $<
target-local/%.s: ../chj-64lib/%.c
	$(CC) $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<`  -o $@ $<

_opcode_dispatch.h: opcodes.scm
	gsi opcodes.scm

opcode_constants.h: opcodes.scm
	gsi opcodes.scm

.PHONY: all .deps

.PRECIOUS: target-local/%.s
