#ifndef ERRORS_H_
#define ERRORS_H_

/*
  Errors:

  pointer 0 is returned, and vm_errno is set

#define IS_ERROR(x) (!(x))
extern int vm_errno;
#define ERRNO(x) vm_errno

#define OUT_OF_MEM_ERROR   1
*/
// for now just;
#define ERROR(...) DIE_(__VA_ARGS__)
#define ERROR_(x) DIE(x)
#define UNFINISHED DIE("unfinished")
#define XXX() DIE("XXX")



#endif /* ERRORS_H_ */
