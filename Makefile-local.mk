ifdef CPLUSPLUS
  ifdef CLANG
    COMPILER=clang++
  else
    COMPILER=g++
  endif
else
  ifdef CLANG
    COMPILER=clang
  else
    COMPILER=gcc
    # gcc -pedantic-errors conflicts with computed goto
  endif
endif

ASSEMBLER=gcc

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

ifdef VM_TRACE
  DEFS+=-DVM_TRACE
endif

# https://laptrinhx.com/recommended-compiler-and-linker-flags-for-gcc-2717284731/

ifdef SECURITY
  CFLAGS_SECURITY+=-D_FORTIFY_SOURCE=2
  ifdef CPLUSPLUS
    CFLAGS_SECURITY+=-D_GLIBCXX_ASSERTIONS
  endif
  CFLAGS_SECURITY+=-fstack-protector-strong
  CFLAGS_SECURITY+=-fno-strict-aliasing
  # Enable table-based thread cancellation
  CFLAGS_SECURITY+=-fexceptions
  ifdef CLANG
    # https://blog.llvm.org/posts/2021-01-05-stack-clash-protection/
    # XX add for recent enough versions
    #CFLAGS_SECURITY+=-fstack-clash-protection
  else
    # assumes GCC
    CFLAGS_SECURITY+=-fstack-clash-protection
  endif
endif
# Increased reliability of backtraces
CFLAGS_SECURITY_CHEAP+=-fasynchronous-unwind-tables
# Store compiler flags in debugging information
CFLAGS_SECURITY_CHEAP+=-grecord-gcc-switches
CFLAGS_SECURITY_CHEAP+=-Werror=format-security -Werror=implicit-function-declaration

# Control flow integrity protection
#CFLAGS_SECURITY+=-mcet -fcf-protection


CFLAGS=-fdiagnostics-color -Wall -Wextra -gdwarf-4 -g3 -fverbose-asm -I.. $(OPTIM) $(PROFILE) $(GPROF) $(LTO) $(CFLAGS_SECURITY) $(CFLAGS_SECURITY_CHEAP) $(DEFS)
# How does one get macros in symbols from clang? -ggdb3 -fdebug-macro
# does not help either.


AFLAGS=$(PROFILE) $(GPROF) $(LTO)
LDFLAGS=$(PROFILE) $(GPROF) $(LTO)

# https://laptrinhx.com/recommended-compiler-and-linker-flags-for-gcc-2717284731/

# Full ASLR for executables
ifndef CLANG
  LDFLAGS+=-fpie -Wl,-pie
  # XX clang: "relocation R_X86_64_32 against hidden symbol `__TMC_END__' can not be used when making a PIE object"
endif
# Detect and reject underlinking
LDFLAGS+=-Wl,-z,defs
# Disable lazy binding
LDFLAGS+=-Wl,-z,now
# Read-only segments after relocation
LDFLAGS+=-Wl,-z,relro


CC=$(COMPILER) $(SAN)
AS=$(ASSEMBLER) -c
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

target-local/%.o: %.s
	$(AS) $(AFLAGS) -o $@ $<

ifdef CLANG
# Does clang have issues with macros as symbols when going via .s? So,
# go direct.
target-local/%.o: %.c
	$(CC) -c $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<` -o $@ $<
else
target-local/%.s: %.c
	$(CC) -S $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<` -o $@ $<
target-local/%.o: target-local/%.s
	$(AS) $(AFLAGS) -o $@ $<
endif

# HACK (compile libs to target-local/, too)
ifdef CLANG
# ditto re macros
target-local/%.o: ../chj-ctest/%.c
	$(CC) -c $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<`  -o $@ $<
else
target-local/%.s: ../chj-ctest/%.c
	$(CC) -S $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<`  -o $@ $<
endif
# HACK (compile libs to target-local/, too)
target-local/%.o: ../chj-64lib/%.s
	$(AS) $(AFLAGS) -o $@ $<
ifdef CLANG
target-local/%.o: ../chj-64lib/%.c
	$(CC) -c $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<`  -o $@ $<
else
target-local/%.s: ../chj-64lib/%.c
	$(CC) -S $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<`  -o $@ $<
endif

_opcode_dispatch.h: opcodes.scm
	gsi opcodes.scm

opcode_constants.h: opcodes.scm
	gsi opcodes.scm

.PHONY: all .deps

.PRECIOUS: target-local/%.s
