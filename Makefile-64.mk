
CFLAGS=--target c64 --add-source -Oi -I..
AFLAGS=--target c64
LDFLAGS=--target c64 -v

OBJS=target/tod_init_s.o target/tod.o target/benchmark.o target/fastfill.o target/test.o target/mem_s.o target/mem.o target/vm.o target/run_tests.o target/foo.o

all: .deps target target/foo.prg

.deps:
	../chj-ctest/bin/gendeps -I . -I .. -I ../chj-ctest -I ../chj-64lib $(OBJS) > .deps.tmp
	mv .deps.tmp .deps

include .deps

run: target/foo.prg
	c64 -autostartprgmode 1 target/foo.prg

clean:
	rm -rf target

target:
	mkdir target

target/foo.prg: target $(OBJS)
	ld65 $(LDFLAGS) -Ln target/foo.labels -o target/foo.prg $(OBJS) c64.lib

target/%.o: target/%.s
	ca65 $(AFLAGS) -o $@ $<

target/%.o: %.s
	ca65 $(AFLAGS) -o $@ $<

target/%.s: %.c
	cc65 $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<` -o $@ $<

# HACK
target/%.s: ../chj-ctest/%.c
	cc65 $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<`  -o $@ $<
# HACK
target/%.o: ../chj-64lib/%.s
	ca65 -l $@.listing $(AFLAGS) -o $@ $<
target/%.s: ../chj-64lib/%.c
	cc65 $(CFLAGS) -DFIL=`../chj-ctest/bin/path-to-FIL $<`  -o $@ $<

_opcode_dispatch.h: opcodes.scm
	gsi opcodes.scm

opcode_constants.h: opcodes.scm
	gsi opcodes.scm

.PHONY: all .deps

.PRECIOUS: target/%.s
