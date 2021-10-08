# LilyVM

What I can't recreate, I don't understand.

## Aims

Run Scheme and similar languages (and, just maybe, concatenative ones,
too?).

Small/adaptable, so that it can run on microcontrollers and is cheap
to load in the browser as WASM binary.

Precise memory management (do not retain memory in lazy code
(streams)).

Efficient call/cc.

Reasonably fast bytecode interpreter. Explore possible performance
improvements via combined ops, custom ops / compilation via C, using
local unsafety in provably safe ways, and partially using lowlevel
data formats in compiled programs (under manual direction) instead of
GC'd ones.

Concurrency: offer shared-nothing processes (CSP / Erlang style),
scheduled in user-space and perhaps (explicitly?) also across
pthreads. Secure separation.

Realtime: every process runs its own GC which can be interrupted by
the scheduler. Also, try to implement realtime GC (but explicit GC at
safe points might be good for a while).

Security: could I prove it? (When parsing or generating C, or parsing
the assembly?) If it can't be proven, then this project probably will
have to die (once it has shown me why I can't).

Parametrizable GC word width (at least 32 and 64, and currently 16
bits).

The project started with the aim to target the C-64 (with memory
extensions). LilyVM is currently using 16-bit words for that
reason. But the [cc65](https://cc65.github.io/doc/) compiler is
generating bloated and slow code, and even the current version of the
VM may not fit into the main RAM in the C-64 any more (I haven't
gotten around to test). So it may have to be rewritten in assembly,
and I may not follow through on that. (Would 65816 CPUs make for a
realistic target?)

## Status

This is work in progress.

GC works, but is a simple two-space (Cheney) algorithm (and doesn't
currently adapt heap size depending on requirements). Similarly, the
execution stack is currently just a fixed-size C array. Pairs,
characters, booleans and some other immediates, integers. Bignums are
supported, but only addition and comparison are implemented right now.

There are only some experimental opcodes implemented right now, I'll
add what I realize is needed when compiling from Scheme.

I don't know yet how error handling in the guest language and on the C
level should interoperate.

There is no bytecode verifier currently; and there are no safety
checks on jump target addresses since the assumption is that a
load-time verifier will exist at some point.

The `fib_register` opcode (which heavily uses computed goto, although
so does `fib_combinedop`) segfaults when compiling with g++, and
sometimes when compiling with gcc -O3. I don't know why. It works fine
with clang and clang++.

The 6502 adaptions are outdated and it won't currently compile for
that architecture.

The interpreter is currently running naive fibonacci about 5x faster
than CPython (`fib_registers_35.bytecode`, i.e. when assuming that the
fibonacci function can't be redefined while running, using register
ops, the vm is compiled with clang++, and running on an Intel i5-2520M
in 64-bit mode). Real programs will be slower than that in the end due
to added features (and possibly use of a larger word width), realtime
GC, and the compiler not generating bytecode as optimal as the
hand-coded examples (unless using controlled unsafety can make up for
the slowdown).

## Notes

The code is in roughly C89 since that's what cc65, the compiler for
the 6502 CPU, supports.

There is lots of shouting in the code due to using macros to pass
along an implicit `process` variable holding the context. This isn't
slower (and in fact appears to be a tad faster) than storing that
context in globals, and on the 6502 that context should be stored in
the zero page and updated on process context switches. But with
pthreads that wouldn't work either, thus macros it is (any other
idea?).

`bignum_add` is ugly.

## Usage

There are two executables, `vmtest` which runs the unit tests and
generates bytecode files, and `lilyvm` which can run existing bytecode
files.

The `gendeps` script in chj-ctest has a dependency on FunctionalPerl,
hence you'll need:

    sudo cpan install FunctionalPerl

or follow https://metacpan.org/pod/local::lib if you can't use
root. Then:

    git clone https://github.com/pflanze/chj-ctest
    git clone https://github.com/pflanze/chj-64lib
    git clone https://github.com/pflanze/lilyvm
    
I suggest you check the latest Git version tag signatures in each
repository via `git tag -v ...` at this point to verify that the
source is coming from me. Then:

    cd lilyvm
    touch *opcode*.h
    
The latter is to ensure that the generated files are newer, to avoid
make trying to regenerate them, which needs a Scheme system (see
below).

To run with debugging and ASAN enabled:

    make -f Makefile-local.mk clean run

To benchmark:

    RELEASE=1 make -f Makefile-local.mk clean target-local/{vmtest,lilyvm}
    target-local/vmtest

This generates some bytecode files (via calls to `bytecode_write_file`
in [`vmtest.c`](vmtest.c)). You can run those via:

    time target-local/lilyvm fib_35.bytecode
    time target-local/lilyvm fib_combinedop_35.bytecode
    time target-local/lilyvm fib_registers_35.bytecode
    time target-local/lilyvm fib_binaryregisters_35.bytecode
    time target-local/lilyvm fib_compiled_35.bytecode 
    time target-local/lilyvm fib_compiled_register_35.bytecode # segv with GCC

or also

    time target-local/lilyvm fib_registers_40.bytecode
    time target-local/lilyvm fib_binaryregisters_40.bytecode

There is currently no disassembler. To understand what those files do,
read the sections in [`vmtest.c`](vmtest.c) where the opcodes that are
stored to them are being written to memory, and then
[`normal-opcodes.scm`](normal-opcodes.scm) and
[`optim-opcodes.scm`](optim-opcodes.scm) to see how the opcodes are
implemented.

Run `target-local/lilyvm --help` for help on command line options.

Note that the result of the fibonacci calculation is a bignum (since
currently immediates are only 16 bits wide for the mentioned 16-bit
CPU target), and bignum division is not yet implemented and hence the
Scheme writer cannot print the numbers in decimal and thus shows them
in hex (like `#bignum{00e3-d1b0}` here for fib(35), which represents
the expected decimal result 14930352, or `#bignum{09de-8d6d}`
(165580141) for fib(40)).

For small optimized binaries, set `SMALL` in the environment. To
compile in C++ mode, set `CPLUSPLUS`. To use clang/clang++, set
`CLANG`. To enable execution tracing, set `VM_TRACE` (it needs to also
be enabled at runtime via the `trace_on` opcode). Set `SECURITY` if
you're interested in security over speed (although it probably won't
hurt the latter). Set `LTO` for link time optimization (although that
may make it slower).

All of those env variables are checked for their existence, not their
value.

(Note: currently the build system is buggy, `clean` is needed both for
the env var changes and in general for safety.)

A Scheme system is needed to regenerate the `*opcode*.h` files.  Any
R5RS compatible system should do (just run `opcodes.scm`). The
makefile runs Gambit when needed.

## License

Copyright (C) 2021 Christian Jaeger, <ch@christianjaeger.ch>

This is currently being published under the LGPL 2.1, but I can be
persuaded to add or change to other licenses.
