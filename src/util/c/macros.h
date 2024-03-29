#pragma once
#ifndef HGUARD__MACROS_H_
#define HGUARD__MACROS_H_
/****************************************************************************************/

#ifdef __cplusplus
# include <climits>
#else
# include <limits.h>
#endif

#if !defined __GNUC__ && !defined __clang__ && !defined __attribute__
# define __attribute__(x)
#endif
#ifndef __attr_access
# define __attr_access(x)
#endif
//#if !defined __declspec && !defined _MSC_VER
//# define __declspec(...)
//#endif
#ifndef __has_include
# define __has_include(x)
#endif
#if defined __GNUC__ && (defined __clang_major__ && __clang_major__ >= 14)
# define __attribute_error__(x) __attribute__((__error__(x)))
#else
# define __attribute_error__(x) DEPRECATED_MSG(x)
#endif

#ifdef __cplusplus
# if defined _MSC_VER && defined _MSVC_LANG
#  define CXX_LANG_VER _MSVC_LANG
# endif
# ifndef CXX_LANG_VER
#  define CXX_LANG_VER __cplusplus
# endif
#endif

#define HELPER_EXPAND_MACRO(macro) macro

#if (defined __cplusplus      && __cplusplus >= 201703L)  || \
    (defined CXX_LANG_VER     && CXX_LANG_VER >= 201703L) || \
    (defined __STDC_VERSION__ && __STDC_VERSION__ > 201710L)
# define UNUSED            [[maybe_unused]]
# define NODISCARD         [[nodiscard]]
# define FALLTHROUGH       [[fallthrough]]
# define NORETURN          [[noreturn]]
# define DEPRECATED        [[deprecated]]
# define DEPRECATED_MSG(x) [[deprecated(x)]]
#elif defined __GNUC__ || defined __clang__
# define UNUSED            __attribute__((__unused__))
# define NODISCARD         __attribute__((__warn_unused_result__))
# define FALLTHROUGH       __attribute__((__fallthrough__))
# define NORETURN          __attribute__((__noreturn__))
# define DEPRECATED        __attribute__((__deprecated__))
# define DEPRECATED_MSG(x) __attribute__((__deprecated__(x)))
#elif defined _MSC_VER
# define UNUSED            __pragma(warning(suppress : 4100 4101 4102))
# define NODISCARD         _Check_return_
# define FALLTHROUGH       __fallthrough
# define NORETURN          __declspec(noreturn)
# define DEPRECATED        __declspec(deprecated)
# define DEPRECATED_MSG(x) __declspec(deprecated(x))
#else
# define UNUSED
# define NODISCARD
# define NORETURN
# define FALLTHROUGH
# define DEPRECATED
# define DEPRECATED_MSG(x)
#endif

#if defined __GNUC__ || defined __clang__
# define UNREACHABLE __builtin_unreachable()
#elif defined _MSC_VER
# define UNREACHABLE __assume(0)
#else
# define UNREACHABLE() ((void)0)
#endif

/*--------------------------------------------------------------------------------------*/

#ifdef __cplusplus

# define DELETE_COPY_CTORS(t)               \
      t(t const &)                = delete; \
      t &operator=(t const &)     = delete

# define DELETE_MOVE_CTORS(t)               \
      t(t &&) noexcept            = delete; \
      t &operator=(t &&) noexcept = delete

# define DEFAULT_COPY_CTORS(t)               \
      t(t const &)                = default; \
      t &operator=(t const &)     = default

# define DEFAULT_MOVE_CTORS(t)               \
      t(t &&) noexcept            = default; \
      t &operator=(t &&) noexcept = default

# define DELETE_ALL_CTORS(t) \
      DELETE_COPY_CTORS(t);  \
      DELETE_MOVE_CTORS(t)

#define DEFAULT_ALL_CTORS(t) \
      DEFAULT_COPY_CTORS(t); \
      DEFAULT_MOVE_CTORS(t)

