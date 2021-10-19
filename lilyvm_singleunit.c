#define NOTEST 1

#define FIL DD_chjM64lib_tod_
#include <chj-64lib/tod.c>
#undef FIL

#define FIL DD_chjM64lib_benchmark_
#include <chj-64lib/benchmark.c>
#undef FIL

#define FIL DD_chjMctest_test_
#include <chj-ctest/test.c>
#undef FIL

#define FIL D_vm_process_
#include <vm_process.c>
#undef FIL

#define FIL D_vm_mem_
#include <vm_mem.c>
#undef FIL

#define FIL D_vm_
#include <vm.c>
#undef FIL

#define FIL D_bytecode_
#include <bytecode.c>
#undef FIL

/* #define FIL D_run_tests_ */
/* #include <run_tests.c> */
/* #undef FIL */

/* #define FIL D_vmtest_ */
/* #include <vmtest.c> */
/* #undef FIL */

#define FIL D_lilyvm_
#include <lilyvm.c>
#undef FIL
