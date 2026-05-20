#ifndef TORUS_COMPILER_H
#define TORUS_COMPILER_H 1

#define __always_inline inline __attribute__((__always_inline__))
#define __noinline      __attribute__((__noinline__))
#define __noreturn      __attribute__((__noreturn__))
#define __aligned(_a)   __attribute__((__aligned__(_a)))
#define __aligned_max   __attribute__((__aligned__))
#define __packed        __attribute__((__packed__))
#define __used          __attribute__((__used__))
#define __section(_s)   __attribute__((__section__(_s)))
#define __pure          __attribute__((__pure__))

#define fallthrough     __attribute__((__fallthrough__))

#define unreachable()   __builtin_unreachable()
#define likely(_c)      __builtin_expect(!!(_c), 1)
#define unlikely(_c)    __builtin_expect(!!(_c), 0)

#endif // TORUS_COMPILER_H
