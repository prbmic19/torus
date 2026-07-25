#ifndef TORUS_COMPILER_H
#define TORUS_COMPILER_H

#define __always_inline inline __attribute__((__always_inline__))
#define __noinline      __attribute__((__noinline__))
#define __noreturn      __attribute__((__noreturn__))
#define __aligned(_a)   __attribute__((__aligned__(_a)))
#define __aligned_max   __attribute__((__aligned__))
#define __packed        __attribute__((__packed__))
#define __used          __attribute__((__used__))
#define __unused        __attribute__((__unused__))
#define __section(_s)   __attribute__((__section__(_s)))
#define __pure          __attribute__((__pure__))
#define __const         __attribute__((__const__))
#define __nonnull(...)  __attribute__((__nonnull__(__VA_ARGS__)))
#define __printf(...)   __attribute__((__format__(__printf__, __VA_ARGS__)))
#define __must_check    __attribute__((__warn_unused_result))
#define __error_on_call(_msg) __attribute__((__error__(_msg)))

#define fallthrough     __attribute__((__fallthrough__))

#define unreachable()   __builtin_unreachable()
#define likely(_c)      __builtin_expect(!!(_c), 1)
#define unlikely(_c)    __builtin_expect(!!(_c), 0)

#endif // TORUS_COMPILER_H