#define DUMP_EXCEPTION(e)                                                            \
      do {                                                                           \
            FILE *const stderr_file_ = stderr;                                       \
            fflush(stderr_file_);                                                    \
            fprintf(                                                                 \
                stderr_file_,                                                        \
                "\nCaught exception in function '%s', at line %d "                   \
                "'%s'\n\033[1;32mWhat <<_EOF_\033[0m\n%s\n\033[1;32m_EOF_\033[0m\n", \
                FUNCTION_NAME, __LINE__, __FILE__, (e).what());                      \
            fflush(stderr_file_);                                                    \
      } while (0)

# ifdef __TAG_HIGHLIGHT__
#  define REQUIRES(...)
# else
#  define REQUIRES(...) requires (__VA_ARGS__)
# endif

# ifndef NO_OBNOXIOUS_TWO_LETTER_CONVENIENCE_MACROS_PLEASE
#  define FC(str) FMT_COMPILE(str)
# endif

#else // not defined __cplusplus

# ifndef thread_local
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#   define thread_local _Thread_local
#  elif defined(_MSC_VER)
#   define thread_local __declspec(thread)
#  elif defined(__GNUC__)
#   define thread_local __thread
#  else
#   define thread_local
#  endif
# endif

# define C_STATIC_ASSERT_EXPR_(cond) ((void)((char [(-1) + (2 * (cond))]){}))

# ifndef static_assert
#  if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || defined(_MSC_VER)
#   define static_assert(...) _Static_assert(__VA_ARGS__)
#  else
#   define static_assert(cond, ...) C_STATIC_ASSERT_EXPR_(cond)
#  endif
# endif

# ifndef NORETURN
#  if defined __GNUC__ || defined __clang__
#   define NORETURN __attribute__((__noreturn__))
#  elif defined _MSC_VER
#   ifdef noreturn
#    undef noreturn
#   endif
#   define NORETURN __declspec(noreturn)
#  elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#   define NORETURN _Noreturn
#  else
#   define NORETURN
#  endif
# endif

#endif // defined __cplusplus

#ifndef NO_OBNOXIOUS_TWO_LETTER_CONVENIENCE_MACROS_PLEASE
# define ND NODISCARD
# define UU UNUSED
#endif

#if defined __GNUC__ || defined __clang__
# define NOINLINE __attribute__((__noinline__))
# ifndef __always_inline
#  define __always_inline __attribute__((__always_inline__)) __inline
# endif
# ifndef __forceinline
#  define __forceinline __always_inline
# endif
#elif defined _MSC_VER
# define NOINLINE __declspec(noinline)
#else
# define NOINLINE
# define __forceinline __inline
#endif

#ifndef __has_builtin
#  define __has_builtin(x) 0
#endif

/*--------------------------------------------------------------------------------------*/
/* Pragmas */

#ifdef _MSC_VER
# define PRAGMA_MSVC(...) __pragma(__VA_ARGS__)
#else
# define PRAGMA_MSVC(...)
#endif
#ifdef __clang__
# define PRAGMA_CLANG(...) _Pragma(__VA_ARGS__)
#else
# define PRAGMA_CLANG(...)
#endif
#ifdef __GNUC__
# define PRAGMA_GCC(...) _Pragma(__VA_ARGS__)
#else
# define PRAGMA_GCC(...)
#endif

/*--------------------------------------------------------------------------------------*/

#ifndef __WORDSIZE
# if SIZE_MAX == ULLONG_MAX
#  define __WORDSIZE 64
# elif SIZE_MAX == UINT_MAX
#  define __WORDSIZE 32
# elif SIZE_MAX == SHRT_MAX
#  define __WORDSIZE 16
# else
#  error "I have no useful warning message to give here. You know what you did."
# endif
#endif

#define P99_CAT2(_1, _2) _1 ## _2

