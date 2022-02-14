#ifndef HGUARD_d_C_UTIL_HH_
#define HGUARD_d_C_UTIL_HH_
#pragma once
/****************************************************************************************/

#include "Common.hh"

#if defined __GNUC__ || defined __clang__
# ifdef __clang__
#  define ATTRIBUTE_PRINTF(...) __attribute__((__format__(__printf__, __VA_ARGS__)))
# else
#  define ATTRIBUTE_PRINTF(...) __attribute__((__format__(__gnu_printf__, __VA_ARGS__)))
# endif
#else
# define ATTRIBUTE_PRINTF(...)
#endif
#ifdef _MSC_VER
#else
# define _Maybenull_ //NOLINT
# ifndef _Check_return_
#  define _Check_return_ __attribute__((__warn_unused_result__)) //NOLINT
# endif
# ifndef _Printf_format_string_
#  define _Printf_format_string_
# endif
#endif
#define PFSAL _Printf_format_string_

#if !defined restrict && (defined __cplusplus || defined __RESHARPER__)
# define restrict __restrict
#endif


__BEGIN_DECLS


_Check_return_ char const *
my_strerror(errno_t errval, _Notnull_ char *buf, size_t buflen)
    __attribute__((__nonnull__(2)));


/**
 * \brief
 * Creates a pseudorandom filename. The buffer should be empty; no template is required.
 * Instead, provide the desired directory for the file and it a name will be created
 * there. Optionally, a prefix and/or suffix may be provided for the filename.
 *
 * NOTE: NOT FOR USE IN GENERATING FILENAMES TO BE USED DIRECTLY IN A WORLD WRITABLE
 * TEMPORARY DIRECTORY SUCH AS '/tmp`! This function is totally insecure and does not
 * check whether the filename is unique!
 *
 * It is only safe if the directory you provide has been created securely by a function
 * like mkdtemp, or otherwise guarantee that the file you're creating does not already
 * exist.
 *
 * \param buf An empty char buffer which should be of size PATH_MAX + 1.
 * \param dir The base directory for the file. May not be NULL. May be an empty string.
 * \param prefix Optional prefix for the filename. May be NULL.
 * \param suffix Optional suffix for the filename. May be NULL.
 * \return Length of the generated filename.
 */
extern size_t braindead_tempname(_Notnull_   char       *restrict buf,
                                 _Notnull_   char const *restrict dir,
                                 _Maybenull_ char const *restrict prefix,
                                 _Maybenull_ char const *restrict suffix)
    __attribute__((__nonnull__(1, 2)))
    __attr_access((__write_only__, 1)) __attr_access((__read_only__, 2))
    __attr_access((__read_only__,  3)) __attr_access((__read_only__, 4));

#ifndef HAVE_ASPRINTF
extern int asprintf(_Notnull_       char       **restrict destp,
                    _Notnull_ PFSAL char const  *restrict fmt,
                    ...)
    __attribute__((__nonnull__)) ATTRIBUTE_PRINTF(2, 3);
#endif
#ifdef HAVE_STRLCPY
__always_inline __attribute__((__artificial__))
size_t emlsp_strlcpy(_Notnull_ char *restrict dst,
                     _Notnull_ char const *restrict src,
                     size_t size)
    __attribute__((__nonnull__, __warn_unused_result__))
    __attr_access((__write_only__, 1))
    __attr_access((__read_only__, 2))
{
      return strlcpy(dst, src, size);
}
#else
extern size_t emlsp_strlcpy(_Notnull_ char       *restrict dst,
                            _Notnull_ char const *restrict src,
                            size_t size)
    __attribute__((__nonnull__, __warn_unused_result__))
    __attr_access((__write_only__, 1))
    __attr_access((__read_only__,  2));

# ifndef __cplusplus
#  define strlcpy(dst, src, size) emlsp_strlcpy((dst), (src), (size))
# endif
#endif

// Not listed by default, but still available.
//extern uint32_t cxx_random_device_get_random_val(void);

// Get random 32 bit value. Duh.
extern uint32_t cxx_random_engine_get_random_val_32(void);
// Get random 64 bit value. Duh.
extern uint64_t cxx_random_engine_get_random_val_64(void);


__END_DECLS


#undef PFSAL
#undef ATTRIBUTE_PRINTF

/****************************************************************************************/
#endif // c_util.h
// vim: ft=c
