#ifndef KERNEL_BUILD_BUG_H
#define KERNEL_BUILD_BUG_H

#include <torus/compiler.h>

#define static_assert(cond, msg) _Static_assert(cond, msg)

#define ASSERT_STRUCT_OFFSET(structure, member, expected_offset) \
    static_assert(offsetof(structure, member) == (expected_offset), "Offset of " #member " in " #structure " has changed.")

#ifdef __OPTIMIZE__
#   define __BUILD_BUG_ON_MSG(cond, msg, prefix, suffix) \
        do                                               \
        {                                                \
            extern void prefix ## suffix(void) __error_on_call(msg); \
            if (cond)                                    \
            {                                            \
                prefix ## suffix();                      \
            }                                            \
        }                                                \
        while (0)
#else
#   define __BUILD_BUG_ON_MSG(cond, msg, prefix, suffix)
#endif

#define _BUILD_BUG_ON_MSG(cond, msg, prefix, suffix) \
    __BUILD_BUG_ON_MSG(cond, msg, prefix, suffix)

#define BUILD_BUG_ON_MSG(cond, msg) \
    _BUILD_BUG_ON_MSG(cond, msg, __build_bug_, __COUNTER__)

#define BUILD_BUG_ON(cond) \
    BUILD_BUG_ON_MSG(cond, "BUILD_BUG_ON() failed: " #cond)

#define BUILD_BUG() \
    BUILD_BUG_ON_MSG(1, "BUILD_BUG() reached.")

#endif // KERNEL_BUILD_BUG_H