#define P99_PASTE_0()
#define P99_PASTE_1(_1) _1
#define P99_PASTE_2(_1, _2) P99_CAT2(_1, _2)
#define P99_PASTE_3(_1, _2, _3) P99_PASTE_2(P99_PASTE_2(_1, _2), _3)
#define P99_PASTE_4(_1, _2, _3, _4) P99_PASTE_2(P99_PASTE_3(_1, _2, _3), _4)
#define P99_PASTE_5(_1, _2, _3, _4, _5) P99_PASTE_2(P99_PASTE_4(_1, _2, _3, _4), _5)
#define P99_PASTE_6(_1, _2, _3, _4, _5, _6) P99_PASTE_2(P99_PASTE_5(_1, _2, _3, _4, _5), _6)
#define P99_PASTE_7(_1, _2, _3, _4, _5, _6, _7) P99_PASTE_2(P99_PASTE_6(_1, _2, _3, _4, _5, _6), _7)
#define P99_PASTE_8(_1, _2, _3, _4, _5, _6, _7, _8) P99_PASTE_2(P99_PASTE_7(_1, _2, _3, _4, _5, _6, _7), _8)
#define P99_PASTE_9(_1, _2, _3, _4, _5, _6, _7, _8, _9) P99_PASTE_2(P99_PASTE_8(_1, _2, _3, _4, _5, _6, _7, _8), _9)
#define P99_PASTE_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10) P99_PASTE_2(P99_PASTE_9(_1, _2, _3, _4, _5, _6, _7, _8, _9), _10)
#define P99_PASTE_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) P99_PASTE_2(P99_PASTE_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10), _11)
#define P99_PASTE_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) P99_PASTE_2(P99_PASTE_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11), _12)
#define P99_PASTE_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) P99_PASTE_2(P99_PASTE_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12), _13)
#define P99_PASTE_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) P99_PASTE_2(P99_PASTE_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13), _14)
#define P99_PASTE_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) P99_PASTE_2(P99_PASTE_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14), _15)

#define P00_NUM_ARGS_b(_0a, _0b, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define P00_NUM_ARGS_a(...) P00_NUM_ARGS_b(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 0, 0)
#define P00_NUM_ARGS(...)   P00_NUM_ARGS_a(__VA_ARGS__)
#define P00_PASTE_a(n)      P99_CAT2(P99_PASTE_, n)
#define P00_PASTE(...)      P00_PASTE_a(P00_NUM_ARGS(__VA_ARGS__))

#define P99_PASTE(...) P00_PASTE(__VA_ARGS__)(__VA_ARGS__)

#define P00_PASTE_TEST_2(a, b, c, d, ...) P99_PASTE(a, b, c, d, __VA_ARGS__)
#define P00_PASTE_TEST_1(a, b, c, ...)    P00_PASTE_TEST_2(a, b, c, __VA_ARGS__)
#if P99_PASTE(0, x, A, F, 5, 1, 0, U, L, L) != 0xAF510ULL || P00_PASTE_TEST_1(0, x, 9, B, 7, F, A, 3, F, F, 0, 0, U, L, L) != 0x9B7FA3FF00ULL
# error "Your C pre-processor is broken or non-conformant. Cannot continue."
#endif
#undef P00_PASTE_TEST_1

#if !defined __BEGIN_DECLS || !defined __END_DECLS
# ifdef __cplusplus
#  define __BEGIN_DECLS extern "C" {
#  define __END_DECLS   }
# else
#  define __BEGIN_DECLS
#  define __END_DECLS
# endif
#endif

#define SIZE_C        P99_PASTE_3(UINT, __WORDSIZE, _C)
#define SSIZE_C       P99_PASTE_3(INT,  __WORDSIZE, _C)
#define SLS(s)        ("" s ""), (sizeof(s))
#define LSTRCPY(d, s) memcpy((d), SLS(s))
#define eprintf(...)  fprintf(stderr, __VA_ARGS__)

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#if defined _WIN64 && !defined PATH_MAX
# if WDK_NTDDI_VERSION >= NTDDI_WIN10_RS1
#  define PATH_MAX 4096
# else
#  define PATH_MAX MAX_PATH
# endif
#endif

#if defined __GNUC__ || defined __clang__
# define FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined _MSC_VER
# define FUNCTION_NAME __FUNCSIG__
#else
# define FUNCTION_NAME __func__
#endif

#ifdef _WIN32
# define SETERRNO(x) SetLastError(x)
#else
# define SETERRNO(x) ((void)(errno = (x)))
#endif

/*--------------------------------------------------------------------------------------*/

#define USECS_IN_SECOND 1000000ULL  /* 1,000,000 - One Million */
#define NSECS_IN_SECOND 1000000000ULL  /* 1,000,000,000 - One Billion */


/****************************************************************************************/
#endif // macros.h
// vim: ft=c
